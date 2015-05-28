/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _CUTE_MEM_H
#define _CUTE_MEM_H 1

#include <stdlib.h>

void *calloc(size_t nmemb, size_t size);

void *malloc(size_t size);

void free(void *ptr);

void *realloc(void *ptr, size_t size);

void init_memory_func_ptr();

#endif
