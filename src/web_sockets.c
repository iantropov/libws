/*
 * web_sockets.c
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#include "web_sockets.h"
#include "list.h"

#include <stdio.h>
#include <openssl/md5.h>
#include "sys/socket.h"
#include "sys/types.h"
#include "netinet/in.h"
#include "memory.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <event.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_CLIENTS 5

struct cb_entry {
	struct list_head list;

	char *uri;
	ws_cb *cb;
};

struct ws_server {
	struct list_head callbacks;

	struct event ev_accept;
};

struct ws_connection {
	char *uri;
	struct bufferevent *bufev;

	ws_cb *cb;
};


static char *string_copy(char *source);

static int handshake_response(struct bufferevent *bufev, u_char *hash)
{
	struct evbuffer *buf = evbuffer_new();
	if (buf == NULL)
		return -1;

	evbuffer_add_printf(buf, "HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
	evbuffer_add_printf(buf, "Upgrade: WebSocket\r\n");
	evbuffer_add_printf(buf, "Connection: Upgrade\r\n");
	evbuffer_add_printf(buf, "Sec-WebSocket-Origin: null\r\n");
	evbuffer_add_printf(buf, "Sec-WebSocket-Location: ws://192.168.0.100:8082/\r\n");
	evbuffer_add_printf(buf, "\r\n");
	evbuffer_add(buf, hash, 16);

	int ret = bufferevent_write_buffer(bufev, buf);

	evbuffer_free(buf);

	return ret;
}

static int get_number_from_string(char *str)
{
	int space_num = 0;
	unsigned int raw_num = 0;
	int rank = 1;

	int len = strlen(str);
	int start_pos = (int)(strstr(str, " ") - str);
	int i;

	for (i = len - 1; i > start_pos; i--) {
		switch (str[i]) {
			case '0' : raw_num += rank * 0; rank *= 10; break;
			case '1' : raw_num += rank * 1; rank *= 10; break;
			case '2' : raw_num += rank * 2; rank *= 10; break;
			case '3' : raw_num += rank * 3; rank *= 10; break;
			case '4' : raw_num += rank * 4; rank *= 10; break;
			case '5' : raw_num += rank * 5; rank *= 10; break;
			case '6' : raw_num += rank * 6; rank *= 10; break;
			case '7' : raw_num += rank * 7; rank *= 10; break;
			case '8' : raw_num += rank * 8; rank *= 10; break;
			case '9' : raw_num += rank * 9; rank *= 10; break;
			case ' ' : space_num++; break;
		}
	}

	return raw_num / space_num;
}

static u_char *md5_hash_challenge(int key_1, int key_2, u_char *value)
{
	u_char *hashed_value = (u_char *)malloc(16);
	if (hashed_value == NULL) {
		fprintf(stderr, "malloc : %s\n", __func__);
		exit(EXIT_FAILURE);
	}

	int key_1_big_endian = htonl(key_1);
	int key_2_big_endian = htonl(key_2);
	memcpy(hashed_value, (char *)&key_1_big_endian, 4);
	memcpy(hashed_value + 4, (char *)&key_2_big_endian, 4);
	memcpy(hashed_value + 8, value, 8);

	u_char *hash = (u_char *)malloc(16);
	if (hash == NULL) {
		fprintf(stderr, "malloc : %s\n", __func__);
		exit(EXIT_FAILURE);
	}

	MD5(hashed_value, 16, hash);

	free(hashed_value);

	return hash;
}


static u_char *try_to_treat_handshake(struct evbuffer *buf)
{
	char *line;
	int i;

	for (i = 0; i < 5; i++) {
		line = evbuffer_readline(buf);
		free(line);
	}

	line = evbuffer_readline(buf);
	int key_1 = get_number_from_string(line);
	free(line);

	line = evbuffer_readline(buf);
	int key_2 = get_number_from_string(line);
	free(line);

	u_char *val = (u_char *)malloc(8);
	memcpy(val, buf->buffer + 2, 8);

	u_char *hash = md5_hash_challenge(key_1, key_2, val);

	free(val);

	return hash;
}

static void ws_read_message(struct bufferevent *bufev, void *arg)
{


	bufev->input->buffer++;
	u_char c = bufev->input->buffer[bufev->input->off - 2];
	bufev->input->buffer[bufev->input->off - 2] = '\0';
	fprintf(stderr, "%s\n", bufev->input->buffer);
	bufev->input->buffer[bufev->input->off - 2] = c;
	bufev->input->buffer--;

	bufferevent_write(bufev, bufev->input->buffer, bufev->input->off);

	evbuffer_drain(bufev->input, bufev->input->off);
}

static void ws_close_connection(struct bufferevent *bufev, short what, void *arg)
{
	struct ws_connection *conn = (struct ws_connection *)arg;

	bufferevent_free(conn->bufev);

	free(conn);
}

static ws_cb *ws_server_get_cb(struct ws_server *serv, char *uri)
{
	return NULL;
}

static int ws_open_connection(struct bufferevent *bufev, struct ws_server *serv)
{
	struct ws_connection *conn = (struct ws_connection *)malloc(sizeof(struct ws_connection));
	if (conn == NULL)
		return -1;

	conn->bufev = bufev;

	bufferevent_setcb(bufev, ws_read_message, NULL, ws_close_connection, conn);

	conn->cb = ws_server_get_cb(serv, "/");

	return 0;
}

static void ws_read_handshake(struct bufferevent *bufev, void *arg)
{
	struct ws_server *serv = (struct ws_server *)arg;

	u_char *hash = try_to_treat_handshake(bufev->input);

	evbuffer_drain(bufev->input, bufev->input->off);

	handshake_response(bufev, hash);

	free(hash);

	ws_open_connection(bufev, serv);
}

static void ws_error_handshake(struct bufferevent *bufev, short what, void *arg)
{
	bufferevent_free(bufev);
}

static void ws_accept(int sock, short type, void *arg)
{
	struct sockaddr_in sin_client;
	int from_len = sizeof(sin_client);
	int sock_client = accept(sock, (struct sockaddr *)&sin_client, (socklen_t *)&from_len);

	struct bufferevent *bufev = bufferevent_new(sock_client, ws_read_handshake, NULL, ws_error_handshake, arg);
	if (bufev == NULL)
		return;

	bufferevent_enable(bufev, EV_READ);
	bufferevent_enable(bufev, EV_WRITE);
}

static int prepare_server_socket(int port)
{
	int sock_server = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_server == -1) {
		return -1;
	}
	struct sockaddr_in sin_server;
	memset(&sin_server, '\0', sizeof(struct sockaddr_in));
	sin_server.sin_family = AF_INET;
	sin_server.sin_port = htons(port);/*FIXME*/
	sin_server.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock_server, (struct sockaddr *)&sin_server,
							sizeof(struct sockaddr_in)) == -1) {
		return -1;
	}
	if (listen(sock_server, MAX_CLIENTS) == -1) {
		return -1;
	}
	if (fcntl(sock_server, F_SETFL, O_NONBLOCK) == -1) {
		return -1;
	}
	return sock_server;
}


