## Process this file with automake to produce Makefile.in

SUBDIRS= ws_parser .
TESTS = check_ws
check_PROGRAMS = check_ws
check_ws_SOURCES = 	check_ws.c
check_ws_CFLAGS = @CHECK_CFLAGS@
check_ws_LDADD = 	$(top_builddir)/src/libws.la  \
			$(top_builddir)/tests/ws_parser/libcheckwsparser.la \
			 @CHECK_LIBS@