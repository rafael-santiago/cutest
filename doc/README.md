# How to use it?

Firstly you need to include the main library header which stands for:

    #include <cute.h>

This header has lots of macro definitions including macros for test assertions, test running, etc.

Basically the ``main`` function from your test binary should include a call to the unit test entry point:

	int main(int argc, char **argv) {
	    CUTE_RUN(entry);
	    return 0;
	}

The entry point is merely an unit test case which must be declared using special definition macros:

	CUTE_TEST_CASE(entry)
	    (...)
	CUTE_TEST_CASE_END

The entry point is the place where you really should call your test-cases. The following
macro is destinated to call a ``test-case``:

	CUTE_RUN_TEST(my_previous_well_defined_test_case);

The ``test-cases`` should use the same definition schema that is used with the ``entry-point``. Look an
example:

	CUTE_TEST_CASE(one_should_differ_zero)
	    CUTE_CHECK("0 == 1!!", 1 != 0);
	CUTE_TEST_CASE_END

The example below will break if for some reason one be equals to zero. In this case the error message
passed as first argument will be shown. The second argument is the logical test assertion.

# Available assertion macros

Until now these are the available assertion macros.

## CUTE_CHECK()

Implements a generic assertion.

``CUTE_CHECK(<error-message>, <logical-assertion>)``

Example:

	CUTE_CHECK("boo!", strcmp(buffer, "foobar") != 0);

## CUTE_CHECK_EQ()

Implements an ``equals-to`` assertion.

``CUTE_CHECK_EQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_EQ("error exit_code != 0", x, 0);

## CUTE_CHECK_NEQ()

Implements a ``not-equals-to`` assertion.

``CUTE_CHECK_NEQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_NEQ("error x == -1", x, -1);

## CUTE_CHECK_LE()

Implements a ``less-than`` assertion.

``CUTE_CHECK_LE(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_LE("x > 19", x, 20);

## CUTE_CHECK_LEQ()

Implements a ``less-than-or-equals-to`` assertion.

``CUTE_CHECK_LEQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_LEQ("x >= 21", x, 20);

## CUTE_CHECK_GR()

Implements a ``greater-than`` assertion.

``CUTE_CHECK_GE(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_GR("x <= 99", x, 100);

## CUTE_CHECK_GEQ()

Implements a ``greater-than-or-equals-to`` assertion.

``CUTE_CHECK_GEQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_GEQ("x <= 19", x, 20);

# How to link my test with cute?

In order to ``link`` your test binary you can proceed like that:

``gcc your-test.c -oyour-test libcute.a``

Or still:

``gcc your-test.c -oyour-test -lcute``

If you put the libcute in a well-known place.
