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
    CUTE_CHECK_EQUAL("foo != 0xdeadbeef", foo, 0xdeadbeef);
    CUTE_CHECK_NEQUAL("foo == 0xf00b45", foo, 0xf00b45);
    foo = 10;
    CUTE_CHECK_LESS("foo > 11", foo, 11);
    CUTE_CHECK_GREATER("foo > 9", foo, 9);
    CUTE_CHECK_LEQUALS("foo > 10", foo, 10);
    CUTE_CHECK_GEQUALS("foo < 10", foo, 10);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(cute_tests)
    char *retval = get_test_decl_return();
    CUTE_CHECK_EQUAL("retval != NULL", retval, NULL);
    CUTE_RUN_TEST(assertion_tests);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(entry)
    CUTE_RUN_TEST(cute_tests);
CUTE_TEST_CASE_END

int main(int argc, char **argv) {
    CUTE_RUN(entry);
    return 0;
}
