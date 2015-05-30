/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef _CUTE_MMAP_H
#define _CUTE_MMAP_H 1

#include <stdlib.h>

static long long g_cute_mmap_id = 0;

struct cute_mmap_ctx {
    long long id;
    size_t size;
    void *addr;
    char file_path[8192];
    int line_nr;
    struct cute_mmap_ctx *next;
};

struct cute_mmap_ctx *add_allocation_to_cute_mmap_ctx(struct cute_mmap_ctx *mmap,
                                                      size_t size, void *addr);

struct cute_mmap_ctx *rm_allocation_from_cute_mmap_ctx(struct cute_mmap_ctx *mmap,
                                                       void *addr);

void del_cute_mmap_ctx(struct cute_mmap_ctx *mmap);


#endif
