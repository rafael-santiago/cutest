/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "cutest.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if _MSC_VER
#include <BaseTsd.h>

#define ssize_t SSIZE_T
#endif

int g_cute_ran_tests = 0;

int g_cute_general_counter = 0;

FILE *g_cute_log_fd = NULL;

char **g_cute_argv = NULL;

int g_cute_argc = 0;

int g_cute_leak_check = 0;

int g_leak_sum = 0;

void (*g_cute_fixture_setup)() = NULL;

void (*g_cute_fixture_teardown)() = NULL;

int g_cute_last_exec_line = -1;

char *g_cute_last_ref_file = NULL;

struct cute_mmap_ctx *g_cute_mmap = NULL;

struct cute_mmap_ctx *g_mp = NULL;

char *g_cute_test_status = "idle";

char *g_cute_test_name = "(null)";

char g_cute_user_template[0xffff];

char *g_cute_assertion_message = NULL;

int g_cute_leak_id = 0;

static void *get_file();

static void *get_line();

static void *get_status();

static void *get_test_name();

static void *get_ran_tests();

static void *get_assertion_message();

static void *get_leak_id();

static void *get_leak_addr();

static void *get_leak_file_path();

static void *get_leak_line();

static void *get_leak_size();

static void *get_leak_sum();

static ssize_t get_cute_var_data(const char *vname);

static int cute_find_option_in_option_list(const char *option, const char *option_list);

enum {
    kInt,
    kStr,
    kRaw,
    kAddr,
    kSize
};

struct cute_vars_ctx {
    char *vname;
    void *(*get)();
    int   dtype;
};

static void *get_file() {
    return &g_cute_last_ref_file[0];
}

static void *get_line() {
    return &g_cute_last_exec_line;
}

static void *get_status() {
    return &g_cute_test_status[0];
}

static void *get_test_name() {
    return &g_cute_test_name[0];
}

static void *get_ran_tests() {
    return &g_cute_ran_tests;
}

static void *get_assertion_message() {
    return &g_cute_assertion_message[0];
}

static void *get_leak_id() {
    return &g_mp->id;
}

static void *get_leak_addr() {
    return g_mp->addr;
}

static void *get_leak_file_path() {
    return &g_mp->file_path[0];
}

static void *get_leak_line() {
    return &g_mp->line_nr;
}

static void *get_leak_size() {
    return &g_mp->size;
}

static void *get_leak_sum() {
    return &g_leak_sum;
}

#define CUTE_VARS_NR 13

static struct cute_vars_ctx cute_vars[CUTE_VARS_NR] = {
    {              "FILE",              get_file,   kStr},
    {              "LINE",              get_line,   kInt},
    {            "STATUS",            get_status,   kStr},
    {         "CASE_NAME",         get_test_name,   kStr},
    {      "RAN_TESTS_NR",         get_ran_tests,   kInt},
    { "ASSERTION_MESSAGE", get_assertion_message,   kStr},
    {           "LEAK_ID",           get_leak_id,   kInt},
    {         "LEAK_ADDR",         get_leak_addr,  kAddr},
    {    "LEAK_FILE_PATH",    get_leak_file_path,   kStr},
    {         "LEAK_LINE",         get_leak_line,   kInt},
    {         "LEAK_DATA",         get_leak_addr,   kRaw},
    {         "LEAK_SIZE",         get_leak_size,   kInt},
    {          "LEAK_SUM",          get_leak_sum,   kInt}
};

static ssize_t get_cute_var_data(const char *vname) {
    size_t v = 0;
    if (vname == NULL) {
        return -1;
    }
    for (v = 0; v < CUTE_VARS_NR; v++) {
        if (strcmp(cute_vars[v].vname, vname) == 0) {
            return v;
        }
    }
    return -1;
}

void cute_open_log_fd(const char *filepath) {
    cute_close_log_fd();
#ifndef _WIN32
    //  WARN(Santiago): This is a workaround for avoiding the 4k leak which stdio lets.
    //
    //                  It is caused due to an allocation done at the first time of printf's functions calling.
    //
    //                  It seems to be done for performance issues, but is a kind of annoying. When we are hunting for real leaks.
    //
    //                  I observed this problem under FreeBSD and only when logging the test's output for a file different of the stdout.
    //
    //                  Anyway, with the following setbuf()'s the 4k alloc is inhibited. What does it stop "leaking".
    //
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);
#endif
    g_cute_log_fd = fopen(filepath, "wb");
    if (g_cute_log_fd == NULL) {
        printf("cutest WARNING: Unable to create file \"%s\". All will be logged to stdout.\n", filepath);
    }
}

