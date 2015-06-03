/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _CUTEST_MEMORY_H
#define _CUTEST_MEMORY_H 1

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _WIN32

void *calloc(size_t nmemb, size_t size);

void *malloc(size_t size);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void init_memory_func_ptr();

#endif

#ifdef __cplusplus
}
#endif

#endif
