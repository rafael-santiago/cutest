/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "../cute.h"

static int g_counter = 0;

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
    CUTE_CHECK_EQ("cute_get_option() != \"/usr/boo\"", 0, strcmp(cute_get_option("cute-log-path", 2, argv, "(null)"), "/usr/boo"));
    CUTE_CHECK_EQ("cute_get_option() != \"1\"", 0, strcmp(cute_get_option("passed", 2, argv, "0"), "1"));
CUTE_TEST_CASE_END

CUTE_FIXTURE_SETUP(fixture_test)
    g_counter = 1;
CUTE_FIXTURE_END

CUTE_FIXTURE_TEARDOWN(fixture_test)
    g_counter++;
CUTE_FIXTURE_END

CUTE_TEST_CASE(fixture_test)
    CUTE_CHECK_EQ("g_counter != 1", g_counter, 1);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(CUTE_GET_OPTION_MACRO_test)
    char *value = NULL;
    CUTE_CHECK_EQ("CUTE_GET_OPTION(\"superunknown\") != NULL", CUTE_GET_OPTION("superunknown"), NULL);
    value = CUTE_GET_OPTION("foo");
    CUTE_CHECK_NEQ("CUTE_GET_OPTION(\"foo\") == NULL", value, NULL);
    CUTE_CHECK("CUTE_GET_OPTION(\"foo\") != \"bar\"", strcmp(value, "bar") == 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(entry)
    char *retval = get_test_decl_return();
    CUTE_CHECK_EQ("retval != NULL", retval, NULL);
    CUTE_RUN_TEST(assertion_tests);
    CUTE_RUN_TEST(get_option_tests);
    CUTE_RUN_TEST_WITH_FIXTURE(fixture_test);
    CUTE_CHECK_EQ("g_counter != 2", g_counter, 2);
    CUTE_RUN_TEST(CUTE_GET_OPTION_MACRO_test);
CUTE_TEST_CASE_END

CUTE_MAIN(entry)
