/*
 * web_sockets_parser_parse.c
 *
 *  Created on: Apr 11, 2011
 *      Author: ant
 */
 
#include "web_sockets_parser_internal.h"
 
#include <stdlib.h>
#include <string.h>

static void handle_errorcb(int fd, short what, void *arg)
{
	struct ws_parser_info *wsp = arg;
	wsp->e_cb(wsp, wsp->error, wsp->cb_arg);
}

static void handle_error(struct ws_parser_info *wsp, short what)
{
	wsp->error |= what;
	
	struct timeval tv = {0, 1};
	event_once(-1 - what, EV_TIMEOUT, handle_errorcb, wsp, &tv);
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
	
	action A_handle_message
	{
		message = save_string(start, info->start + 1, p);

		info->m_cb(message, info->cb_arg);

		info->ready_for_drain += (size_t)(p - start) - info->start + 1;
	}
	
	action A_handle_closing_frame
	{
		handle_error(info, WS_CLOSING_FRAME);
		info->ready_for_drain += 2;
	}
	 
	action A_handle_error
	{
		handle_error(info, WS_PARSE_ERROR);
		
		fbreak;
	}

#--------------------------------------------------------#
	include web_sockets "web_sockets.rl";
	
	main := (text_frame* closing_frame) $err(A_handle_error);
}%%

%% write data;

void ws_parser_parse(struct ws_parser_info *info, u_char *start, u_char *end)
{
	int		cs = info->cs, 
			ret;
	u_char	*p = info->off == 0 ? start : start + info->off, 
			*pe = end,
			*eof = NULL,
			*message;

	%% write exec;

	info->cs = cs;
	info->off = p - start;
}