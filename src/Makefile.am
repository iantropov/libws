## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libws.la

web_sockets_parser_parse.c : web_sockets_parser_parse.rl
				ragel -o $@ $<

libws_la_SOURCES = 	web_sockets.c web_sockets.h\
			web_sockets_parser.h web_sockets_parser.c\
			web_sockets_parser_parse.c\
			web_sockets_parser_internal.h

library_includedir = $(includedir)/ws
library_include_HEADERS = web_sockets.h