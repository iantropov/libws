#include "check.h"
#include <stdlib.h>

#include "../../src/web_sockets.h"

START_TEST(test_hello)
{
    fail_unless(1 == 1);
}
END_TEST

TCase *ws_parser_tcase()
{
	TCase *tc_ws_parser = tcase_create ("web_sockets_parser");
	tcase_add_test (tc_ws_parser, test_hello);

	return tc_ws_parser;
}

Suite *make_ws_parser_suite (void)
{
	Suite *s = suite_create ("web_sockets_parser");

	suite_add_tcase (s, ws_parser_tcase());
	
	return s;
}
