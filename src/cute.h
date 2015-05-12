/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _CUTE_CUTE_H
#define _CUTE_CUTE_H 1

#include <stdio.h>

#define CUTE_CHECK(msg, chk) do { if ((chk) == 0) { printf("hmm bad, bad bug in %s at line %d: ", __FILE__, __LINE__); return msg; } } while (0)

#define CUTE_CHECK_EQ(msg, a, b) CUTE_CHECK(msg, (a) == (b))

#define CUTE_CHECK_NEQ(msg, a, b) CUTE_CHECK(msg, (a) != (b))

#define CUTE_CHECK_LE(msg, a, b) CUTE_CHECK(msg, (a) < (b))

#define CUTE_CHECK_GR(msg, a, b) CUTE_CHECK(msg, (a) > (b))

#define CUTE_CHECK_LEQ(msg, a, b) CUTE_CHECK(msg, (a) <= (b))

#define CUTE_CHECK_GEQ(msg, a, b) CUTE_CHECK(msg, (a) >= (b))

#define CUTE_RUN_TEST(test) do {\
                            printf("-- running %s...\n", #test);\
                            char *msg = test();\
                            g_cute_ran_tests++;\
                            if (msg != NULL) return msg;\
                            printf("-- passed.\n");\
                         } while (0)

#define CUTE_RUN_TEST_NTIMES(test, times) do { for (g_cute_general_counter = 0; g_cute_general_counter < times; g_cute_general_counter++) {\
                                        CUTE_RUN_TEST(test);\
                                     } } while (0)

#define CUTE_RUN(entry) do { char *entry_return = entry(); if (entry_return == NULL) printf("*** all tests passed. [%d test(s) ran]\n", g_cute_ran_tests); else { printf("*** fail: %s [%d test(s) ran]\n", entry_return, g_cute_ran_tests); return 1; } } while(0);

#define CUTE_TEST_CASE(test) char *test() {

#define CUTE_TEST_CASE_END  return NULL;\
                           }
extern int g_cute_general_counter;

extern int g_cute_ran_tests;

#endif
