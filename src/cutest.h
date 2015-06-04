/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _CUTEST_H
#define _CUTEST_H 1

#include <stdio.h>
#include <signal.h>
#ifndef _WIN32
#include <execinfo.h>
#endif
#include <unistd.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "cutest_memory.h"
#include "cutest_mmap.h"

#define CUTE_PASSED_LABEL "passed"

#define CUTE_FAILED_LABEL "failed"

#define CUTE_CHECK(msg, chk) do { g_cute_last_exec_line = __LINE__; g_cute_last_ref_file = __FILE__; g_cute_test_status = CUTE_PASSED_LABEL; if ((chk) == 0) { g_cute_test_status = CUTE_FAILED_LABEL; g_cute_assertion_message = msg; cute_log("hmm bad, bad bug in %s at line %d: ", __FILE__, __LINE__); cute_close_log_fd(); return msg; } } while (0)

#define CUTE_CHECK_EQ(msg, a, b) CUTE_CHECK(msg, (a) == (b))

#define CUTE_CHECK_NEQ(msg, a, b) CUTE_CHECK(msg, (a) != (b))

#define CUTE_CHECK_LE(msg, a, b) CUTE_CHECK(msg, (a) < (b))

#define CUTE_CHECK_GR(msg, a, b) CUTE_CHECK(msg, (a) > (b))

#define CUTE_CHECK_LEQ(msg, a, b) CUTE_CHECK(msg, (a) <= (b))

#define CUTE_CHECK_GEQ(msg, a, b) CUTE_CHECK(msg, (a) >= (b))

#define CUTE_RUN_TEST(test) do {\
                            if (g_cute_user_template[0] == 0) cute_log("-- running %s...\n", #test);\
                            g_cute_last_exec_line = __LINE__; g_cute_last_ref_file = __FILE__;\
                            g_cute_test_name = #test;\
                            g_cute_test_status = NULL;\
                            char *msg = test();\
                            g_cute_ran_tests++;\
                            if (msg != NULL) { g_cute_test_status = CUTE_FAILED_LABEL; return msg; }\
                            g_cute_test_status = CUTE_PASSED_LABEL;\
                            cute_log("-- passed.\n");\
                         } while (0)

#define CUTE_RUN_TEST_WITH_FIXTURE(test) do {\
                            g_cute_last_exec_line = __LINE__; g_cute_last_ref_file = __FILE__;\
                            g_cute_test_name = #test;\
                            g_cute_fixture_setup = test ## _setup;\
                            g_cute_fixture_teardown = test ## _teardown;\
                            test ## _setup();\
                            g_cute_fixture_setup = NULL;\
                            if (g_cute_user_template[0] == 0) cute_log("-- running %s...\n", #test);\
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

#define CUTE_RUN(entry) do {\
                            char *entry_return = entry();\
                            if (entry_return == NULL) {\
                                cute_set_log_template(NULL);\
                                user_template = cute_get_option("cutest-log-footer", argc, argv, NULL);\
                                if (user_template != NULL) {\
                                    cute_set_log_template(user_template);\
                                    cute_log("");\
                                }\
                                if (g_cute_user_template[0] == 0) {\
                                    cute_log("*** all tests passed. [%d test(s) ran]\n", g_cute_ran_tests);\
                                }\
                            } else {\
                                cute_close_log_fd();\
                                cute_set_log_template(NULL);\
                                user_template = cute_get_option("cutest-log-footer", argc, argv, NULL);\
                                if (user_template != NULL) {\
                                    cute_set_log_template(user_template);\
                                    cute_log("");\
                                    cute_set_log_template(NULL);\
                                } else {\
                                    cute_log("*** fail: %s [%d test(s) ran]\n", entry_return, g_cute_ran_tests);\
                                }\
                                return 1;\
                            }\
                        } while(0);

#define CUTE_TEST_CASE(test) char *test() {

#define CUTE_TEST_CASE_END  return NULL;\
                           }

#define CUTE_FIXTURE_SETUP(test) void test ## _setup() {

#define CUTE_FIXTURE_END  }

