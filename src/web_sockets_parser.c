/*
 * web_sockets_parse.c
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#include "web_sockets_parser.h"

#include <stdlib.h>

struct ws_parser_info {
	int top;

	handshake_cb *h_cb;
	message_cb *m_cb;
	error_cb *e_cb;
};

//void (*handshake_cb)(struct list_head *h, u_char *challenge, void *arg);
//void (*message_cb)(u_char *message, void *arg);
//void (*handshake_cb)(short what, void *arg);

struct ws_parser_info *ws_parser_init(handshake_cb *hcb, message_cb *mcb, error_cb *ecb)
{
	struct ws_parser_info *info = (struct ws_parser_info *)calloc(1, sizeof(struct ws_parser_info));
	if (info == NULL)
		return NULL;

	info->h_cb = hcb;
	info->m_cb = mcb;
	info->e_cb = ecb;

	return info;
}

void ws_parser_parse(struct ws_parser_info *info, u_char *str);

void ws_parser_destroy(struct ws_parser_info *info)
{
	free(info);
}
