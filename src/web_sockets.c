/*
 * web_sockets.c
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#include "web_sockets.h"
#include "web_sockets_parser.h"

#include <event0/bufevent.h>
#include <event0/evhttp.h>

#include <stdio.h>
#include <openssl/md5.h>
#include "netinet/in.h"
#include <event.h>

#define MAX_CLIENTS 5
#define CHALLENGE_REQUEST_SIZE 8
#define CHALLENGE_RESPONSE_SIZE 16

#define WS_SCHEME "ws://"
#define WSS_SCHEME "wss://"

struct ws_connection {
	struct bufevent *bufev;
	struct ws_parser_info *info;

	ws_message_cb mes_cb;
	ws_error_cb err_cb;
	void *cb_arg;
};

struct ws_accepter {
	struct evhttp *eh;
	char *uri;

	ws_accept_cb a_cb;
	void *arg;
};

static int get_number_from_string(char *str);
static u_char *md5_hash_challenge(int key_1, int key_2, u_char *value);
static char *ws_get_location(struct evhttp_request *req, char *location, char *uri);

static int ws_send_handshake_response(struct evhttp_request *req, char *origin, char *location, u_char *hash)
{
	evhttp_add_header(req->output_headers, "Upgrade", "WebSocket");
	evhttp_add_header(req->output_headers, "Connection", "Upgrade");
	evhttp_add_header(req->output_headers, "Sec-WebSocket-Origin", origin);

	char *sec_location = ws_get_location(req, location, req->uri);
	if (sec_location == NULL)
		return -1;

	evhttp_add_header(req->output_headers, "Sec-WebSocket-Location", sec_location);
	free(sec_location);

	struct evbuffer *buf = evbuffer_new();
	if (buf == NULL)
		return -1;

	evbuffer_add(buf, hash, CHALLENGE_RESPONSE_SIZE);

	evhttp_send_reply(req, 101, "WebSocket Protocol Handshake", buf);

	evbuffer_free(buf);

	return 0;
}

static u_char *ws_prepare_handshake_response(char *s_key_1, char *s_key_2, u_char *hash)
{
	int key_1 = get_number_from_string(s_key_1);
	int key_2 = get_number_from_string(s_key_2);

	return md5_hash_challenge(key_1, key_2, hash);
}

void ws_connection_free(struct ws_connection *conn)
{
	if (conn->bufev != NULL)
		bufevent_free(conn->bufev);

	ws_parser_destroy(conn->info);

	free(conn);
}

static void ws_parser_handle_message(u_char *message, void *arg)
{
	struct ws_connection *conn = (struct ws_connection *)arg;

	if (conn->mes_cb != NULL)
		conn->mes_cb(conn, message, conn->cb_arg);
}

static void ws_error(struct ws_connection *conn, short what)
{
	if (conn->err_cb != NULL)
		conn->err_cb(conn, what, conn->cb_arg);
}

static void ws_parser_handle_error(struct ws_parser_info *info, short what, void *arg)
{
	ws_error((struct ws_connection *)arg, what);
}

void ws_connection_set_cbs(struct ws_connection *conn,
		ws_message_cb mes_cb, ws_error_cb err_cb, void *arg)
{
	conn->mes_cb = mes_cb;
	conn->err_cb = err_cb;

	conn->cb_arg = arg;
}

static void ws_bufevent_readcb(struct bufevent *bufev, void *arg)
{
	struct ws_connection *conn = (struct ws_connection *)arg;
	struct evbuffer *buf = bufevent_get_input(bufev);

	ws_parser_parse(conn->info, buf->buffer, buf->buffer + buf->off);

	evbuffer_drain(buf, ws_parser_drain(conn->info));
}

static void ws_bufevent_errorcb(struct bufevent *bufev, short what, void *arg)
{
	ws_error((struct ws_connection *)arg, what);
}

struct ws_connection *ws_connection_new(struct bufevent *bufev,
		ws_message_cb mes_cb, ws_error_cb err_cb, void *arg)
{
	struct ws_parser_info *info = NULL;
	struct ws_connection *conn = (struct ws_connection *)calloc(1, sizeof(struct ws_connection));
	if (conn == NULL)
		goto error;

	info = ws_parser_init(ws_parser_handle_message, ws_parser_handle_error, conn);
	if (info == NULL)
		goto error;

	conn->info = info;

	conn->bufev = bufev;
	bufevent_setcb(bufev, ws_bufevent_readcb, NULL, ws_bufevent_errorcb, conn);

	ws_connection_set_cbs(conn, mes_cb, err_cb, arg);

	return conn;
error:
	free(conn);
	ws_parser_destroy(info);

	return NULL;
}

static void ws_after_handshake(struct bufevent *bufev, void *arg)
{
	struct ws_accepter *wa = arg;

	wa->a_cb(wa, bufev, wa->arg);
}

static void ws_handshakecb(struct evhttp_request *req, void *arg)
{
	struct evkeyvalq *headers = req->input_headers;

	char *sec_key_1 = (char *)evhttp_find_header(headers, "Sec-WebSocket-Key1");
	if (sec_key_1 == NULL)
		goto error;

	char *sec_key_2 = (char *)evhttp_find_header(headers, "Sec-WebSocket-Key2");
	if (sec_key_2 == NULL)
		goto error;

	char *origin = (char *)evhttp_find_header(headers, "Origin");
	if (origin == NULL)
		goto error;

	char *location = (char *)evhttp_find_header(headers, "Host");
	if (location == NULL)
		goto error;

	u_char *challenge = EVBUFFER_DATA(req->input_buffer);
	if (challenge == NULL)
		goto error;

	u_char *hash = ws_prepare_handshake_response(sec_key_1, sec_key_2, challenge);

	evhttp_connection_upgrade(req->evcon, ws_after_handshake, arg);

	if (ws_send_handshake_response(req, origin, location, hash) == -1)
		goto error;

	free(hash);

	return;
error:
	free(hash);

	evhttp_send_error(req, 500, "Internal Error");
}

int ws_connection_send_message(struct ws_connection *conn, u_char *message)
{
	if (conn->bufev == NULL)
		return -1;

	if (bufevent_write(conn->bufev, "\x00", 1) == -1)
		return -1;

	if (bufevent_write(conn->bufev, message, strlen((char *)message)) == -1)
		return -1;

	if (bufevent_write(conn->bufev, "\xff", 1) == -1)
		return -1;

	return 0;
}

//struct bufevent *ws_get_bufevent(struct ws_connection *conn)
//{
//	return conn->bufev;
//}

int ws_connection_send_close(struct ws_connection *conn)
{
	if (conn->bufev == NULL)
		return -1;

	if (bufevent_write(conn->bufev, "\xff", 1) == -1)
		return -1;

	if (bufevent_write(conn->bufev, "\x00", 1) == -1)
		return -1;

	return 0;
}

struct ws_accepter *ws_accepter_new(struct evhttp *eh, char *uri, ws_accept_cb ac_cb, void *arg)
{
	struct ws_accepter *wa = (struct ws_accepter *)calloc(1, sizeof(struct ws_accepter));
	if (wa == NULL)
		return NULL;

	wa->uri = strdup(uri);
	if (wa->uri == NULL) {
		free(wa);
		return NULL;
	}

	wa->eh = eh;
	wa->a_cb = ac_cb;
	wa->arg = arg;

	evhttp_set_cb(eh, uri, ws_handshakecb, wa);

	return wa;
}

void ws_accepter_free(struct ws_accepter *wa)
{
	evhttp_del_cb(wa->eh, wa->uri);

	free(wa->uri);
	free(wa);
}

/*-----------------HELPER FUNCTIONS------------------------*/

