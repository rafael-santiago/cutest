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
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <stdlib.h>

#include "cute_memory.h"
#include "cute_mmap.h"

#define CUTE_CHECK(msg, chk) do { g_cute_last_exec_line = __LINE__; g_cute_last_ref_file = __FILE__; if ((chk) == 0) { cute_log("hmm bad, bad bug in %s at line %d: ", __FILE__, __LINE__); cute_close_log_fd(); return msg; } } while (0)

#define CUTE_CHECK_EQ(msg, a, b) CUTE_CHECK(msg, (a) == (b))

#define CUTE_CHECK_NEQ(msg, a, b) CUTE_CHECK(msg, (a) != (b))

#define CUTE_CHECK_LE(msg, a, b) CUTE_CHECK(msg, (a) < (b))

#define CUTE_CHECK_GR(msg, a, b) CUTE_CHECK(msg, (a) > (b))

#define CUTE_CHECK_LEQ(msg, a, b) CUTE_CHECK(msg, (a) <= (b))

#define CUTE_CHECK_GEQ(msg, a, b) CUTE_CHECK(msg, (a) >= (b))

#define CUTE_RUN_TEST(test) do {\
                            cute_log("-- running %s...\n", #test);\
                            g_cute_last_exec_line = __LINE__; g_cute_last_ref_file = __FILE__;\
                            char *msg = test();\
                            g_cute_ran_tests++;\
                            if (msg != NULL) return msg;\
                            cute_log("-- passed.\n");\
                         } while (0)

#define CUTE_RUN_TEST_WITH_FIXTURE(test) do {\
                            g_cute_last_exec_line = __LINE__; g_cute_last_ref_file = __FILE__;\
                            g_cute_fixture_setup = test ## _setup;\
                            g_cute_fixture_teardown = test ## _teardown;\
                            test ## _setup();\
                            g_cute_fixture_setup = NULL;\
                            cute_log("-- running %s...\n", #test);\
                            char *msg = test();\
                            g_cute_ran_tests++;\
                            test ## _teardown();\
                            g_cute_fixture_teardown = NULL;\
                            if (msg != NULL) return msg;\
                            cute_log("-- passed.\n");\
                         } while (0)


#define CUTE_RUN_TEST_NTIMES(test, times) do { g_cute_last_exec_line = __LINE__; g_cute_last_ref_file = __FILE__; for (g_cute_general_counter = 0; g_cute_general_counter < times; g_cute_general_counter++) {\
                                        CUTE_RUN_TEST(test);\
                                     } } while (0)

#define CUTE_RUN(entry) do { char *entry_return = entry(); if (entry_return == NULL) cute_log("*** all tests passed. [%d test(s) ran]\n", g_cute_ran_tests); else { cute_log("*** fail: %s [%d test(s) ran]\n", entry_return, g_cute_ran_tests); cute_close_log_fd(); return 1; } } while(0);

#define CUTE_TEST_CASE(test) char *test() {

#define CUTE_TEST_CASE_END  return NULL;\
                           }

#define CUTE_FIXTURE_SETUP(test) void test ## _setup() {

#define CUTE_FIXTURE_END  }

#define CUTE_FIXTURE_TEARDOWN(test) void test ## _teardown() {

#define CUTE_MAIN(entry) void sigint_watchdog(int signum) {\
                          printf("-- CUTE INT TRAP --\n");\
                          if (g_cute_fixture_teardown != NULL) {\
                           g_cute_fixture_teardown();\
                           g_cute_fixture_teardown = NULL;\
                          }\
                         }\
                         void sigsegv_watchdog(int signum) {\
                          size_t size;\
                          void *array[50];\
                          printf("-- CUTE PANIC TRAP --\n");\
                          if (g_cute_last_exec_line > -1) {\
                           printf("\n< The last successfully executed line was the line #%d from file \"%s\" >", g_cute_last_exec_line, g_cute_last_ref_file);\
                          }\
                          printf("\n\n");\
                          size = backtrace(array, 50);\
                          backtrace_symbols_fd(array, size, STDERR_FILENO);\
                          printf("\n\n");\
                          exit(1);\
                         }\
                         int main(int argc, char **argv) {\
                          char *logpath = NULL;\
                          int exit_code = 0;\
                          signal(SIGSEGV, sigsegv_watchdog);\
                          signal(SIGBUS, sigsegv_watchdog);\
                          signal(SIGABRT, sigsegv_watchdog);\
                          signal(SIGINT, sigint_watchdog);\
                          signal(SIGTERM, sigint_watchdog);\
                          init_memory_func_ptr();\
                          logpath = cute_get_option("cute-log-path", argc, argv, NULL);\
                          if (cute_get_option("cute-leak-check", argc, argv, NULL) != NULL) {\
                           g_cute_leak_check = 1;\
                          }\
                          g_cute_argv = argv;\
                          g_cute_argc = argc;\
                          if (logpath != NULL) {\
                           cute_open_log_fd(logpath);\
                          }\
                          CUTE_RUN(entry);\
                          if (g_cute_leak_check && g_cute_mmap != NULL) {\
                           cute_log_memory_leak();\
                           del_cute_mmap_ctx(g_cute_mmap);\
                           exit_code = 1;\
                          }\
                          cute_close_log_fd();\
                          return exit_code;\
                         }

#define CUTE_GET_OPTION(o) ( cute_get_option(o, g_cute_argc, g_cute_argv, NULL) )

extern int g_cute_general_counter;

extern int g_cute_ran_tests;

extern FILE *g_cute_log_fd;

extern char **g_cute_argv;

extern int g_cute_argc;

extern int g_cute_leak_check;

extern void (*g_cute_fixture_setup)();

extern void (*g_cute_fixture_teardown)();

extern int g_cute_last_exec_line;

extern char *g_cute_last_ref_file;

extern struct cute_mmap_ctx *g_cute_mmap;

void cute_open_log_fd(const char *filepath);

void cute_close_log_fd();

void cute_log(const char *msg, ...);

char *cute_get_option(const char *option, int argc, char **argv, char *default_value);

void cute_log_memory_leak();

#endif
