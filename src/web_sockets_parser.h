/*
 * web_sockets_parse.h
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#ifndef WEB_SOCKETS_PARSE_H_
#define WEB_SOCKETS_PARSE_H_

#include "list.h"
#include </usr/include/sys/types.h>
#include <event.h>

struct ws_parser_info;

struct key_value_entry {
	struct list_head list;

	u_char *key;
	u_char *value;
};

#define WS_PARSER_PARSE_ERROR 0x1
#define WS_PARSER_INTERNAL_ERROR 0x2
#define WS_PARSER_CLOSING_FRAME 0x4

typedef void (*handshake_cb)(struct list_head *h, u_char *uri, u_char *challenge, void *arg);
typedef void (*message_cb)(u_char *message, void *arg);
typedef void (*error_cb)(struct ws_parser_info *info, short what, void *arg);

struct ws_parser_info *ws_parser_init(handshake_cb hcb, message_cb mcb, error_cb ecb, void *arg);

void ws_parser_parse(struct ws_parser_info *info, u_char *start, u_char *end);

size_t ws_parser_drain(struct ws_parser_info *info);

void ws_parser_destroy(struct ws_parser_info *info);

#endif /* WEB_SOCKETS_PARSE_H_ */
