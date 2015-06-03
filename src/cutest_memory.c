/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "cutest_memory.h"
#include "cutest.h"
#include "cutest_mmap.h"
#ifndef _WIN32
#include <dlfcn.h>
#endif

static void *(*tru_calloc)(size_t, size_t) = NULL;

static void *(*tru_malloc)(size_t) = NULL;

static void *(*tru_free)(void *) = NULL;

static void *(*tru_realloc)(void *, size_t) = NULL;

static int g_memhook_init_done = 0;

void init_memory_func_ptr() {
#ifndef _WIN32
    void *handle = (void *)-1;
    tru_calloc = (void *)dlsym(handle, "calloc");
    tru_malloc = (void *)dlsym(handle, "malloc");
    tru_free = (void *)dlsym(handle, "free");
    tru_realloc = (void *)dlsym(handle, "realloc");
    g_memhook_init_done = 1;
#endif
}

#ifndef _WIN32

void *calloc(size_t nmemb, size_t size) {
    void *retval = NULL;
    if (tru_calloc == NULL) {
	if (g_memhook_init_done) {
	    cute_log("libcute INTERNAL ERROR: null tru_calloc().");
	}
        return NULL;
    }
    retval = tru_calloc(nmemb, size);
    if (g_cute_leak_check) {
	g_cute_mmap = add_allocation_to_cute_mmap_ctx(g_cute_mmap, size, retval);
    }
    return retval;
}

void *malloc(size_t size) {
    void *retval = NULL;
    if (tru_malloc == NULL) {
	if (g_memhook_init_done) {
	    cute_log("libcute INTERNAL ERROR: null tru_malloc().");
	}
        return NULL;
    }
    retval = tru_malloc(size);
    if (g_cute_leak_check) {
	g_cute_mmap = add_allocation_to_cute_mmap_ctx(g_cute_mmap, size, retval);
    }
    return retval;
}

void free(void *ptr) {
    if (tru_free == NULL) {
	if (g_memhook_init_done) {
	    cute_log("libcute INTERNAL ERROR: null tru_free().");
	}
        return;
    }
    if (g_cute_leak_check) {
	g_cute_mmap = rm_allocation_from_cute_mmap_ctx(g_cute_mmap, ptr);
    }
    tru_free(ptr);
}

void *realloc(void *ptr, size_t size) {
    void *retval = NULL;
    if (tru_realloc == NULL) {
	if (g_memhook_init_done) {
	    cute_log("libcute INTERNAL ERROR: null tru_realloc().");
	}
        return NULL;
    }
    retval = tru_realloc(ptr, size);
    if (g_cute_leak_check) {
	g_cute_mmap = rm_allocation_from_cute_mmap_ctx(g_cute_mmap, ptr);
	g_cute_mmap = add_allocation_to_cute_mmap_ctx(g_cute_mmap, size, retval);
    }
    return retval;
}

#endif
