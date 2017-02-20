/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "cutest_mmap.h"
#include "cutest.h"
#include <string.h>
#ifndef _WIN32
#ifndef HAS_NO_PTHREAD
#include <pthread.h>
#endif
#else
#include <windows.h>
#endif
#include <signal.h>


static int g_temp_cute_leak_check = 0;

#define new_cute_mmap_ctx(m) ( g_temp_cute_leak_check = g_cute_leak_check,\
                               g_cute_leak_check = 0,\
                               (m) = tru_malloc(sizeof(struct cute_mmap_ctx)),\
                               (m)->next = NULL,\
                               (m)->line_nr = g_cute_last_exec_line,\
                               strncpy((m)->file_path, g_cute_last_ref_file, sizeof((m)->file_path)-1),\
                               g_cute_leak_check = g_temp_cute_leak_check )

#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

static pthread_mutex_t mmap_mutex = PTHREAD_MUTEX_INITIALIZER;

#endif

#else

HANDLE mmap_mutex;

#endif

void init_mmap_mutex() {
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mmap_mutex, &attr);
    pthread_mutexattr_destroy(&attr);
#endif

#else
    mmap_mutex = CreateMutex(NULL, FALSE, NULL);
#endif
}

void deinit_mmap_mutex() {
#ifdef _WIN32
    CloseHandle(mmap_mutex);
#endif
}

static struct cute_mmap_ctx *get_cute_mmap_ctx_tail(struct cute_mmap_ctx *mmap) {
    struct cute_mmap_ctx *p;
    if (mmap == NULL) {
        return NULL;
    }
    for (p = mmap; p->next != NULL; p = p->next)
         ;
    return p;
}

struct cute_mmap_ctx *add_allocation_to_cute_mmap_ctx(struct cute_mmap_ctx *mmap,
                                                      size_t size, void *addr) {
    struct cute_mmap_ctx *head = NULL;
    struct cute_mmap_ctx *p = NULL;
    if (g_cute_last_ref_file == NULL) {
        return mmap;
    }
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

    pthread_mutex_lock(&mmap_mutex);

#endif

#else
    WaitForSingleObject(mmap_mutex, INFINITE);
#endif
    head = mmap;
    if (head == NULL) {
        new_cute_mmap_ctx(head);
        p = head;
    } else {
        p = get_cute_mmap_ctx_tail(mmap);
        new_cute_mmap_ctx(p->next);
        p = p->next;
    }
    p->id = ++g_cute_mmap_id;
    p->size = size;
    p->addr = addr;
    if (p->id == g_cute_leak_id) {
#ifndef _WIN32
        raise(SIGTRAP);
#else
        DebugBreak();
#endif
    }
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

    pthread_mutex_unlock(&mmap_mutex);

#endif

#else
    ReleaseMutex(mmap_mutex);
#endif
    return head;
}

struct cute_mmap_ctx *rm_allocation_from_cute_mmap_ctx(struct cute_mmap_ctx *mmap,
                                                       void *addr) {
    struct cute_mmap_ctx *head = NULL;
    struct cute_mmap_ctx *burn = NULL;
    struct cute_mmap_ctx *last = NULL;
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

    pthread_mutex_lock(&mmap_mutex);

#endif

#else
    WaitForSingleObject(mmap_mutex, INFINITE);
#endif
    head = mmap;
    if (mmap == NULL) {
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

        pthread_mutex_unlock(&mmap_mutex);

#endif

#else
        ReleaseMutex(mmap_mutex);
#endif
        return NULL;
    }
    for (burn = mmap; burn != NULL; last = burn, burn = burn->next) {
        if (burn->addr == addr) {
            break;
        }
    }
    if (burn != NULL) {
        if (last == NULL) {
            head = burn->next;
            burn->next = NULL;
        } else {
            last->next = burn->next;
            burn->next = NULL;
        }
        free(burn);
    }
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

    pthread_mutex_unlock(&mmap_mutex);

#endif

#else
    ReleaseMutex(mmap_mutex);
#endif
    return head;
}

void del_cute_mmap_ctx(struct cute_mmap_ctx *mmap) {
    struct cute_mmap_ctx *p = NULL, *t = NULL;
    int temp = 0;
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

    pthread_mutex_lock(&mmap_mutex);

#endif

#else
    WaitForSingleObject(mmap_mutex, INFINITE);
#endif
    temp = g_cute_leak_check;
    g_cute_leak_check = 0;
    for (t = p = mmap; t != NULL; p = t) {
        t = p->next;
        tru_free(p);
    }
    g_cute_leak_check = temp;
#ifndef _WIN32

#ifndef HAS_NO_PTHREAD

    pthread_mutex_unlock(&mmap_mutex);

#endif

#else
    ReleaseMutex(mmap_mutex);
#endif
}
