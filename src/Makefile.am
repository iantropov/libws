## Process this file with automake to produce Makefile.in

bin_PROGRAMS = ws_demo

ws_demo_SOURCES = main.c

lib_LTLIBRARIES = libws.la

libws_la_SOURCES = web_sockets.c web_sockets.h web_sockets_parse.h web_sockets_parse.c

ws_demo_LDADD = libws.la