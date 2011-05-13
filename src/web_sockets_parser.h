/*
 * web_sockets_parse.h
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#ifndef WEB_SOCKETS_PARSE_H_
#define WEB_SOCKETS_PARSE_H_

#include <sys/types.h>
#include <event.h>

struct ws_parser_info;

typedef void (*message_cb)(u_char *message, void *arg);
typedef void (*error_cb)(struct ws_parser_info *info, short what, void *arg);

struct ws_parser_info *ws_parser_init(message_cb mcb, error_cb ecb, void *arg);

void ws_parser_parse(struct ws_parser_info *info, u_char *start, u_char *end);

size_t ws_parser_drain(struct ws_parser_info *info);

void ws_parser_destroy(struct ws_parser_info *info);

#endif /* WEB_SOCKETS_PARSE_H_ */
