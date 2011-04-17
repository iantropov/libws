/*
 * web_sockets_parser_parse.c
 *
 *  Created on: Apr 11, 2011
 *      Author: ant
 */
 
#include "web_sockets_parser_internal.h"
 
#include <stdlib.h>
#include <string.h>
 
static void handle_error(struct ws_parser_info *wsp, short what)
{
	wsp->e_cb(wsp, what, wsp->cb_arg);
}

static int save_header_key(struct ws_parser_info *wsp, u_char *key)
{
	struct key_value_entry *entry = (struct key_value_entry *)calloc(1, sizeof(struct key_value_entry));
	if (entry == NULL) {
		handle_error(wsp, WS_PARSER_INTERNAL_ERROR);
		return -1;
	}
	
	entry->key = key;
	
	list_add_tail(&(entry->list), &(wsp->headers));
	
	return 0;
}

static void save_header_value(struct ws_parser_info *wsp, u_char *value)
{
	struct key_value_entry *entry = list_entry(wsp->headers.prev, struct key_value_entry, list);
	
	entry->value = value;
}

static u_char *save_string(u_char *start, int off, u_char *cur)
{
	*cur = '\0';
	return start + off;
} 

%%{
	machine web_sockets_parser;
	
	action A_save_start
	{
		info->start = (int)(p - start);
	}
	
	action A_save_uri
	{
		info->uri = save_string(start, info->start, p);
	}
	
	action A_save_key
	{
		key = save_string(start, info->start, p);
		
		if (save_header_key(info, key) == -1)
			fbreak;
	}
	
	action A_save_value
	{
		value = save_string(start, info->start, p);
		
		save_header_value(info, value);
	}
	
	action A_handle_handshake
	{
		challenge = save_string(start, info->start, p);
		
		info->h_cb(&info->headers, info->uri, challenge, info->cb_arg);
		
		info->ready_for_drain += (size_t)(p - start);
	}
	
	action A_handle_message
	{
		message = save_string(start, info->start + 1, p);

		info->m_cb(message, info->cb_arg);
		
		info->ready_for_drain += (size_t)(p - start) - info->start + 1;
	}
	
	action A_handle_closing_frame
	{
		handle_error(info, WS_PARSER_CLOSING_FRAME);
		
		info->ready_for_drain += 2;
		
		fbreak;
	}
	 
	action A_handle_error
	{
		handle_error(info, WS_PARSER_PARSE_ERROR);
		fbreak;
	}

#--------------------------------------------------------#
	include web_sockets "web_sockets.rl";
	
	main := (handshake (text_frame)* closing_frame) $err(A_handle_error);
}%%

%% write data;

void ws_parser_parse(struct ws_parser_info *info, u_char *start, u_char *end)
{
	int		cs = info->cs;
	u_char	*p = info->off == 0 ? start : start + info->off, 
			*pe = end,
			*eof = NULL,
			*message,
			*value,
			*key,
			*challenge;

	%% write exec;

	info->cs = cs;
	info->off = p - start;
}