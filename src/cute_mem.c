/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "cute_mem.h"
#include "cute.h"
#include "cute_mmap.h"

static void *(*tru_calloc)(size_t, size_t) = NULL;

static void *(*tru_malloc)(size_t) = NULL;

static void *(*tru_free)(void *) = NULL;

static void *(*tru_realloc)(void *, size_t) = NULL;

void init_memory_func_ptr() {
    void *handle = (void *)-1;
    tru_calloc = (void *)dlsym(handle, "calloc");
    tru_malloc = (void *)dlsym(handle, "malloc");
    tru_free = (void *)dlsym(handle, "free");
    tru_realloc = (void *)dlsym(handle, "realloc");
}

void *calloc(size_t nmemb, size_t size) {
    void *retval = NULL;
    if (tru_calloc == NULL) {
        cute_log("libcute INTERNAL ERROR: null tru_calloc().");
        return NULL;
    }
    if (g_cute_leak_check) {
    }
    retval = tru_calloc(nmemb, size);
    return retval;
}

void *malloc(size_t size) {
    void *retval = NULL;
    if (tru_malloc == NULL) {
        cute_log("libcute INTERNAL ERROR: null tru_malloc().");
        return NULL;
    }
    if (g_cute_leak_check) {
    }
    retval = tru_malloc(size);
    return retval;
}

void free(void *ptr) {
    if (tru_free == NULL) {
        cute_log("libcute INTERNAL ERROR: null tru_free().");
        return;
    }
    if (g_cute_leak_check) {
    }
    tru_free(ptr);
}

void *realloc(void *ptr, size_t size) {
    void *retval = NULL;
    if (tru_realloc == NULL) {
        cute_log("libcute INTERNAL ERROR: null tru_realloc().");
        return NULL;
    }
    if (g_cute_leak_check) {
    }
    retval = tru_realloc(ptr, size);
    return retval;
}
