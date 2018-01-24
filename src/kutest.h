/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#ifndef KUTEST_H
#define KUTEST_H 1

#if defined(__FreeBSD__)

#include <sys/cdefs.h>
#include <sys/param.h>
#include <sys/module.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/libkern.h>

static int g_kutest_ran_tests = 0;

#define KUTE_ASSERT_CHECK(msg, chk) do {\
    if ((chk) == 0) {\
        uprintf("hmm bad, bad bug in %s at line %d: %s is false.\n", __FILE__, __LINE__, msg);\
        return 1;\
    }\
} while (0)

#define KUTE_RUN_TEST(test) do {\
    uprintf("-- running %s...\n", #test);\
    g_kutest_ran_tests++;\
    if (test() != 0) {\
        return 1;\
    }\
    uprintf("-- passed.\n");\
} while (0)

#elif defined(__NetBSD__)

#include <sys/module.h>

static int g_kutest_ran_tests = 0;

#define KUTE_ASSERT_CHECK(msg, chk) do {\
    if ((chk) == 0) {\
        uprintf("hmm bad, bad bug in %s at line %d: %s is false.\n", __FILE__, __LINE__, msg);\
        return 1;\
    }\
} while (0)

#define KUTE_RUN_TEST(test) do {\
    uprintf("-- running %s...\n", #test);\
    g_kutest_ran_tests++;\
    if (test() != 0) {\
        return 1;\
    }\
    uprintf("-- passed.\n");\
} while (0)

#elif defined(__linux__)

#include <linux/init.h>
#include <linux/module.h>

#if !defined(__GNUC__)

static int g_kutest_ran_tests = 0;

#else

__attribute__((__unused__)) static int g_kutest_ran_tests = 0;

#endif

#define KUTE_ASSERT_CHECK(msg, chk) do {\
    if ((chk) == 0) {\
        printk(KERN_ERR "hmm bad, bad bug in %s at line %d: %s is false.\n", __FILE__, __LINE__, msg);\
        return -EAGAIN;\
    }\
} while (0)

#define KUTE_RUN_TEST(test) do {\
    printk(KERN_ERR "-- running " #test "...\n");\
    g_kutest_ran_tests++;\
    if (test() != 0) {\
        return -EAGAIN;\
    }\
    printk(KERN_ERR "-- passed.\n");\
} while (0)

#endif

#define KUTE_CHECK_EQ(msg, a, b) KUTE_ASSERT_ASSERT_CHECK(msg, (a) == (b))

#define KUTE_CHECK_NEQ(msg, a, b) KUTE_ASSERT_CHECK(msg, (a) != (b))

#define KUTE_CHECK_LE(msg, a, b) KUTE_ASSERT_CHECK(msg, (a) < (b))

#define KUTE_CHECK_GR(msg, a, b) KUTE_ASSERT_CHECK(msg, (a) > (b))

#define KUTE_CHECK_LEQ(msg, a, b) KUTE_ASSERT_CHECK(msg, (a) <= (b))

#define KUTE_CHECK_GEQ(msg, a, b) KUTE_ASSERT_CHECK(msg, (a) >= (b))

#define KUTE_ASSERT(chk) KUTE_ASSERT_CHECK(#chk, chk)

#define KUTE_TEST_CASE(test) int test(void) {

#define KUTE_TEST_CASE_END      return 0;\
                           }

#define KUTE_DECLARE_TEST_CASE(test) int test(void)

#if defined(__FreeBSD__)

#define KUTE_MAIN(test)\
static int modld(struct module *module, int cmd, void *arg) {\
    int exit_code = 0;\
    switch (cmd) {\
        case MOD_LOAD:\
            uprintf("*** " #test " loaded...\n");\
            if ((exit_code = test()) == 0) {\
                uprintf("*** all tests passed. [%d test(s) ran]\n", g_kutest_ran_tests);\
            } else {\
                uprintf("fail: [%d test(s) ran]\n", g_kutest_ran_tests);\
            }\
            break;\
        case MOD_UNLOAD:\
            uprintf("*** " #test " unloaded.\n");\
            break;\
        default:\
            exit_code = EOPNOTSUPP;\
            break;\
    }\
    return exit_code;\
}\
static moduledata_t test ## _mod = {\
    #test,\
    modld,\
    NULL\
};\
DECLARE_MODULE(test, test ## _mod, SI_SUB_DRIVERS, SI_ORDER_MIDDLE);

#elif defined(__NetBSD__)

#define KUTE_MAIN(test)\
MODULE(MODULE_CLASS_DRIVER, test, NULL);\
static int test ##_modcmd(modcmd_t cmd, void *args) {\
    int exit_code = 0;\
    switch (cmd) {\
        case MODULE_CMD_INIT:\
            uprintf("***" #test " loaded...\n");\
            if ((exit_code = test()) == 0) {\
                uprintf("*** all tests passed. [%d test(s) ran]\n", g_kutest_ran_tests);\
            } else {\
                uprintf("fail: [%d test(s) ran]\n", g_kutest_ran_tests);\
            }\
            break;\
        case MOD_UNLOAD:\
            uprintf("*** " #test " unloaded.\n");\
            break;\
        default:\
            exit_code = EOPNOTSUPP;\
            break;\
    }\
    return exit_code;\
}

#elif defined(__linux__)

#define KUTE_MAIN(test)\
MODULE_LICENSE("GPL");\
static int mod_init(void) {\
    int exit_code = 1;\
    printk(KERN_ERR "*** " #test " loaded...\n");\
    if ((exit_code = test()) == 0) {\
        printk(KERN_ERR "*** all tests passed. [%d test(s) ran]\n", g_kutest_ran_tests);\
    } else {\
        printk(KERN_ERR "fail: [%d test(s) ran]\n", g_kutest_ran_tests);\
    }\
    return exit_code;\
}\
static void mod_fini(void) {\
    printk(KERN_ERR "*** " #test " unloaded.\n");\
}\
module_init(mod_init);\
module_exit(mod_fini);

#endif

#endif
