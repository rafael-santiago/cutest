/*
 *                                Copyright (C) 2015 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include "alien_test_case.h"

CUTE_TEST_CASE(alien_test_case)
    CUTE_CHECK("0 == 1", 0 != 1);
CUTE_TEST_CASE_END

CUTE_FIXTURE_SETUP(alien_fixture_test_case)
CUTE_FIXTURE_END

CUTE_TEST_CASE(alien_fixture_test_case)
    CUTE_CHECK("0 == 1", 0 != 1);
CUTE_TEST_CASE_END

CUTE_FIXTURE_TEARDOWN(alien_fixture_test_case)
CUTE_FIXTURE_END
