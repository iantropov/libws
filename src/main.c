/*
 * main.c
 *
 *  Created on: Apr 8, 2011
 *      Author: ant
 */

#include "web_sockets.h"
#include "web_sockets_parser.h"
#include "web_sockets_parser_internal.h"

#include "stdio.h"

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
//
//#define MAX_CLIENTS 5

//int __sock_client;
//struct event __ev_read, __ev_write;
//struct evbuffer *__input_buffer;
//struct evbuffer *__output_buffer;
//u_char *__hash;
//
//struct bufferevent *__client_bufev;
//
///*
//HTTP/1.1 101 WebSocket Protocol Handshake
//Upgrade: WebSocket
//Connection: Upgrade
//Sec-WebSocket-Origin: http://example.com
//Sec-WebSocket-Location: ws://example.com/demo
//Sec-WebSocket-Protocol: sample
//
//8jKS'y:G*Co,Wxa-
// */
//void prepare_response(struct evbuffer *buf, u_char *hash, int sock)
//{
//	evbuffer_add_printf(buf, "HTTP/1.1 101 WebSocket Protocol Handshake\r\n");
//	evbuffer_add_printf(buf, "Upgrade: WebSocket\r\n");
//	evbuffer_add_printf(buf, "Connection: Upgrade\r\n");
//	evbuffer_add_printf(buf, "Sec-WebSocket-Origin: null\r\n");
//	evbuffer_add_printf(buf, "Sec-WebSocket-Location: ws://127.0.0.1:8081/\r\n");
////	evbuffer_add_printf(buf, "Sec-WebSocket-Protocol: sample\r\n");
//	evbuffer_add_printf(buf, "\r\n");
//
//	write(sock, hash, 16);
//
//	evbuffer_add(buf, hash, 16);
//}
//
//void readcb(struct bufferevent *bufev, void *arg)
//{
//	fprintf(stderr, "%s\n", bufev->input->buffer + 1);
//
//	bufferevent_write(bufev, bufev->input->buffer, bufev->input->off);
//
//	evbuffer_drain(bufev->input, bufev->input->off);
//}
//
//static void init_bufevs(int sock)
//{
//	__client_bufev = bufferevent_new(sock, readcb, NULL, NULL, NULL);
//	bufferevent_enable(__client_bufev, EV_READ);
//}
//
//void write_to(int sock_client, short type, void *arg)
//{
//	prepare_response(__output_buffer, __hash, sock_client);
//
//	evbuffer_write(__output_buffer, sock_client);
//
//	event_del(&__ev_write);
//
//	init_bufevs(sock_client);
//}
//
///*
//GET / HTTP/1.1
//Upgrade: WebSocket
//Connection: Upgrade
//Host: 127.0.0.1:8081
//Origin: null
//Sec-WebSocket-Key1: 2    s " 8 56l1 4f9 0 2H W1
//Sec-WebSocket-Key2: 3,  69-89#K } 5g  4 588>B
//
//k�����r}
//*/
//
//int get_number_from_string(char *str)
//{
//	int space_num = 0;
//	unsigned int raw_num = 0;
//	int rank = 1;
//
//	int len = strlen(str);
//	int start_pos = (int)(strstr(str, " ") - str);
//	int i;
//
//	for (i = len - 1; i > start_pos; i--) {
//		switch (str[i]) {
//			case '0' : raw_num += rank * 0; rank *= 10; break;
//			case '1' : raw_num += rank * 1; rank *= 10; break;
//			case '2' : raw_num += rank * 2; rank *= 10; break;
//			case '3' : raw_num += rank * 3; rank *= 10; break;
//			case '4' : raw_num += rank * 4; rank *= 10; break;
//			case '5' : raw_num += rank * 5; rank *= 10; break;
//			case '6' : raw_num += rank * 6; rank *= 10; break;
//			case '7' : raw_num += rank * 7; rank *= 10; break;
//			case '8' : raw_num += rank * 8; rank *= 10; break;
//			case '9' : raw_num += rank * 9; rank *= 10; break;
//			case ' ' : space_num++; break;
//		}
//	}
//
//	return raw_num / space_num;
//}
//
//u_char *md5_hash(int key_1, int key_2, char *value)
//{
//	u_char *hashed_value = (u_char *)malloc(16);
//	if (hashed_value == NULL) {
//		fprintf(stderr, "malloc : %s\n", __func__);
//		exit(EXIT_FAILURE);
//	}
//
//	int key_1_big_endian = htonl(key_1);
//	int key_2_big_endian = htonl(key_2);
//	memcpy(hashed_value, (char *)&key_1_big_endian, 4);
//	memcpy(hashed_value + 4, (char *)&key_2_big_endian, 4);
//	memcpy(hashed_value + 8, value, 8);
//
//	u_char *hash = (u_char *)malloc(16);
//	if (hash == NULL) {
//		fprintf(stderr, "malloc : %s\n", __func__);
//		exit(EXIT_FAILURE);
//	}
//
//	return MD5(hashed_value, 16, hash);
//}
//
//u_char *try_to_treat_handshake(struct evbuffer *buf)
//{
//	char *line;
//	int i;
//
//	for (i = 0; i < 5; i++) {
//		line = evbuffer_readline(buf);
//		fprintf(stderr, "%s\n", line);
//		free(line);
//	}
//
//	line = evbuffer_readline(buf);
//	fprintf(stderr, "%s\n", line);
//	int key_1 = get_number_from_string(line);
//	free(line);
//	line = evbuffer_readline(buf);
//	fprintf(stderr, "%s\n", line);
//	int key_2 = get_number_from_string(line);
//	free(line);
//
////	line = evbuffer_readline(buf);
////	free(line);
//
////	char *value = evbuffer_readline(buf);
//
//	u_char *val = (u_char *)malloc(8);
//	memcpy(val, buf->buffer + 2, 8);
//
//
//	u_char *hash = md5_hash(key_1, key_2, val);
//
//	free(val);
//
//	return hash;
//}
//
//void read_from(int sock_client, short type, void *arg)
//{
//	u_char buf[550];
//
//	int ret = read(sock_client, buf, 550);
//
//	evbuffer_add(__input_buffer, buf, ret);
//
//
////	evbuffer_read(__input_buffer, sock_client, -1);
//
//	__hash = try_to_treat_handshake(__input_buffer);
//
//	event_del(&__ev_read);
//	event_add(&__ev_write, NULL);
//}
//
//void accept_from(int sock, short type, void *arg)
//{
//	struct sockaddr_in sin_client;
//	int from_len = sizeof(sin_client);
//	__sock_client = accept(sock, (struct sockaddr *)&sin_client, (socklen_t *)&from_len);
//	event_set(&__ev_read, __sock_client, EV_READ | EV_PERSIST, read_from, NULL);
//	event_set(&__ev_write, __sock_client, EV_WRITE | EV_PERSIST, write_to, NULL);
//	event_add(&__ev_read, NULL);
//
//	__input_buffer = evbuffer_new();
//	__output_buffer = evbuffer_new();
//}
//
//int prepare_server_socket(int port)
//{
//	int sock_server = socket(AF_INET, SOCK_STREAM, 0);
//	if (sock_server == -1) {
//		return -1;
//	}
//	struct sockaddr_in sin_server;
//	memset(&sin_server, '\0', sizeof(struct sockaddr_in));
//	sin_server.sin_family = AF_INET;
//	sin_server.sin_port = htons(port);/*FIXME*/
//	sin_server.sin_addr.s_addr = htonl(INADDR_ANY);
//	if (bind(sock_server, (struct sockaddr *)&sin_server,
//							sizeof(struct sockaddr_in)) == -1) {
//		return -1;
//	}
//	if (listen(sock_server, MAX_CLIENTS) == -1) {
//		return -1;
//	}
//	if (fcntl(sock_server, F_SETFL, O_NONBLOCK) == -1) {
//		return -1;
//	}
//	return sock_server;
//}
//
//int prepare_server_event(struct event *event_server,
//		int sock_server, void *db)
//{
//	event_init();
//	event_set(event_server, sock_server, EV_READ | EV_PERSIST,
//					accept_from, db);
//	if (event_add(event_server, NULL) == -1) {
//		return -1;
//	}
//	return 0;
//}

