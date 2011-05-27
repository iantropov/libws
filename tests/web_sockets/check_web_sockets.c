#include "check.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <event0/bufevent.h>
#include <event.h>

#include "../../src/web_sockets.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

#define WS_URI "/"
#define HTTP_PORT 7777
#define HTTP_HOST "127.0.0.1"

#define CB_ARG_VALUE 0xdffd
#define MESSAGE "Hello"

static struct evhttp *__eh;
static struct bufevent *__client_bufev;
static int __client_sock;
static const struct timeval __tv = {0, 100000};

static void (*__commands[100])();
static int __command_counter = 0;
static int __waiting_command_counter;
static u_char *__message;
static short __what;
static struct ws_connection *__ws_conn;
static struct ws_accepter *__ws_accepter;

static int connect_to_port(int port)
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in sin_server;
	struct hostent *host_serv;
	memset(&sin_server, '\0', sizeof(sin_server));
	sin_server.sin_family = AF_INET;
	sin_server.sin_port = htons(port);
	host_serv = gethostbyname("localhost");
	memcpy((char *)&sin_server.sin_addr, host_serv->h_addr_list[0], host_serv->h_length);
	if (connect(sock, (struct sockaddr *)&sin_server, sizeof(sin_server)) == -1){
		perror("connect");
		return -1;
	}
	return sock;
}

static int process_commands(void(**commands)(), int command_counter)
{
	void(*counter)();
	for (counter = commands[command_counter++];counter != NULL; counter = commands[command_counter++])
		counter();

	return command_counter;
}

static void start_process_commands()
{
	__command_counter = process_commands(__commands, __command_counter);
	event_dispatch();
}

static void add_command(void (*cb)())
{
	__commands[__waiting_command_counter++] = cb;
}

static void check_arg(void *arg)
{
	fail_unless(arg == (void *)CB_ARG_VALUE, "error cb_arg");
}

static void ws_messagecb(struct ws_connection *conn, u_char *mes, void *arg)
{
	check_arg(arg);

	__message = mes;
	__ws_conn = conn;
	__command_counter = process_commands(__commands, __command_counter);
}

static void ws_errorcb(struct ws_connection *conn, short what, void *arg)
{
	check_arg(arg);

	__what = what;
	__ws_conn = conn;
	__command_counter = process_commands(__commands, __command_counter);
}

static void ws_acceptcb(struct ws_accepter *wa, struct bufevent *bufev, void *arg)
{
	check_arg(arg);

	__ws_conn = ws_connection_new(bufev, ws_messagecb, ws_errorcb, (void *)CB_ARG_VALUE);
	fail_unless(__ws_conn != NULL, "ws_new failed");

	__command_counter = process_commands(__commands, __command_counter);
}

static void prepare_server_side()
{
	__eh = evhttp_start(HTTP_HOST, HTTP_PORT);

	__ws_accepter = ws_accepter_new(__eh, WS_URI, ws_acceptcb, (void *)CB_ARG_VALUE);
}

static void clean_server_side()
{
	ws_accepter_free(__wsaccepter);
	evhttp_free(__eh);
	ws_connection_free(__ws_conn);
}

static void client_readcb(struct bufevent *bufev, void *arg)
{
	check_arg(arg);

	__command_counter = process_commands(__commands, __command_counter);
}

static void client_writecb(struct bufevent *bufev, void *arg)
{
	check_arg(arg);
}

static void client_errorcb(struct bufevent *bufev, short what, void *arg)
{
	check_arg(arg);

	__what = what;
	__command_counter = process_commands(__commands, __command_counter);
}

static void prepare_client_side()
{
	__client_sock = connect_to_port(HTTP_PORT);

	__client_bufev = bufevent_new(__client_sock, client_readcb, client_writecb, client_errorcb, (void *)CB_ARG_VALUE);
	fail_unless(__client_bufev != NULL, "bufevent_new");
	bufevent_enable(__client_bufev, EV_READ);
}

static void clean_client_side()
{
	bufevent_free(__client_bufev);
	close(__client_sock);
}

static void setup()
{
	event_init();

	__waiting_command_counter = __command_counter = 0;

	prepare_server_side();
	prepare_client_side();
}

static void teardown()
{
	fail_unless(__waiting_command_counter  + 1 == __command_counter, "Not all cbs are executed");

	clean_server_side();
	clean_client_side();
}

static void loop_exit()
{
	event_loopexit(NULL);
}

static void send_handshake_client()
{
	struct evbuffer *buf = evbuffer_new();

	evbuffer_add_printf(buf, "GET %s HTTP/1.1\r\n", WS_URI);
	evbuffer_add_printf(buf, "Upgrade: WebSocket\r\n");
	evbuffer_add_printf(buf, "Connection: Upgrade\r\n");
	evbuffer_add_printf(buf, "Host: %s:%d\r\n", HTTP_HOST, HTTP_PORT);
	evbuffer_add_printf(buf, "Origin: null\r\n");

	evbuffer_add_printf(buf, "Sec-WebSocket-Key1: 18x 6]8vM;54 *(5:  {   U1]8  z [  8\r\n");
	evbuffer_add_printf(buf, "Sec-WebSocket-Key2: 1_ tx7X d  <  nw  334J702) 7]o}` 0\r\n");

	evbuffer_add_printf(buf, "\r\n");

	evbuffer_add(buf, "Tm[K T2u", 8);

	bufevent_write_buffer(__client_bufev, buf);

	evbuffer_free(buf);
}

