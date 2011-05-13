/*
 * web_sockets.h
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#ifndef WEB_SOCKETS_H_
#define WEB_SOCKETS_H_

#include <sys/types.h>
#include <event0/evhttp.h>
#include <event0/bufevent.h>

#define WS_PARSE_ERROR 0x1
#define WS_INTERNAL_ERROR 0x2
#define WS_CLOSING_FRAME 0x4

struct ws_connection;

typedef void (*ws_message_cb)(struct ws_connection *conn, u_char *message, void *arg);
typedef void (*ws_error_cb)(struct ws_connection *conn, short what, void *arg);
typedef void (*ws_init_cb)(struct ws_connection *conn, void *arg);

struct ws_connection *ws_new(ws_init_cb init_cb, ws_message_cb mes_cb,
		ws_error_cb err_cb, void *arg);
int ws_send_message(struct ws_connection *conn, u_char *message);
int ws_send_close(struct ws_connection *conn);
void ws_free(struct ws_connection *conn);

void ws_set_cbs(struct ws_connection *conn, ws_init_cb init_cb,
		ws_message_cb mes_cb, ws_error_cb err_cb, void *arg);

struct bufevent *ws_get_bufevent(struct ws_connection *);

void evhttp_set_ws(struct evhttp *http, char *uri, struct ws_connection *conn);

#endif /* WEB_SOCKETS_H_ */