void _handshake(struct list_head *h, u_char *uri, u_char *challenge, void *arg)
{
	int a = 5;
}

void _message(u_char *message, void *arg)
{
	int a = 5;
}

void _error(struct ws_parser_info *info, short what, void *arg)
{
	int a = 5;
}


void fail_unless(int bool, char *str)
{
	if (!bool)
		fprintf(stderr, "%s\n", str);
}

#define MESSAGE (u_char *)"Hello from parser"
#define CHALLENGE "12121212"
#define APP_URI "/demo_app"
#define HEADERS_NUM 6
#define CB_ARG_VALUE 0xdffd
#define HANDSHAKE_CALL 0x01
#define MESSAGE_CALL 0x02
#define ERROR_CALL 0x04

char *__header_keys [] = {"Upgrade", "Connection", "Host", "Origin", "Sec-WebSocket-Key1", "Sec-WebSocket-Key2"};
char *__header_values [] = {"WebSocket", "Upgrade", "127.0.0.1:8081", "null", "2    s \" 8 56l1 4f9 0 2H W1", "3,  69-89#K } 5g  4 588>B"};

struct ws_parser_info *__parser_info;
struct evbuffer *__buf;

int __call_info;
int __call_count;

int __waiting_error;

void check_arg(void *arg)
{
	fail_unless(arg == CB_ARG_VALUE, "error cb_arg");
}