static void send_message(struct bufevent *bufev, u_char *mess)
{
	struct evbuffer *buf = evbuffer_new();

	evbuffer_add(buf, "\x00", 1);

	evbuffer_add(buf, mess, strlen((char *)mess));

	evbuffer_add(buf, "\xff", 1);

	bufevent_write_buffer(bufev, buf);

	evbuffer_free(buf);
}

static void send_message_client()
{
	send_message(__client_bufev, MESSAGE);
}

static void send_message_server()
{
	ws_connection_send_message(__ws_conn, MESSAGE);
}

static void test_message_client()
{
	struct evbuffer *buf = bufevent_get_input(__client_bufev);

	fail_unless(strncmp(buf->buffer, "\x00", 1) == 0, "error of start ofmessage");
	evbuffer_drain(buf, 1);
	fail_unless(strncmp(buf->buffer, MESSAGE, strlen(MESSAGE)) == 0, "error of body of message");
	evbuffer_drain(buf, strlen(MESSAGE));
	fail_unless(strncmp(buf->buffer, "\xff", 1) == 0, "error of end of message");
	evbuffer_drain(buf, 1);
}

static void test_message_server()
{
	fail_unless(strncmp(__message, MESSAGE, strlen(MESSAGE)) == 0, "error of body of message");
}

static void test_closing_frame(struct bufevent *bufev)
{
	struct evbuffer *buf = bufevent_get_input(bufev);

	fail_unless(strncmp(buf->buffer, "\xff", 1) == 0, "error of closing frame");
	evbuffer_drain(buf, 1);
	fail_unless(strncmp(buf->buffer, "\x00", 1) == 0, "error of closing frame");
	evbuffer_drain(buf, 1);
}

static void test_closing_frame_client()
{
	test_closing_frame(__client_bufev);
}

static void test_closing_frame_server()
{
	fail_unless(__what == WS_CLOSING_FRAME, "error of closing frame");
}

static void send_closing_frame(struct bufevent *bufev)
{
	struct evbuffer *buf = evbuffer_new();

	evbuffer_add(buf, "\xff", 1);

	evbuffer_add(buf, "\x00", 1);

	bufevent_write_buffer(bufev, buf);

	evbuffer_free(buf);
}

static void send_closing_frame_server()
{
	ws_connection_send_close(__ws_conn);
}

static void send_closing_frame_client()
{
	send_closing_frame(__client_bufev);
}

static void test_handshake_client()
{
	struct evbuffer *buf = bufevent_get_input(__client_bufev);
	char *challenge = strstr(buf->buffer, "\r\n\r\n");
	fail_unless(challenge != NULL, "challenge missed");

	fail_unless(strncmp(challenge + 4, "fQJ,fN/4F4!~K~MH", 16) == 0, "error of challenge");

	evbuffer_drain(buf, buf->off);
}

static void test_init_server()
{
	fail_unless(__ws_conn != NULL, "ws_conn NULL");
	fail_unless(ws_get_bufevent(__ws_conn) != NULL, "bufev doesn`t exist yet");
}

START_TEST(test_success_0)
{
	add_command(send_handshake_client);
	add_command(NULL);
	add_command(test_init_server);
	add_command(NULL);
	add_command(test_handshake_client);
	add_command(send_message_client);
	add_command(NULL);
	add_command(test_message_server);
	add_command(send_closing_frame_server);
	add_command(NULL);
	add_command(test_closing_frame_client);
	add_command(loop_exit);

	start_process_commands();
}
END_TEST

START_TEST(test_success_1)
{
	add_command(send_handshake_client);
	add_command(NULL);
	add_command(test_init_server);
	add_command(NULL);
	add_command(test_handshake_client);
	add_command(send_message_client);
	add_command(NULL);
	add_command(test_message_server);
	add_command(send_message_server);
	add_command(NULL);
	add_command(test_message_client);
	add_command(send_closing_frame_client);
	add_command(NULL);
	add_command(test_closing_frame_server);
	add_command(loop_exit);

	start_process_commands();
}
END_TEST

START_TEST(test_error_0)
{
	struct ws_connection *ws = ws_new(NULL, NULL, NULL, NULL);
	ws_send_message(ws, "cdcd");
	ws_send_close(ws);
	ws_free(ws);

	__command_counter = 1;
}
END_TEST

static TCase *web_sockets_tcase()
{
	TCase *tc_web_sockets = tcase_create ("web_sockets");
	tcase_add_checked_fixture(tc_web_sockets, setup, teardown);
	tcase_add_test (tc_web_sockets, test_success_0);
	tcase_add_test (tc_web_sockets, test_success_1);

	tcase_add_test (tc_web_sockets, test_error_0);

	return tc_web_sockets;
}

Suite *make_web_sockets_suite (void)
{
	Suite *s = suite_create ("web_sockets");

	suite_add_tcase (s, web_sockets_tcase());
	
	return s;
}
