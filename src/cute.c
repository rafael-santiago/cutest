/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "cute.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int g_cute_ran_tests = 0;

int g_cute_general_counter = 0;

FILE *g_cute_log_fd = NULL;

char **g_cute_argv = NULL;

int g_cute_argc = 0;

void (*g_cute_fixture_setup)() = NULL;

void (*g_cute_fixture_teardown)() = NULL;

int g_cute_last_exec_line = -1;

char *g_cute_last_ref_file = NULL;

void cute_open_log_fd(const char *filepath) {
    cute_close_log_fd();
    g_cute_log_fd = fopen(filepath, "wb");
    if (g_cute_log_fd == NULL) {
        printf("cute WARNING: Unable to create file \"%s\". All will be logged to stdout.\n", filepath);
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
    char *s, c;
    int d;
    int should_log_to_stdout = (g_cute_log_fd == NULL);
    va_list val;
    if (mp == NULL) {
        return;
    }
    if (should_log_to_stdout) {
        g_cute_log_fd = stdout;
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
        if (strstr(argv[a], optlabel) == argv[a]) {
            return "1";
        }
    }
    return default_value;
}
