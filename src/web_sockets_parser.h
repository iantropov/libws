/*
 * web_sockets_parse.h
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#ifndef WEB_SOCKETS_PARSE_H_
#define WEB_SOCKETS_PARSE_H_

#include "list.h"

struct ws_parser_info;

typedef unsigned char u_char;
typedef void (*handshake_cb)(struct list_head *h, u_char *challenge, void *arg);
typedef void (*message_cb)(u_char *message, void *arg);
typedef void (*error_cb)(short what, void *arg);

struct ws_parser_info *ws_parser_init(handshake_cb *hcb, message_cb *mcb, error_cb *ecb);

void ws_parser_parse(struct ws_parser_info *info, u_char *str);

void ws_parser_destroy(struct ws_parser_info *info);

#endif /* WEB_SOCKETS_PARSE_H_ */