#define CUTE_FIXTURE_TEARDOWN(test) void test ## _teardown() {

#ifndef _WIN32

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
                          printf("-- CUTEST PANIC TRAP --\n");\
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
                          char *user_template = NULL;\
                          char *leak_id = NULL;\
                          int exit_code = 0;\
                          signal(SIGSEGV, sigsegv_watchdog);\
                          signal(SIGBUS, sigsegv_watchdog);\
                          signal(SIGABRT, sigsegv_watchdog);\
                          signal(SIGINT, sigint_watchdog);\
                          signal(SIGTERM, sigint_watchdog);\
                          init_memory_func_ptr();\
                          logpath = cute_get_option("cutest-log-path", argc, argv, NULL);\
                          if (cute_get_option("cutest-leak-check", argc, argv, NULL) != NULL) {\
                           g_cute_leak_check = 1;\
                          }\
                          leak_id = cute_get_option("cutest-leak-id", argc, argv, NULL);\
                          if (leak_id != NULL) {\
                           g_cute_leak_id = atoi(leak_id);\
                          }\
                          g_cute_argv = argv;\
                          g_cute_argc = argc;\
                          if (logpath != NULL) {\
                           cute_open_log_fd(logpath);\
                          }\
                          user_template = cute_get_option("cutest-log-header", argc, argv, NULL);\
                          if (user_template != NULL) {\
                           cute_set_log_template(user_template);\
                           cute_log("");\
                           cute_set_log_template(NULL);\
                          }\
                          user_template = cute_get_option("cutest-log-detail", argc, argv, NULL);\
                          if (user_template != NULL) {\
                           cute_set_log_template(user_template);\
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

#else

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
                          exit(1);\
                         }\
                         int main(int argc, char **argv) {\
                          char *logpath = NULL;\
                          char *user_template = NULL;\
                          char *leak_id = NULL;\
                          int exit_code = 0;\
                          signal(SIGSEGV, sigsegv_watchdog);\
                          signal(SIGABRT, sigsegv_watchdog);\
                          signal(SIGINT, sigint_watchdog);\
                          signal(SIGTERM, sigint_watchdog);\
                          init_memory_func_ptr();\
                          logpath = cute_get_option("cutest-log-path", argc, argv, NULL);\
                          if (cute_get_option("cutest-leak-check", argc, argv, NULL) != NULL) {\
                           g_cute_leak_check = 1;\
                          }\
                          leak_id = cute_get_option("cutest-leak-id", argc, argv, NULL);\
                          if (leak_id != NULL) {\
                           g_cute_leak_id = atoi(leak_id);\
                          }\
                          g_cute_argv = argv;\
                          g_cute_argc = argc;\
                          if (logpath != NULL) {\
                           cute_open_log_fd(logpath);\
                          }\
                          user_template = cute_get_option("cutest-log-header", argc, argv, NULL);\
                          if (user_template != NULL) {\
                           cute_set_log_template(user_template);\
                           cute_log("");\
                           cute_set_log_template(NULL);\
                          }\
                          user_template = cute_get_option("cutest-log-detail", argc, argv, NULL);\
                          if (user_template != NULL) {\
                           cute_set_log_template(user_template);\
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

#endif

#define CUTE_GET_OPTION(o) ( cute_get_option(o, g_cute_argc, g_cute_argv, NULL) )

#define CUTE_CASE_NAME ( g_cute_test_name )

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

extern char *g_cute_test_status;

extern char *g_cute_test_name;

extern char *g_cute_test_log_header;

extern char *g_cute_test_log_detail;

extern char *g_cute_test_log_footer;

extern char g_cute_user_template[0xffff];

extern char *g_cute_assertion_message;

extern int g_cute_leak_id;

void cute_open_log_fd(const char *filepath);

void cute_close_log_fd();

void cute_log(const char *msg, ...);

char *cute_get_option(const char *option, int argc, char **argv, char *default_value);

void cute_log_memory_leak();

void cute_set_log_template(const char *template_file_path);

#if __cplusplus
}
#endif

#endif
