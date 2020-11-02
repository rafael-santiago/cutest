/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#if !(defined(_KERNEL) && defined(_LKM) && defined(__NetBSD__))

#include "cutest_memory.h"
#include "cutest.h"
#include "cutest_mmap.h"
#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#ifndef _WIN32

#ifndef RTLD_NEXT

#define RTLD_NEXT -1

#endif

#endif

void *(*tru_calloc)(size_t, size_t) = NULL;

void *(*tru_malloc)(size_t) = NULL;

void *(*tru_free)(void *) = NULL;

void *(*tru_realloc)(void *, size_t) = NULL;

static int g_memhook_init_done = 0;

void init_memory_func_ptr() {
#ifdef _WIN32
    HMODULE handle = NULL;
    char *msvcr_filenames[] = {
        "MSVCRT.dll",
        "MSVCR70.dll",
        "MSVCR70d.dll",
        "MSVCR71.dll",
        "MSVCR71d.dll",
        "MSVCR80.dll",
        "MSCVR80d.dll",
        "MSVCR90.dll",
        "MSVCR90d.dll",
        "MSVCR100.dll",
        "MSVCR100d.dll",
        "MSVCR110.dll",
        "MSVCR110d.dll",
        "MSVCR120.dll",
        "MSCVR120d.dll"
    };
    size_t msvcr_filenames_count = sizeof(msvcr_filenames) / sizeof(msvcr_filenames[0]);
    size_t m = 0;
#endif
    if (tru_calloc != NULL && tru_malloc != NULL && tru_realloc != NULL && tru_free != NULL) {
        return;
    }
#ifndef _WIN32
    void *handle = (void *)RTLD_NEXT;
    tru_calloc = (void *)dlsym(handle, "calloc");
    tru_malloc = (void *)dlsym(handle, "malloc");
    tru_free = (void *)dlsym(handle, "free");
    tru_realloc = (void *)dlsym(handle, "realloc");
    g_memhook_init_done = 1;
#else
    if (handle == NULL) {
        for (m = 0; m < msvcr_filenames_count && handle == NULL; m++) {
            handle = GetModuleHandle(msvcr_filenames[m]);
        }
    }
    if (handle != NULL) {
        tru_calloc = (void *)GetProcAddress(handle, "calloc");
        tru_malloc = (void *)GetProcAddress(handle, "malloc");
        tru_free  = (void *)GetProcAddress(handle, "free");
        tru_realloc = (void *)GetProcAddress(handle, "realloc");
        g_memhook_init_done = 1;
    } else {
        cute_log("libcutest INTERNAL ERROR: unable to find \"MSVCRT.dll\".\n");
    }
#endif
}

void *calloc(size_t nmemb, size_t size) {
    void *retval = NULL;
    if (tru_calloc == NULL) {
#if defined(_WIN32) || defined(__FreeBSD__)
        init_memory_func_ptr();
#endif
        //if (g_memhook_init_done) {
        //    cute_log("libcutest INTERNAL ERROR: null tru_calloc().\n");
        //}
        if (tru_calloc == NULL) {
            return NULL;
        }
    }
    retval = tru_calloc(nmemb, size);
    if (g_cute_leak_check) {
        g_cute_mmap = add_allocation_to_cute_mmap_ctx(g_cute_mmap, size, retval, &g_cute_mmap_tail);
    }
    return retval;
}

void *malloc(size_t size) {
    void *retval = NULL;
    if (tru_malloc == NULL) {
#if defined(_WIN32) || defined(__FreeBSD__)
        init_memory_func_ptr();
#endif
        //if (g_memhook_init_done) {
        //    cute_log("libcutest INTERNAL ERROR: null tru_malloc().\n");
        //}
        if (tru_malloc == NULL) {
            return NULL;
        }
    }
    retval = tru_malloc(size);
    if (g_cute_leak_check) {
        g_cute_mmap = add_allocation_to_cute_mmap_ctx(g_cute_mmap, size, retval, &g_cute_mmap_tail);
    }
    return retval;
}

void free(void *ptr) {
    if (tru_free == NULL) {
#if defined(_WIN32) || defined(__FreeBSD__)
        init_memory_func_ptr();
#endif
        //if (g_memhook_init_done) {
        //    cute_log("libcutest INTERNAL ERROR: null tru_free().\n");
        //}
        if (tru_free == NULL) {
            return;
        }
    }
    if (ptr == NULL) {
        return;
    }
    if (g_cute_leak_check) {
        g_cute_mmap = rm_allocation_from_cute_mmap_ctx(g_cute_mmap, ptr, &g_cute_mmap_tail);
    }
    tru_free(ptr);
}

void *realloc(void *ptr, size_t size) {
    void *retval = NULL;
    if (tru_realloc == NULL) {
#if defined(_WIN32) || defined(__FreeBSD__)
        init_memory_func_ptr();
#endif
        //if (g_memhook_init_done) {
        //    cute_log("libcutest INTERNAL ERROR: null tru_realloc().\n");
        //}
        if (tru_realloc == NULL) {
            return NULL;
        }
    }
    retval = tru_realloc(ptr, size);
    if (g_cute_leak_check) {
        g_cute_mmap = rm_allocation_from_cute_mmap_ctx(g_cute_mmap, ptr, &g_cute_mmap_tail);
        g_cute_mmap = add_allocation_to_cute_mmap_ctx(g_cute_mmap, size, retval, &g_cute_mmap_tail);
    }
    return retval;
}

#endif