static int prepare_server_event(struct event *event_server,
		int sock_server, void *arg)
{
	event_init();
	event_set(event_server, sock_server, EV_READ | EV_PERSIST,
					ws_accept, arg);
	if (event_add(event_server, NULL) == -1) {
		return -1;
	}
	return 0;
}

struct ws_server *ws_init(int port)
{
	struct ws_server *serv = (struct ws_server *)malloc(sizeof(struct ws_server));
	if (serv == NULL)
		return NULL;

	INIT_LIST_HEAD(&serv->callbacks);

	int sock = prepare_server_socket(port);
	if (sock == -1)
		goto error;

	int ret = prepare_server_event(&serv->ev_accept, sock, serv);
	if (ret != 0)
		goto error;

	signal(SIGPIPE, SIG_IGN);

	return serv;

error:
	free(serv);
	return NULL;
}

int ws_set_callback(struct ws_server *serv, char *uri, ws_cb *cb)
{
	//check already exist
	struct cb_entry *e = (struct cb_entry *)malloc(sizeof(struct cb_entry));
	if (e == NULL)
		return -1;

	e->uri = string_copy(uri);
	if (e->uri == NULL)
		goto error;

	e->cb = cb;

	list_add(&e->list, &serv->callbacks);

	return 0;

error:
	free(e);
	return -1;
}

void ws_remove_callback(struct ws_server *serv, char *uri)
{
	//check and remove
}

void ws_destroy(struct ws_server);
int ws_send(struct ws_connection conn, u_char *message);


static char *string_copy(char *source)
{
	if (source == NULL)
		return NULL;

	int len = strlen(source);
	char *dest = (char *)malloc(len + 1);
	if (dest == NULL)
		return NULL;

	return strcpy(dest, source);
}