void cute_close_log_fd() {
    if (g_cute_log_fd != NULL && g_cute_log_fd != stdout) {
        fclose(g_cute_log_fd);
        g_cute_log_fd = NULL;
    }
}

void cute_log(const char *msg, ...) {
    const char *mp = msg;
    char *s = NULL, c = 0;
    int d = 0;
    void *m = NULL;
    int should_log_to_stdout = (g_cute_log_fd == NULL);
    va_list val;
    long long l = 0;
    char vname[0xff];
    void *vdata = NULL;
    size_t v = 0;
    size_t a = 0;
    ssize_t vindex = -1;
    const char *temp_mp = NULL;
    char assertion_message[0xffff] = "";
    if (mp == NULL) {
        return;
    }
    if (should_log_to_stdout) {
        g_cute_log_fd = stdout;
    }
    if (g_cute_user_template[0] != 0) {
        mp = (const char *) g_cute_user_template;
    }
    va_start(val, msg);
    while (*mp) {
        if(*mp == '%') {
            switch (*(mp + 1)) {
                case 's':
                    s = va_arg(val, char *);
                    fprintf(g_cute_log_fd, "%s", s);
                    break;

                case 'd':
                    d = va_arg(val, int);
                    fprintf(g_cute_log_fd, "%d", d);
                    break;

                case 'c':
                    c = (char) va_arg(val, int);
                    fprintf(g_cute_log_fd, "%c", c);
                    break;

                case 'x':
                    d = va_arg(val, int);
                    fprintf(g_cute_log_fd, "0x%.8X", d);
                    break;

                case 'm':
                    m = va_arg(val, void *);
                    fprintf(g_cute_log_fd, sizeof(void *) == 4 ? "0x%.8X" : "0x%.16X", m);
                    break;

                case 'l':
                    l = va_arg(val, long long);
                    fprintf(g_cute_log_fd, "%lld", l);
                    break;

                default:
                    fprintf(g_cute_log_fd, "%%");
                    break;
            }
            mp++;
        } else if (*mp == '\\') {
            switch (*(mp + 1)) {
                case 'n':
                    fprintf(g_cute_log_fd, "\n");
                    break;

                case 'r':
                    fprintf(g_cute_log_fd, "\r");
                    break;

                case 't':
                    fprintf(g_cute_log_fd, "\t");
                    break;

                default:
                    fprintf(g_cute_log_fd, "%c", *(mp + 1));
                    break;
            }
            mp++;
        } else if (*mp == '$') {
            mp++;
            temp_mp = mp;
            memset(vname, '\0', sizeof(vname));
            for (v = 0; v < sizeof(vname) && (isalpha(*mp) || *mp == '_'); v++, mp++) {
                vname[v] = *mp;
            }
            vindex = get_cute_var_data(vname);
            if (vindex == -1 || cute_vars[vindex].get == NULL) {
                fprintf(g_cute_log_fd, "$");
                mp = temp_mp;
                continue;
            } else {
                mp--;
                switch (cute_vars[vindex].dtype) {

                    case kStr:
                        if (strcmp(vname, "ASSERTION_MESSAGE") != 0) {
                            fprintf(g_cute_log_fd, "%s", (char *)cute_vars[vindex].get());
                        } else {
                            if (g_cute_test_status == NULL || strcmp(g_cute_test_status, CUTE_PASSED_LABEL) == 0) {
                                fprintf(g_cute_log_fd, "-");
                            } else {
                                fprintf(g_cute_log_fd, (char *)cute_vars[vindex].get());
                            }
                        }
                        break;

                    case kInt:
                        fprintf(g_cute_log_fd, "%d", *(int *)cute_vars[vindex].get());
                        break;

                    case kRaw:
                        vdata = cute_vars[vindex].get();
                        for (a = 0; a < g_mp->size; a++) {
                            if (isprint(((char *)vdata)[a])) {
                                fprintf(g_cute_log_fd, "%c", ((char *)vdata)[a]);
                            } else {
                                fprintf(g_cute_log_fd, ".");
                            }
                        }
                        break;

                    case kAddr:
                        vdata = cute_vars[vindex].get();
                        fprintf(g_cute_log_fd, sizeof(void *) == 4 ? "0x%.8X" : "0x%.16X", vdata);
                        break;

                }
            }
        } else {
            fprintf(g_cute_log_fd, "%c", *mp);
        }
        mp++;
    }
    fflush(g_cute_log_fd);
    if (should_log_to_stdout) {
        g_cute_log_fd = NULL;
    }
    va_end(val);
}

