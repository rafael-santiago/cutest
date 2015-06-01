# Libcute usage guide

This document summarizes what is necessary to do in order to write unit tests for your project using ``libcute``.

## How to use it?

Firstly you need to include the main library header which stands for:

    #include <cute.h>

This header has lots of macro definitions including macros for test assertions, test running, etc.

Basically the ``main`` function from your test binary should include a call to the unit test entry point:

	int main(int argc, char **argv) {
	    CUTE_RUN(entry);
	    return 0;
	}

But for convenience ``cute`` brings a short way to define it:

        CUTE_MAIN(entry)

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

The example above will break if for some reason one be equals to zero. In this case the error message
passed as first argument will be shown. The second argument is the logical test assertion.

If your ``test-case`` needs fixtures you must declare the fixtures using the following schemas:

        CUTE_FIXTURE_SETUP(my_test_case)
            //  Some setup operations goes here.
        CUTE_FIXTURE_END

        CUTE_FIXTURE_TEARDOWN(my_test_case)
            //  Some teardown operations goes here.
        CUTE_FIXTURE_END

        CUTE_TEST_CASE(my_test_case)  //  now you only need to declare the test case in standard form.
            //  Some test asserts goes here.
        CUTE_TEST_CASE_END

        CUTE_TEST_CASE(entry)
            CUTE_RUN_TEST_WITH_FIXTURE(my_test_case); // the test invocation macro differs from tests that do not have fixtures.
        CUTE_TEST_CASE_END

Tests with fixtures must defined the both fixtures, the setup and the teardown, otherwise you will get errors related with unresolved externals.

## Available assertion macros

Until now these are the available assertion macros.

### CUTE_CHECK()

Implements a generic assertion.

``CUTE_CHECK(<error-message>, <logical-assertion>)``

Example:

	CUTE_CHECK("boo!", strcmp(buffer, "foobar") != 0);

### CUTE_CHECK_EQ()

Implements an ``equals-to`` assertion.

``CUTE_CHECK_EQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_EQ("error exit_code != 0", x, 0);

### CUTE_CHECK_NEQ()

Implements a ``not-equals-to`` assertion.

``CUTE_CHECK_NEQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_NEQ("error x == -1", x, -1);

### CUTE_CHECK_LE()

Implements a ``less-than`` assertion.

``CUTE_CHECK_LE(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_LE("x > 19", x, 20);

### CUTE_CHECK_LEQ()

Implements a ``less-than-or-equals-to`` assertion.

``CUTE_CHECK_LEQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_LEQ("x >= 21", x, 20);

### CUTE_CHECK_GR()

Implements a ``greater-than`` assertion.

``CUTE_CHECK_GE(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_GR("x <= 99", x, 100);

### CUTE_CHECK_GEQ()

Implements a ``greater-than-or-equals-to`` assertion.

``CUTE_CHECK_GEQ(<error-message>, <a>, <b>)``

Example:

	CUTE_CHECK_GEQ("x <= 19", x, 20);

## How to link my test with cute?

In order to ``link`` your test binary you can proceed like that:

``gcc your-test.c -oyour-test libcute.a``

Or still:

``gcc your-test.c -oyour-test -lcute``

If you put the libcute in a well-known place.

## Relevant options which your test binary accepts

Until now there is only one option: ``--cute-log-path=<filepath>``. This option specifies a file path
where the test log will be dumped.

## Reading command line options from your test cases

Sometimes is necessary... So to do it you should use the macro ``CUTE_GET_OPTION(<option-name>)``. Supposing that you want to read the option ``--foobar`` try this:

        char *foobar_value = CUTE_GET_OPTION("foobar");

Note that the options should be passed in this form to your test binary:

``somewhere/over/the/rainbow/your-test --foobar=option-with-content --flag-option``

When you try to read an unknown option the return is always ``NULL`` and ``flag options`` always return ``"1"``.

## Detecting memory leaks

Firstly be aware that the [``valgrind``](http://valgrind.org) usage is the best way to catch this kind of [raptors](https://xkcd.com/292/) inside your code. However ``cute`` brings a minimal system that performs memory leak detection. It could be a profitable way to detect this kind of issue as soon as possible. Because you will be looking for memory leaks being still on the test phase.

The usage of this system must be flagged passing the option ``--cute-leak-check`` to your unit test binary. Something like:

``something/leaking/over/the/rainbow/your-test --cute-leak-check``

When some memory leak is detected a report about this issue is include at the end of your general test report and the test binary exits with no zeroed ``exit code``.

The format of this memory leak report is as follows:

        cute INTERNAL ERROR: Memory leak(s) detected!!

        >>>
        Id=4 Address=0x0804C008 File=/leak-sample/main.c [The last check before leak was at line #113] < leak bloody leak...o > 20 byte(s).

        Leak total: 20 byte(s).
        <<<

The report includes:

- An allocation id.
- The effective leak's initial memory address.
- The file path where this leak is being caused.
- The line number where was done the last test assertion before leak.
- A content listing from this memory address and how many bytes are leaking.

In the end is also included a sum of all detected leaks.

The allocation id can be used in order to force a debug break. When you pass to your test binary the option ``--cute-leak-id=<allocation-id>`` a ``trap`` signal will be raised at the moment that the allocation id be equals to passed value.

