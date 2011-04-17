/*
 * web_sockets_parse.c
 *
 *  Created on: Apr 10, 2011
 *      Author: ant
 */

#include "web_sockets_parser_internal.h"

#include <stdlib.h>

struct ws_parser_info *ws_parser_init(handshake_cb hcb, message_cb mcb, error_cb ecb, void *arg)
{
	struct ws_parser_info *info = (struct ws_parser_info *)calloc(1, sizeof(struct ws_parser_info));
	if (info == NULL)
		return NULL;

	INIT_LIST_HEAD(&info->headers);

	info->cs = 1;

	info->h_cb = hcb;
	info->m_cb = mcb;
	info->e_cb = ecb;

	info->cb_arg = arg;

	return info;
}

void ws_parser_destroy(struct ws_parser_info *info)
{
	clean_parser_info(info);

	free(info);
}

size_t ws_parser_drain(struct ws_parser_info *wsp)
{
	int ready_for_drain = wsp->ready_for_drain;
	wsp->off -= ready_for_drain;
	wsp->ready_for_drain = 0;

	return ready_for_drain;
}

void clean_parser_info(struct ws_parser_info *wsp)
{
	wsp->uri = NULL;
	wsp->cs = 1;
	wsp->off = 0;

	struct list_head *pos, *n;
	struct key_value_entry *entry;
	list_for_each_safe(pos, n, &wsp->headers) {
		entry = list_entry(pos, struct key_value_entry, list);

		list_del(&entry->list);

		free(entry);
	}
}
