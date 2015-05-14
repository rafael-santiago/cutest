/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "../cute.h"

//  INFO(Santiago): nothing to do here but it breaks the compilation
//                  if there is something wrong with the macro definition.
CUTE_TEST_CASE(test_decl)
CUTE_TEST_CASE_END

char *get_test_decl_return() {
    CUTE_RUN_TEST(test_decl);
    return NULL;
}

CUTE_TEST_CASE(assertion_tests)
    unsigned int foo = 0xdeadbeef;
    CUTE_CHECK("foo != 0xdeadbeef", foo == 0xdeadbeef);
    CUTE_CHECK_EQ("foo != 0xdeadbeef", foo, 0xdeadbeef);
    CUTE_CHECK_NEQ("foo == 0xf00b45", foo, 0xf00b45);
    foo = 10;
    CUTE_CHECK_LE("foo > 11", foo, 11);
    CUTE_CHECK_GR("foo > 9", foo, 9);
    CUTE_CHECK_LEQ("foo > 10", foo, 10);
    CUTE_CHECK_GEQ("foo < 10", foo, 10);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(get_option_tests)
    char *argv[] = {
        "--cute-log-path=/usr/boo",
        "--passed"
    };
    CUTE_CHECK_EQ("cute_get_option() != NULL", NULL, cute_get_option("none", 2, argv, NULL));
    CUTE_CHECK_EQ("cute_get_option() != NULL", NULL, cute_get_option(NULL, 2, argv, NULL));
    CUTE_CHECK_EQ("cute_get_option() != \"/usr/boo\"", 0, strcmp(cute_get_option("--cute-log-path", 2, argv, "(null)"), "/usr/boo"));
    CUTE_CHECK_EQ("cute_get_option() != \"1\"", 0, strcmp(cute_get_option("--passed", 2, argv, "0"), "1"));
CUTE_TEST_CASE_END

CUTE_TEST_CASE(entry)
    char *retval = get_test_decl_return();
    CUTE_CHECK_EQ("retval != NULL", retval, NULL);
    CUTE_RUN_TEST(assertion_tests);
    CUTE_RUN_TEST(get_option_tests);
CUTE_TEST_CASE_END

CUTE_MAIN(entry)
