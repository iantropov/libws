
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
	/*int len = (int)(cur - start - off) + 1;
	u_char *str = (u_char *)calloc(len, sizeof(u_char));
	if (str == NULL)
		return NULL;
		*/
	*cur = '\0';
	
	return start + off;//memcpy(str, start + off, len);
} 


#line 137 "web_sockets_parser_parse.rl"



#line 60 "web_sockets_parser_parse.c"
static const char _web_sockets_parser_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 1, 
	3, 1, 4, 1, 5, 1, 6, 1, 
	7, 2, 4, 0
};

static const unsigned char _web_sockets_parser_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 17, 29, 
	31, 32, 33, 34, 35, 36, 37, 38, 
	39, 40, 51, 52, 52, 52, 52, 52, 
	52, 52, 52, 52, 54, 63, 65, 67, 
	69, 71, 72, 81, 90, 99, 101, 103, 
	105, 107, 109, 111, 117, 123, 137
};

static const unsigned char _web_sockets_parser_trans_keys[] = {
	71u, 69u, 84u, 32u, 32u, 33u, 37u, 47u, 
	61u, 95u, 126u, 36u, 59u, 64u, 90u, 97u, 
	122u, 32u, 33u, 37u, 61u, 95u, 126u, 36u, 
	59u, 63u, 90u, 97u, 122u, 32u, 72u, 84u, 
	84u, 80u, 47u, 49u, 46u, 49u, 13u, 10u, 
	13u, 32u, 57u, 59u, 126u, 192u, 223u, 224u, 
	239u, 240u, 247u, 10u, 0u, 255u, 255u, 32u, 
	126u, 192u, 223u, 224u, 239u, 240u, 247u, 128u, 
	191u, 128u, 191u, 128u, 191u, 0u, 255u, 0u, 
	58u, 32u, 126u, 192u, 223u, 224u, 239u, 240u, 
	247u, 32u, 33u, 126u, 192u, 223u, 224u, 239u, 
	240u, 247u, 13u, 32u, 126u, 192u, 223u, 224u, 
	239u, 240u, 247u, 128u, 191u, 128u, 191u, 128u, 
	191u, 128u, 191u, 128u, 191u, 128u, 191u, 48u, 
	57u, 65u, 70u, 97u, 102u, 48u, 57u, 65u, 
	70u, 97u, 102u, 32u, 33u, 37u, 61u, 95u, 
	126u, 36u, 46u, 48u, 59u, 63u, 90u, 97u, 
	122u, 0
};

static const char _web_sockets_parser_single_lengths[] = {
	0, 1, 1, 1, 1, 7, 6, 2, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 0, 0, 0, 0, 0, 
	0, 0, 0, 2, 1, 0, 0, 0, 
	2, 1, 1, 1, 1, 0, 0, 0, 
	0, 0, 0, 0, 0, 6, 0
};

static const char _web_sockets_parser_range_lengths[] = {
	0, 0, 0, 0, 0, 3, 3, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 5, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 4, 1, 1, 1, 
	0, 0, 4, 4, 4, 1, 1, 1, 
	1, 1, 1, 3, 3, 4, 0
};

static const unsigned char _web_sockets_parser_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 19, 29, 
	32, 34, 36, 38, 40, 42, 44, 46, 
	48, 50, 57, 59, 60, 61, 62, 63, 
	64, 65, 66, 67, 70, 76, 78, 80, 
	82, 85, 87, 93, 99, 105, 107, 109, 
	111, 113, 115, 117, 121, 125, 136
};

static const char _web_sockets_parser_indicies[] = {
	1, 0, 2, 0, 3, 0, 4, 0, 
	4, 5, 6, 7, 5, 5, 5, 5, 
	5, 5, 0, 8, 9, 10, 9, 9, 
	9, 9, 9, 9, 0, 11, 12, 0, 
	13, 0, 14, 0, 15, 0, 16, 0, 
	17, 0, 18, 0, 19, 0, 20, 0, 
	21, 0, 22, 23, 23, 24, 25, 26, 
	0, 27, 0, 28, 29, 30, 31, 32, 
	33, 34, 35, 36, 37, 0, 42, 38, 
	39, 40, 41, 0, 38, 0, 39, 0, 
	40, 0, 43, 44, 0, 45, 0, 47, 
	46, 48, 49, 50, 0, 51, 52, 53, 
	54, 55, 0, 56, 57, 58, 59, 60, 
	0, 57, 0, 58, 0, 59, 0, 46, 
	0, 48, 0, 49, 0, 61, 61, 61, 
	0, 9, 9, 9, 0, 8, 9, 10, 
	9, 9, 9, 9, 9, 9, 9, 0, 
	0, 0
};

