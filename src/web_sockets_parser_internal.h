/*
 * web_sockets_parser_internal.h
 *
 *  Created on: Apr 11, 2011
 *      Author: ant
 */

#ifndef WEB_SOCKETS_PARSER_INTERNAL_H_
#define WEB_SOCKETS_PARSER_INTERNAL_H_

#include "web_sockets_parser.h"
#include "web_sockets.h"
#include "event.h"

struct ws_parser_info {
	int cs;
	int start;
	int off;

	size_t ready_for_drain;

	message_cb m_cb;
	error_cb e_cb;

	void *cb_arg;

	short error;
};

void clean_parser_info(struct ws_parser_info *wsp);


#endif /* WEB_SOCKETS_PARSER_INTERNAL_H_ */
