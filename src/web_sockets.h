/*
 * web_sockets.h
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#ifndef WEB_SOCKETS_H_
#define WEB_SOCKETS_H_

struct ws_server;
struct ws_connection;

typedef unsigned char u_char;
typedef void (*ws_cb)(struct ws_connection, u_char *message);

struct ws_server *ws_init(int port);
int ws_set_callback(struct ws_server *serv, char *uri, ws_cb *cb);
void ws_remove_callback(struct ws_server *serv, char *uri);
void ws_destroy(struct ws_server);
int ws_send(struct ws_connection conn, u_char *message);

#endif /* WEB_SOCKETS_H_ */