static const char _web_sockets_parser_trans_targs[] = {
	0, 2, 3, 4, 5, 6, 43, 45, 
	7, 6, 43, 7, 8, 9, 10, 11, 
	12, 13, 14, 15, 16, 17, 18, 34, 
	40, 41, 42, 19, 20, 21, 22, 23, 
	24, 25, 26, 27, 28, 33, 28, 29, 
	30, 31, 32, 28, 33, 46, 34, 35, 
	40, 41, 42, 35, 36, 37, 38, 39, 
	16, 36, 37, 38, 39, 44
};

static const char _web_sockets_parser_trans_actions[] = {
	15, 0, 0, 0, 0, 1, 1, 1, 
	3, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 1, 1, 0, 1, 0, 0, 0, 
	0, 0, 0, 0, 17, 9, 0, 0, 
	0, 0, 0, 1, 0, 0, 0, 5, 
	0, 0, 0, 0, 1, 1, 1, 1, 
	7, 0, 0, 0, 0, 0
};

static const char _web_sockets_parser_to_state_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	11, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 13
};

static const char _web_sockets_parser_eof_actions[] = {
	0, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 15, 15, 
	15, 15, 15, 15, 15, 15, 0
};

static const int web_sockets_parser_start = 1;
static const int web_sockets_parser_first_final = 46;
static const int web_sockets_parser_error = 0;

static const int web_sockets_parser_en_main = 1;


#line 140 "web_sockets_parser_parse.rl"

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

	
#line 206 "web_sockets_parser_parse.c"
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
#line 56 "web_sockets_parser_parse.rl"
	{
		info->start = (int)(p - start);
	}
	break;
	case 1:
#line 61 "web_sockets_parser_parse.rl"
	{
		info->uri = save_string(start, info->start, p);
		if (info->uri == NULL) {
			handle_error(info, WS_PARSER_INTERNAL_ERROR);
			{p++; goto _out; }
		}
	}
	break;
	case 2:
#line 70 "web_sockets_parser_parse.rl"
	{
		key = save_string(start, info->start, p);
		if (key == NULL) {
			handle_error(info, WS_PARSER_INTERNAL_ERROR);
			{p++; goto _out; }
		}
		
		if (save_header_key(info, key) == -1)
			{p++; goto _out; }
	}
	break;
	case 3:
#line 82 "web_sockets_parser_parse.rl"
	{
		value = save_string(start, info->start, p);
		if (value == NULL) {
			handle_error(info, WS_PARSER_INTERNAL_ERROR);
			{p++; goto _out; }
		}
		
		save_header_value(info, value);
	}
	break;
	case 4:
#line 93 "web_sockets_parser_parse.rl"
	{
		challenge = save_string(start, info->start, p);
		if (challenge == NULL) {
			handle_error(info, WS_PARSER_INTERNAL_ERROR);
			{p++; goto _out; }
		}
		
		info->h_cb(&info->headers, info->uri, challenge, info->cb_arg);
		
		info->ready_for_drain += (size_t)(p - start);
	}
	break;
	case 7:
#line 128 "web_sockets_parser_parse.rl"
	{
		handle_error(info, WS_PARSER_PARSE_ERROR);
		return;
	}
	break;
#line 342 "web_sockets_parser_parse.c"
		}
	}

_again:
	_acts = _web_sockets_parser_actions + _web_sockets_parser_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 5:
#line 106 "web_sockets_parser_parse.rl"
	{
		message = save_string(start, info->start + 1, p);
		if (message == NULL) {
			handle_error(info, WS_PARSER_INTERNAL_ERROR);
			{p++; goto _out; }
		}

		info->m_cb(message, info->cb_arg);
		
		info->ready_for_drain += (size_t)(p - start) - info->start + 1;
	}
	break;
	case 6:
#line 119 "web_sockets_parser_parse.rl"
	{
		handle_error(info, WS_PARSER_CLOSING_FRAME);
		
		info->ready_for_drain += 2;
		
		return;
	}
	break;
#line 375 "web_sockets_parser_parse.c"
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
	case 7:
#line 128 "web_sockets_parser_parse.rl"
	{
		handle_error(info, WS_PARSER_PARSE_ERROR);
		return;
	}
	break;
#line 397 "web_sockets_parser_parse.c"
		}
	}
	}

	_out: {}
	}

#line 153 "web_sockets_parser_parse.rl"

	info->cs = cs;
	info->off = p - start;
}