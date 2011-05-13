#include "check.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../../src/web_sockets.h"
#include "../../src/web_sockets_parser.h"
#include "../../src/web_sockets_parser_internal.h"

#define MESSAGE "Message"
#define CB_ARG_VALUE 0xdffd
#define HANDSHAKE_CALL 0x01
#define MESSAGE_CALL 0x02
#define ERROR_CALL 0x04

static struct ws_parser_info *__parser_info;
static struct evbuffer *__buf;

static int __call_info;
static int __call_count;

static int __waiting_error;
static int __waiting_call_count;
static int __waiting_call_info;


static void check_arg(void *arg)
{
	fail_unless(arg == (void *)CB_ARG_VALUE, "error cb_arg");
}

static void message(u_char *message, void *arg)
{
	fail_unless(message != NULL, "parser_message");

	check_arg(arg);

	fail_unless(strcmp(message, MESSAGE) == 0, "unsuspected message");

	__call_count++;
	__call_info |= MESSAGE_CALL;
}

static void error(struct ws_parser_info *info, short what, void *arg)
{
//	fprintf(stderr, "Call count - %d, reason - %d\n", __call_count, what);

	fail_unless(info != NULL, "parser_error_parser_info");
	fail_unless((what & __waiting_error) != 0, "parser_error_reason");

	__call_count++;
	__call_info |= ERROR_CALL;
}

static void parse_part(struct ws_parser_info *info, struct evbuffer *buf, u_char *str)
{
	evbuffer_add(buf, str, strlen(str));
	ws_parser_parse(info, buf->buffer, buf->buffer + buf->off);
}

static void parse_part_n(struct ws_parser_info *info, struct evbuffer *buf, u_char *str, size_t len)
{
	evbuffer_add(buf, str, len);
	ws_parser_parse(info, buf->buffer, buf->buffer + buf->off);
}

static void parse_message(struct ws_parser_info *parser_info, struct evbuffer *buf, u_char *message)
{
    parse_part_n(parser_info, buf, "\x00", 1);

    parse_part(parser_info, buf, message);

    parse_part_n(parser_info, buf, "\xff", 1);
}

static void parse_closing_frame(struct ws_parser_info *parser_info, struct evbuffer *buf)
{
    parse_part_n(parser_info, __buf, "\xff\x00", 2);
}

static void setup()
{
	event_init();

	__parser_info = ws_parser_init(message, error, (void *)CB_ARG_VALUE);
    fail_unless(__parser_info != NULL, "ws_parser_init");

	__buf = evbuffer_new();

	__waiting_error = WS_PARSE_ERROR;

	__call_count = __call_info = 0;
	__waiting_call_info = __waiting_call_count = 0;
}

static void teardown()
{
	event_dispatch();

	ws_parser_destroy(__parser_info);

	evbuffer_free(__buf);

	fail_unless(__waiting_call_count == __call_count, "Unsuspected count of cbs");
    fail_unless((__call_info & __waiting_call_info) != 0, "Unsuspected cbs are called");
}

START_TEST(test_ws_success_0)
{
	__waiting_error = WS_CLOSING_FRAME;

    parse_message(__parser_info, __buf, MESSAGE);

    parse_closing_frame(__parser_info, __buf);


    __waiting_call_count = 2;
    __waiting_call_info = ERROR_CALL | MESSAGE_CALL;
}
END_TEST

START_TEST(test_ws_error_0)
{
	parse_part(__parser_info, __buf, "HET");

    __waiting_call_count = 1;
    __waiting_call_info = ERROR_CALL;
}
END_TEST

START_TEST(test_ws_error_1)
{
    parse_message(__parser_info, __buf, MESSAGE);

    parse_part(__parser_info, __buf, "cdcd");

    __waiting_call_count = 2;
    __waiting_call_info = ERROR_CALL | MESSAGE_CALL;
}
END_TEST

START_TEST(test_ws_error_2)
{
    parse_message(__parser_info, __buf, MESSAGE);

    __waiting_error = WS_CLOSING_FRAME;
    parse_closing_frame(__parser_info, __buf);

    event_dispatch();

    __waiting_error = WS_PARSE_ERROR;
    parse_message(__parser_info, __buf, MESSAGE);

    __waiting_call_count = 3;

    __waiting_call_info =  ERROR_CALL | MESSAGE_CALL;
}
END_TEST

START_TEST(test_ws_success_1)
{
	__waiting_error = WS_CLOSING_FRAME;

    parse_closing_frame(__parser_info, __buf);

    __waiting_call_count = 1;
    __waiting_call_info = ERROR_CALL;
}
END_TEST

START_TEST(test_ws_success_2)
{
	__waiting_error = WS_CLOSING_FRAME;

    parse_message(__parser_info, __buf, MESSAGE);
    parse_message(__parser_info, __buf, MESSAGE);
    parse_message(__parser_info, __buf, MESSAGE);

    __waiting_call_count = 3;
    __waiting_call_info =  ERROR_CALL | MESSAGE_CALL;
}
END_TEST

static TCase *ws_parser_tcase()
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