static u_char *md5_hash_challenge(int key_1, int key_2, u_char *value)
{
	u_char *hashed_value = (u_char *)malloc(CHALLENGE_RESPONSE_SIZE);
	if (hashed_value == NULL)
		return NULL;

	int key_1_big_endian = htonl(key_1);
	int key_2_big_endian = htonl(key_2);
	memcpy(hashed_value, (char *)&key_1_big_endian, sizeof(int));
	memcpy(hashed_value + sizeof(int), (char *)&key_2_big_endian, sizeof(int));
	memcpy(hashed_value + CHALLENGE_REQUEST_SIZE, value, CHALLENGE_REQUEST_SIZE);

	u_char *hash = (u_char *)malloc(CHALLENGE_RESPONSE_SIZE);
	if (hash == NULL)
		goto exit;

	MD5(hashed_value, CHALLENGE_RESPONSE_SIZE, hash);

exit :
	free(hashed_value);
	return hash;
}

static char *ws_get_location(struct evhttp_request *req, char *location, char *uri)
{
	char *ws_scheme = evhttp_connection_is_secure(req->evcon) ? WSS_SCHEME : WS_SCHEME;
	int len = strlen(location) + strlen(uri) + strlen(ws_scheme) + 1;
	char *res = (char *)calloc(len, sizeof(char));
	if (res == NULL)
		return NULL;

	sprintf(res, "%s%s%s", ws_scheme, location, uri);

	return res;
}

static int get_number_from_string(char *str)
{
	int space_num = 0;
	unsigned int raw_num = 0;
	int rank = 1;

	int len = strlen(str);
	int i;
	for (i = len - 1; i >= 0; i--) {
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