char *cute_get_option(const char *option, int argc, char **argv, char *default_value) {
    int a = 0;
    char optlabel[0xff];
    if (option == NULL || argv == NULL || *argv == NULL) {
        return default_value;
    }
    sprintf(optlabel, "--%s=", option);
    for (a = 0; a < argc; a++) {
        if (strstr(argv[a], optlabel) == argv[a]) {
            return argv[a] + strlen(optlabel);
        }
    }
    sprintf(optlabel, "--%s", option);
    for (a = 0; a < argc; a++) {
        if (strcmp(argv[a], optlabel) == 0) {
            return "1";
        }
    }
    return default_value;
}

void cute_log_memory_leak() {
    struct cute_mmap_ctx *mp;
    char *template_path = NULL;
    size_t a = 0;
    size_t leak_total = 0;
    template_path = cute_get_option("cutest-leak-log-header", g_cute_argc, g_cute_argv, NULL);
    if (template_path == NULL) {
        cute_log("\n\ncutest INTERNAL ERROR: Memory leak(s) detected!!\n\n>>>\n");
    } else {
        g_leak_sum = 0;
        for (mp = g_cute_mmap; mp != NULL; mp = mp->next) {
            g_leak_sum += mp->size;
        }
        cute_set_log_template(template_path);
        cute_log("");
        cute_set_log_template(NULL);
    }
    template_path = cute_get_option("cutest-leak-log-detail", g_cute_argc, g_cute_argv, NULL);
    if (template_path != NULL) {
        cute_set_log_template(template_path);
    }
    for (mp = g_cute_mmap; mp != NULL; mp = mp->next) {
        if (template_path == NULL) {
            cute_log("Id=%l Address=%m File=%s [The last check before leak was at line #%d] < ", mp->id, mp->addr, mp->file_path, mp->line_nr);
            for (a = 0; a < mp->size; a++) {
                if (isprint(((char *)mp->addr)[a] & 0xff)) {
                    cute_log("%c", ((char *)mp->addr)[a]);
                } else {
                    cute_log(".");
                }
            }
            cute_log(" > %d byte(s).\n", mp->size);
        } else {
            g_mp = mp;
            cute_log("");
            g_mp = NULL;
        }
        leak_total += mp->size;
    }
    if (template_path != NULL) {
        cute_set_log_template(NULL);
    }
    template_path = cute_get_option("cutest-leak-log-footer", g_cute_argc, g_cute_argv, NULL);
    if (template_path == NULL) {
        cute_log("\nLeak total: %d byte(s).\n<<<\n", leak_total);
    } else {
        cute_set_log_template(template_path);
        cute_log("");
        cute_set_log_template(NULL);
    }
}

void cute_set_log_template(const char *template_file_path) {
    FILE *fp = NULL;
    long file_size = 0;
    memset(g_cute_user_template, 0, sizeof(g_cute_user_template));
    if (template_file_path == NULL) {
        return;
    }
    fp = fopen(template_file_path, "rb");
    if (fp == NULL) {
        return;
    }
    fseek(fp, 0L, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fread(g_cute_user_template, 1, sizeof(g_cute_user_template) - 1, fp);
    fclose(fp);
}

static int cute_find_option_in_option_list(const char *option, const char *option_list) {
    const char *op = option_list;
    if (op == NULL || option == NULL) {
        return 0;
    }
    while (*op != 0) {
        if (strstr(op, option) == op && (*(op + strlen(option)) == 0 || *(op + strlen(option)) == ',')) {
            return 1;
        }
        while (*op != ',' && *op != 0) {
            op++;
        }
        if (*op == ',') {
            op++;
        }
        while (*op == ' ' || *op == '\n' || *op == '\r') {
            op++;
        }
    }
    return 0;
}

int cute_should_run_suite(const char *suite) {
    const char *cutest_run_suite = CUTE_GET_OPTION("cutest-run-suite");
    if (cutest_run_suite == NULL || suite == NULL) {
        return 1;
    }
    return cute_find_option_in_option_list(suite, cutest_run_suite);
}

int cute_should_run_test(const char *test) {
    const char *cutest_run_test = CUTE_GET_OPTION("cutest-run-test");
    if (cutest_run_test == NULL || test == NULL) {
        return 1;
    }
    return cute_find_option_in_option_list(test, cutest_run_test);
}
