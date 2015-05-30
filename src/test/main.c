/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "../cute.h"
#include "../cute_mmap.h"

static int g_counter = 0;

//  INFO(Santiago): nothing to do here but it breaks the compilation
//                  if there is something wrong with the macro definition.
CUTE_TEST_CASE(test_decl)
CUTE_TEST_CASE_END

char *get_test_decl_return() {
    CUTE_RUN_TEST(test_decl);
    return NULL;
}

CUTE_TEST_CASE(assertion_tests)
    unsigned int foo = 0xdeadbeef;
    CUTE_CHECK("foo != 0xdeadbeef", foo == 0xdeadbeef);
    CUTE_CHECK_EQ("foo != 0xdeadbeef", foo, 0xdeadbeef);
    CUTE_CHECK_NEQ("foo == 0xf00b45", foo, 0xf00b45);
    foo = 10;
    CUTE_CHECK_LE("foo > 11", foo, 11);
    CUTE_CHECK_GR("foo > 9", foo, 9);
    CUTE_CHECK_LEQ("foo > 10", foo, 10);
    CUTE_CHECK_GEQ("foo < 10", foo, 10);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(get_option_tests)
    char *argv[] = {
        "--cute-log-path=/usr/boo",
        "--passed"
    };
    CUTE_CHECK_EQ("cute_get_option() != NULL", NULL, cute_get_option("none", 2, argv, NULL));
    CUTE_CHECK_EQ("cute_get_option() != NULL", NULL, cute_get_option(NULL, 2, argv, NULL));
    CUTE_CHECK_EQ("cute_get_option() != \"/usr/boo\"", 0, strcmp(cute_get_option("cute-log-path", 2, argv, "(null)"), "/usr/boo"));
    CUTE_CHECK_EQ("cute_get_option() != \"1\"", 0, strcmp(cute_get_option("passed", 2, argv, "0"), "1"));
CUTE_TEST_CASE_END

CUTE_FIXTURE_SETUP(fixture_test)
    g_counter = 1;
CUTE_FIXTURE_END

CUTE_FIXTURE_TEARDOWN(fixture_test)
    g_counter++;
CUTE_FIXTURE_END

CUTE_TEST_CASE(fixture_test)
    CUTE_CHECK_EQ("g_counter != 1", g_counter, 1);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(CUTE_GET_OPTION_MACRO_test)
    char *value = NULL;
    CUTE_CHECK_EQ("CUTE_GET_OPTION(\"superunknown\") != NULL", CUTE_GET_OPTION("superunknown"), NULL);
    value = CUTE_GET_OPTION("foo");
    CUTE_CHECK_NEQ("CUTE_GET_OPTION(\"foo\") == NULL", value, NULL);
    CUTE_CHECK("CUTE_GET_OPTION(\"foo\") != \"bar\"", strcmp(value, "bar") == 0);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(cute_mmap_ctx_general_tests)
    struct cute_mmap_ctx *mmap = NULL;
    struct cute_mmap_ctx *mp = NULL;
    char byte = 0;
    int integer = 0;
    float real = 0;
    size_t mmap_nr = 0;
    size_t m = 0;
    struct expected_mmap_state {
        size_t size;
        void *ptr;
    };
    struct expected_mmap_state ems[] = {
        { sizeof(byte), &byte },
        { sizeof(integer), &integer },
        { sizeof(real), &real }
    };
    mmap = add_allocation_to_cute_mmap_ctx(mmap, sizeof(byte), &byte);
    mmap = add_allocation_to_cute_mmap_ctx(mmap, sizeof(integer), &integer);
    mmap = add_allocation_to_cute_mmap_ctx(mmap, sizeof(real), &real);
    CUTE_CHECK("mmap == NULL", mmap != NULL);
    mmap_nr = 0;
    for (mp = mmap; mp != NULL; mp = mp->next, mmap_nr++);
    CUTE_CHECK("mmap_nr != 3", mmap_nr == 3);
    mmap_nr = sizeof(ems) / sizeof(ems[0]);
    for (m = 0, mp = mmap; m < mmap_nr; m++, mp = mp->next) {
        CUTE_CHECK_EQ("ems[m].size != mp->size", ems[m].size, mp->size);
        CUTE_CHECK_EQ("ems[m].ptr != mp->addr", ems[m].ptr, mp->addr);
    }
    mmap = rm_allocation_from_cute_mmap_ctx(mmap, &byte);
    mmap_nr = 0;
    for (mp = mmap; mp != NULL; mp = mp->next, mmap_nr++);
    CUTE_CHECK("mmap_nr != 2", mmap_nr == 2);

    mmap = rm_allocation_from_cute_mmap_ctx(mmap, &real);
    mmap_nr = 0;
    for (mp = mmap; mp != NULL; mp = mp->next, mmap_nr++);
    CUTE_CHECK("mmap_nr != 1", mmap_nr == 1);

    mmap = rm_allocation_from_cute_mmap_ctx(mmap, NULL);
    mmap_nr = 0;
    for (mp = mmap; mp != NULL; mp = mp->next, mmap_nr++);
    CUTE_CHECK("mmap_nr != 1", mmap_nr == 1);

    mmap = rm_allocation_from_cute_mmap_ctx(mmap, &integer);
    mmap_nr = 0;
    for (mp = mmap; mp != NULL; mp = mp->next, mmap_nr++);
    CUTE_CHECK("mmap_nr != 1", mmap_nr == 0);

    del_cute_mmap_ctx(mmap);
CUTE_TEST_CASE_END

CUTE_TEST_CASE(leak_check_tests)
    char *my_sloppy_allocation = NULL;
    int *my_sloppy_int = NULL;
    if (g_cute_leak_check == 0) {
        g_cute_leak_check = 1;
        my_sloppy_allocation = (char *) malloc(8192);
        my_sloppy_int = (int *) malloc(sizeof(int));
        CUTE_CHECK("g_cute_mmap == NULL", g_cute_mmap != NULL);
        CUTE_CHECK("g_cute_mmap->addr != my_sloppy_allocation", g_cute_mmap->addr == my_sloppy_allocation);
        CUTE_CHECK("g_cute_mmap->next == NULL", g_cute_mmap->next != NULL);
        CUTE_CHECK("g_cute_mmap->next->addr != my_sloopy_int", g_cute_mmap->next->addr == my_sloppy_int);
        CUTE_CHECK("g_cute_mmap->next->next != NULL", g_cute_mmap->next->next == NULL);
        free(my_sloppy_allocation);
        free(my_sloppy_int);
        CUTE_CHECK("g_cute_mmap != NULL", g_cute_mmap == NULL);
        g_cute_leak_check = 0;
    } else {
        cute_log("WARNING: Skipped test. Unable to run \"leak_check_tests\". You need to disable cute's memory leak check system in order to run this.\n");
    }
CUTE_TEST_CASE_END

CUTE_TEST_CASE(entry)
    char *retval = get_test_decl_return();
    CUTE_CHECK_EQ("retval != NULL", retval, NULL);
    CUTE_RUN_TEST(assertion_tests);
    CUTE_RUN_TEST(get_option_tests);
    CUTE_RUN_TEST_WITH_FIXTURE(fixture_test);
    CUTE_CHECK_EQ("g_counter != 2", g_counter, 2);
    CUTE_RUN_TEST(CUTE_GET_OPTION_MACRO_test);
    CUTE_RUN_TEST(cute_mmap_ctx_general_tests);
    CUTE_RUN_TEST(leak_check_tests);
CUTE_TEST_CASE_END

CUTE_MAIN(entry)
