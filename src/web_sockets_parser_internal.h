/*
 * web_sockets_parser_internal.h
 *
 *  Created on: Apr 11, 2011
 *      Author: ant
 */

#ifndef WEB_SOCKETS_PARSER_INTERNAL_H_
#define WEB_SOCKETS_PARSER_INTERNAL_H_

#include "web_sockets_parser.h"

struct ws_parser_info {
	int cs;
	u_char *p;
	int start;
	int off;

	size_t ready_for_drain;

	u_char *uri;
	struct list_head headers;

	handshake_cb h_cb;
	message_cb m_cb;
	error_cb e_cb;

	void *cb_arg;
};

void clean_parser_info(struct ws_parser_info *wsp);


#endif /* WEB_SOCKETS_PARSER_INTERNAL_H_ */
