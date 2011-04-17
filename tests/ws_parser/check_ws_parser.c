#include "check.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../src/web_sockets.h"
#include "../../src/web_sockets_parser.h"
#include "../../src/web_sockets_parser_internal.h"

#include "../../src/list.h"

#define MESSAGE "Hello from parser"
#define CHALLENGE "12121212"
#define CB_ARG_VALUE 7
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
    parse_part(info, buf, "GET ");

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

START_TEST(test_ws_success_0)
{
	__waiting_error = WS_PARSER_CLOSING_FRAME;

    parse_request_line(__parser_info, __buf, APP_URI);

    parse_headers(__parser_info, __buf, __header_keys, __header_values, HEADERS_NUM);

    parse_challenge(__parser_info, __buf, CHALLENGE);

    parse_message(__parser_info, __buf, MESSAGE);

    parse_closing_frame(__parser_info, __buf);

    fail_unless(__call_count == 3, "Not all cbs are called");
}
END_TEST


START_TEST(test_ws_error_0)
{
	parse_part(__parser_info, __buf, "HET");

    fail_unless(__call_count == 1, "Not all cbs are called");
}
END_TEST

START_TEST(test_ws_error_1)
{
	parse_part(__parser_info, __buf, "GET / Hj");

    fail_unless(__call_count == 1, "Not all cbs are called");
}
END_TEST

START_TEST(test_ws_error_2)
{
    parse_request_line(__parser_info, __buf, APP_URI);

    parse_part(__parser_info, __buf, "qwe\r\n");

    fail_unless(__call_count == 1, "Not all cbs are called");
}
END_TEST

START_TEST(test_ws_success_1)
{
    parse_request_line(__parser_info, __buf, APP_URI);

    parse_challenge(__parser_info, __buf, CHALLENGE);

    parse_part_n(__parser_info, __buf, "\x00", 1);

    fail_unless(__call_count == 1, "Not all cbs are called");
    fail_unless(__call_info & HANDSHAKE_CALL != 0, "Not all cbs are called");
}
END_TEST

START_TEST(test_ws_success_2)
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
    fail_unless(__call_info & HANDSHAKE_CALL & ERROR_CALL & MESSAGE_CALL != 0, "Unsuspected cbs are called");
}
END_TEST

TCase *ws_parser_tcase()
{
	TCase *tc_ws_parser = tcase_create ("web_sockets_parser");
	tcase_add_checked_fixture(tc_ws_parser, setup, teardown);

	tcase_add_test (tc_ws_parser, test_ws_success_0);
	tcase_add_test (tc_ws_parser, test_ws_success_1);
	tcase_add_test (tc_ws_parser, test_ws_success_2);

	tcase_add_test (tc_ws_parser, test_ws_error_0);
	tcase_add_test (tc_ws_parser, test_ws_error_1);
	tcase_add_test (tc_ws_parser, test_ws_error_2);

	return tc_ws_parser;
}

Suite *make_ws_parser_suite (void)
{
	Suite *s = suite_create ("web_sockets_parser");

	suite_add_tcase (s, ws_parser_tcase());
	
	return s;
}
