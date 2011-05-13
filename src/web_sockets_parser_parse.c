
#line 1 "web_sockets_parser_parse.rl"
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


#line 67 "web_sockets_parser_parse.rl"



#line 41 "web_sockets_parser_parse.c"
static const char _web_sockets_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3
};

static const char _web_sockets_parser_key_offsets[] = {
	0, 0, 2, 11, 13, 15, 17, 19, 
	20
};

static const unsigned char _web_sockets_parser_trans_keys[] = {
	0u, 255u, 255u, 32u, 126u, 192u, 223u, 224u, 
	239u, 240u, 247u, 128u, 191u, 128u, 191u, 128u, 
	191u, 0u, 255u, 0u, 0
};

static const char _web_sockets_parser_single_lengths[] = {
	0, 2, 1, 0, 0, 0, 2, 1, 
	0
};

static const char _web_sockets_parser_range_lengths[] = {
	0, 0, 4, 1, 1, 1, 0, 0, 
	0
};

static const char _web_sockets_parser_index_offsets[] = {
	0, 0, 3, 9, 11, 13, 15, 18, 
	20
};

static const char _web_sockets_parser_indicies[] = {
	0, 2, 1, 7, 3, 4, 5, 6, 
	1, 3, 1, 4, 1, 5, 1, 0, 
	2, 1, 8, 1, 1, 0
};

static const char _web_sockets_parser_trans_targs[] = {
	2, 0, 7, 2, 3, 4, 5, 6, 
	8
};

static const char _web_sockets_parser_trans_actions[] = {
	1, 7, 0, 0, 0, 0, 0, 0, 
	0
};

static const char _web_sockets_parser_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 3, 0, 
	5
};

static const char _web_sockets_parser_eof_actions[] = {
	0, 7, 7, 7, 7, 7, 7, 7, 
	0
};

static const int web_sockets_parser_start = 1;
static const int web_sockets_parser_first_final = 8;
static const int web_sockets_parser_error = 0;

static const int web_sockets_parser_en_main = 1;


#line 70 "web_sockets_parser_parse.rl"

void ws_parser_parse(struct ws_parser_info *info, u_char *start, u_char *end)
{
	int		cs = info->cs, 
			ret;
	u_char	*p = info->off == 0 ? start : start + info->off, 
			*pe = end,
			*eof = NULL,
			*message;

	
#line 118 "web_sockets_parser_parse.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const unsigned char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _web_sockets_parser_trans_keys + _web_sockets_parser_key_offsets[cs];
	_trans = _web_sockets_parser_index_offsets[cs];

	_klen = _web_sockets_parser_single_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _web_sockets_parser_range_lengths[cs];
	if ( _klen > 0 ) {
		const unsigned char *_lower = _keys;
		const unsigned char *_mid;
		const unsigned char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _web_sockets_parser_indicies[_trans];
	cs = _web_sockets_parser_trans_targs[_trans];

	if ( _web_sockets_parser_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _web_sockets_parser_actions + _web_sockets_parser_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 37 "web_sockets_parser_parse.rl"
	{
		info->start = (int)(p - start);
	}
	break;
	case 3:
#line 57 "web_sockets_parser_parse.rl"
	{
		handle_error(info, WS_PARSE_ERROR);
		
		{p++; goto _out; }
	}
	break;
#line 206 "web_sockets_parser_parse.c"
		}
	}

_again:
	_acts = _web_sockets_parser_actions + _web_sockets_parser_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 42 "web_sockets_parser_parse.rl"
	{
		message = save_string(start, info->start + 1, p);

		info->m_cb(message, info->cb_arg);

		info->ready_for_drain += (size_t)(p - start) - info->start + 1;
	}
	break;
	case 2:
#line 51 "web_sockets_parser_parse.rl"
	{
		handle_error(info, WS_CLOSING_FRAME);
		info->ready_for_drain += 2;
	}
	break;
#line 232 "web_sockets_parser_parse.c"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	const char *__acts = _web_sockets_parser_actions + _web_sockets_parser_eof_actions[cs];
	unsigned int __nacts = (unsigned int) *__acts++;
	while ( __nacts-- > 0 ) {
		switch ( *__acts++ ) {
	case 3:
#line 57 "web_sockets_parser_parse.rl"
	{
		handle_error(info, WS_PARSE_ERROR);
		
		{p++; goto _out; }
	}
	break;
#line 255 "web_sockets_parser_parse.c"
		}
	}
	}

	_out: {}
	}

#line 81 "web_sockets_parser_parse.rl"

	info->cs = cs;
	info->off = p - start;
}