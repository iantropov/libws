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


struct ws_accepter;

typedef void (*ws_accept_cb)(struct ws_accepter *wa, struct bufevent *bufev, void *arg);

struct ws_accepter *ws_accepter_new(struct evhttp *eh, char *uri, ws_accept_cb ac_cb, void *arg);
void ws_accepter_free(struct ws_accepter *wa);

struct ws_connection;

typedef void (*ws_message_cb)(struct ws_connection *wc, u_char *mes, void *arg);
typedef void (*ws_error_cb)(struct ws_connection *wc, short error, void *arg);

struct ws_connection *ws_connection_new(struct bufevent *bufev, ws_message_cb m_cb, ws_error_cb e_cb, void *arg);
void ws_connection_set_cbs(struct ws_connection *conn, ws_message_cb m_cb, ws_error_cb e_cb, void *arg);
int ws_connection_send_message(struct ws_connection *wc, u_char *mes);
int ws_connection_send_close(struct ws_connection *wc);
void ws_connection_free(struct ws_connection *wc);


#endif /* WEB_SOCKETS_H_ */