void handshake(struct list_head *h, u_char *uri, u_char *challenge, void *arg)
{
	fail_unless(h != NULL, "parser_handshake_headers");
	fail_unless(challenge != NULL, "parser_handshake_challenge");

	check_arg(arg);

	fail_unless(strcmp(challenge, CHALLENGE) == 0, "unsuspected challenge");
	fail_unless(strcmp(uri, APP_URI) == 0, "unsuspected app_uri");

	struct key_value_entry *pos;
	int i = 0;
	list_for_each_entry(pos, h, list) {
		fail_unless(strcmp(pos->key, __header_keys[i]) == 0, "unsuspected header_key");
		fail_unless(strcmp(pos->value, __header_values[i]) == 0, "unsuspected header_value");
		i++;
	}

	__call_count++;
	__call_info |= HANDSHAKE_CALL;
}

void message(u_char *message, void *arg)
{
	fail_unless(message != NULL, "parser_message");

	check_arg(arg);

	fail_unless(strcmp(message, MESSAGE) == 0, "unsuspected message");

	__call_count++;
	__call_info |= MESSAGE_CALL;
}

void error(struct ws_parser_info *info, short what, void *arg)
{
	fail_unless(info != NULL, "parser_error_parser_info");
	fail_unless(what == __waiting_error, "parser_error_reason");

	__call_count++;
	__call_info |= ERROR_CALL;
}

void parse_part(struct ws_parser_info *info, struct evbuffer *buf, u_char *str)
{
	evbuffer_add(buf, str, strlen(str));
	ws_parser_parse(info, buf->buffer, buf->buffer + buf->off);
}

void parse_part_n(struct ws_parser_info *info, struct evbuffer *buf, u_char *str, size_t len)
{
	evbuffer_add(buf, str, len);
	ws_parser_parse(info, buf->buffer, buf->buffer + buf->off);
}

void parse_request_line(struct ws_parser_info *info, struct evbuffer *buf, u_char *uri)
{
    parse_part(info, buf, (u_char *)"GET ");

	parse_part(info, buf, APP_URI);

	parse_part(info, buf, " HTTP/1.1\r\n");
}

void parse_headers(struct ws_parser_info *info, struct evbuffer *buf, u_char **header_keys, u_char **header_values, int n)
{
	int i;
	for (i = 0; i < HEADERS_NUM; i++) {
		parse_part(info, buf, header_keys[i]);
		parse_part(info, buf, ": ");
		parse_part(info, buf, header_values[i]);
		parse_part(info, buf, "\r\n");
	}
}

void parse_message(struct ws_parser_info *parser_info, struct evbuffer *buf, u_char *message)
{
    parse_part_n(parser_info, buf, "\x00", 1);

    parse_part(parser_info, buf, message);

    parse_part_n(parser_info, buf, "\xff", 1);
}

void parse_closing_frame(struct ws_parser_info *parser_info, struct evbuffer *buf)
{
    parse_part_n(parser_info, __buf, "\xff\x00", 2);
}

void parse_challenge(struct ws_parser_info *parser_info, struct evbuffer *buf, u_char *challenge)
{
    parse_part(parser_info, buf, "\r\n");

    parse_part(parser_info, buf, challenge);
}

void setup()
{
	__parser_info = ws_parser_init(handshake, message, error, CB_ARG_VALUE);
    fail_unless(__parser_info != NULL, "ws_parser_init");

	__buf = evbuffer_new();

	__waiting_error = WS_PARSER_PARSE_ERROR;

	__call_count = __call_info = 0;
}

void teardown()
{
	ws_parser_destroy(__parser_info);

	evbuffer_free(__buf);
}


void test_handshake()
{
	__waiting_error = WS_PARSER_CLOSING_FRAME;

    parse_request_line(__parser_info, __buf, APP_URI);

    parse_headers(__parser_info, __buf, __header_keys, __header_values, HEADERS_NUM);

    parse_challenge(__parser_info, __buf, CHALLENGE);

    parse_message(__parser_info, __buf, MESSAGE);
    parse_message(__parser_info, __buf, MESSAGE);
    parse_message(__parser_info, __buf, MESSAGE);

    parse_closing_frame(__parser_info, __buf);

    fail_unless(__call_count == 5, "Not all cbs are called");
    fail_unless(__call_info & HANDSHAKE_CALL &&
				__call_info & ERROR_CALL &&
				__call_info & MESSAGE_CALL != 0, "Unsuspected cbs are called");
}

int main()
{
//	event_init();
//
//	struct ws_server *serv = ws_init(8082);
//	if (serv == NULL)
//		exit(EXIT_FAILURE);
//
//	ws_set_callback(serv, "/", NULL);
//
//	event_dispatch();
	setup();
	test_handshake();
	teardown();

	return 0;
}
