# Libcutest usage guide

This document summarizes what is necessary to do in order to write unit tests for your project using ``libcutest``.

## How to use it?

Firstly you need to include the main library header which stands for:

```c
    #include <cutest.h>
```

This header has lots of macro definitions including macros for test assertions, test running, etc.

Basically the ``main`` function from your test binary should include a call to the unit test entry point:

```c
        int main(int argc, char **argv) {
            CUTE_RUN(entry);
            return 0;
        }
```

But for convenience ``cutest`` brings a short way to define it:

```c
        CUTE_MAIN(entry)
```

The entry point is merely an unit test case which must be declared using special definition macros:

```c
        CUTE_TEST_CASE(entry)
            (...)
        CUTE_TEST_CASE_END
```

The entry point is the place where you really should call your test-cases. The following
macro is destinated to call a ``test-case``:

```c
        CUTE_RUN_TEST(my_previous_well_defined_test_case);
```

The ``test-cases`` should use the same definition schema that is used with the ``entry-point``. Look an
example:

```c
        CUTE_TEST_CASE(one_should_differ_zero)
            CUTE_CHECK("0 == 1!!", 1 != 0);
        CUTE_TEST_CASE_END
```

The example above will break if for some reason one be equals to zero. In this case the error message
passed as first argument will be shown. The second argument is the logical test assertion.

If your ``test-case`` needs fixtures you must declare the fixtures using the following schemas:

```c
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
```

Tests with fixtures must define the both fixtures, the ``setup`` and the ``teardown``, otherwise you will get errors related with unresolved externals.

## Not implementing all test cases in just one source file

Sometimes is necessary divide the implementation among several files finding to keep our sanity. Then in this case you should use the macros destinated to do the tests prototypes. Take a look:

```c
        // filesystem_test_cases.h
        #ifndef _FILESYSTEM_TEST_CASES_H
        #define _FILESYSTEM_TEST_CASES_H 1

        #include <cutest.h>

        CUTE_DECLARE_TEST_CASE(test_fopen);

        CUTE_DECLARE_TEST_CASE_WITH_FIXTURE(test_remove);

        CUTE_DECLARE_TEST_CASE_WITH_FIXTURE(test_mkdir);

        #endif
```

Now supposing that ``filesystem_test_cases.c`` has already implemented all you need to do inside of your main source file
or at least where you plan call your test cases:

```c
        // main.c
        #include "filesystem_test_cases.h"

        (...)

        CUTE_TEST_CASE(entry)
            CUTE_RUN_TEST(test_fopen);
            CUTE_RUN_TEST_WITH_FIXTURE(test_remove);
            CUTE_RUN_TEST_WITH_FIXTURE(test_mkdir);
        CUTE_TEST_CASE_END
```

## Available assertion macros

Until now these are the available assertion macros.

### CUTE_CHECK()

Implements a generic assertion.

``CUTE_CHECK(<error-message>, <logical-assertion>)``

Example:

```c
        CUTE_CHECK("boo!", strcmp(buffer, "foobar") != 0);
```

### CUTE_CHECK_EQ()

Implements an ``equals-to`` assertion.

``CUTE_CHECK_EQ(<error-message>, <a>, <b>)``

Example:

```c
        CUTE_CHECK_EQ("error exit_code != 0", x, 0);
```

### CUTE_CHECK_NEQ()

Implements a ``not-equals-to`` assertion.

``CUTE_CHECK_NEQ(<error-message>, <a>, <b>)``

Example:

```c
        CUTE_CHECK_NEQ("error x == -1", x, -1);
```

### CUTE_CHECK_LE()

Implements a ``less-than`` assertion.

``CUTE_CHECK_LE(<error-message>, <a>, <b>)``

Example:

```c
        CUTE_CHECK_LE("x > 19", x, 20);
```

### CUTE_CHECK_LEQ()

Implements a ``less-than-or-equals-to`` assertion.

``CUTE_CHECK_LEQ(<error-message>, <a>, <b>)``

Example:

        CUTE_CHECK_LEQ("x >= 21", x, 20);

### CUTE_CHECK_GR()

Implements a ``greater-than`` assertion.

``CUTE_CHECK_GE(<error-message>, <a>, <b>)``

Example:

```c
        CUTE_CHECK_GR("x <= 99", x, 100);
```

### CUTE_CHECK_GEQ()

Implements a ``greater-than-or-equals-to`` assertion.

``CUTE_CHECK_GEQ(<error-message>, <a>, <b>)``

Example:

```c
        CUTE_CHECK_GEQ("x <= 19", x, 20);
```

### Ok, maybe you do not want to specify error messages on assertions

In this case you should use the ``CUTE_ASSERT`` macro group instead of ``CUTE_CHECK``. So take a look at
``Table 1`` to know more details about them.

**Table 1**: Single assertion macros with no custom messages.

|     *Macro*       |       *Assertion type*     |                 *Prototype*                 |   *Usage example*          |
|:-----------------:|:--------------------------:|:-------------------------------------------:|:--------------------------:|
|``CUTE_ASSERT``    | ``generic``                | ``CUTE_ASSERT(<logical-assertion>)``        | ``CUTE_ASSERT(0 == 1)``    |
|``CUTE_ASSERT_EQ`` | ``equals``                 | ``CUTE_ASSERT_EQ(<a>, <b>)``                | ``CUTE_ASSERT_EQ(0, 1)``   |
|``CUTE_ASSERT_NEQ``| ``not-equals``             | ``CUTE_ASSERT_NEQ(<a>, <b>)``               | ``CUTE_ASSERT_NEQ(0, 0)``  |
|``CUTE_ASSERT_LE`` | ``less-than``              | ``CUTE_ASSERT_LE(<a>, <b>)``                | ``CUTE_ASSERT_LE(0, 0)``   |
|``CUTE_ASSERT_GR`` | ``greater-than``           | ``CUTE_ASSERT_GR(<a>, <b>)``                | ``CUTE_ASSERT_GR(0, 0)``   |
|``CUTE_ASSERT_LEQ``| ``less-than-or-equals``    | ``CUTE_ASSERT_LEQ(<a>, <b>)``               | ``CUTE_ASSERT_LEQ(x, 0)``  |
|``CUTE_ASSERT_GEQ``| ``greater-than-or-equals`` | ``CUTE_ASSERT_GEQ(<a>, <b>)``               | ``CUTE_ASSERT_GEQ(x, -1)`` |

However, I think that the lack of good custom assertion error messages can sometimes obfuscate the test report and as result
it also can consume more time on understanding and on the correct fix applying. In my opinion you should use it with moderation.

## Grouping tests inside suites

For it is necessary use two specific macros. One for suite declaring and another for test suite running. Look:

```c
        CUTE_TEST_CASE_SUITE(my_sample_suite)
            CUTE_RUN_TEST(my_sample_suite_case_a);
            CUTE_RUN_TEST(my_sample_suite_case_b);
            CUTE_RUN_TEST(my_sample_suite_case_z);
        CUTE_TEST_CASE_SUITE_END

        CUTE_TEST_CASE(tests_entry)
            CUTE_RUN_TEST_SUITE(my_sample_suite);
        CUTE_TEST_CASE_END

        CUTE_MAIN(tests_entry)
```

All tests between ``CUTE_TEST_CASE_SUITE(my_sample_suite)`` and ``CUTE_TEST_CASE_SUITE_END`` are owned by suite ``my_sample_suite``.
For this suite running is used a special macro called ``CUTE_RUN_TEST_SUITE`` as you can see above on ``tests_entry``.

If you want to run only specific test suites you should use the option ``--cutest-run-suite`` as follows:

``./your-tidy-unit --cutest-run-suite=my_sample_suite,suite_x,suite_y,suite_z``

Tests called directly with ``CUTE_RUN_TEST`` macro are unconditional and for this reason always executed.

If for some reason you need to prototype the suites use the macro ``CUTE_DECLARE_TEST_CASE_SUITE(suite_name)``.

## Running only specific tests

If you just want to run one or few ``test-cases`` you should use the option ``--cutest-run-test`` as follows:

``./your-huge-unit --cutest-run-test=test_a,test_b,test_d,test_w``

In this example only the tests ``test_a``, ``test_b``, ``test_d`` and ``test_w`` will run. Beware that the ``--cutest-run-test``
option has more precedence than ``--cutest-run-suite`` option.

## How can I print the current case name?

Sometimes for debug issues you may need to print this piece of information. So you could try this:

```c
        printf("Oh my God! Houston we got Raptors in %s\n", CUTE_CASE_NAME);
```

Or:

```c
        cute_log("Oh my God! Houston, we got Raptors in %s!\n", CUTE_CASE_NAME);
```

Or still:

```c
        cute_log("Oh my God! Houston, we got Raptors in $CASE_NAME!\n");
```

## How to link my test with cutest?

In order to ``link`` your test binary you can proceed as follows:

``gcc your-test.c -oyour-test libcutest.a``

Or still (if you put ``libcutest`` in a well-known place):

``gcc your-test.c -oyour-test -lcutest``

Being on ``Linux`` is also necessary use the additional link-flag ``-ldl``.

On ``FreeBSD``, ``NetBSD`` and ``MINIX`` you need to use the additional link-flag ``-lexecinfo``.

If you are on ``Solaris`` you need to define the macro ``NO_CUTEST_BACKTRACING``. The related ``GCC`` compiler flag
is ``-DNO_CUTEST_BACKTRACING``.

## Dumping test log to a file

To do it use the option ``--cutest-log-path=<filepath>``. This option specifies a file path where the test log will be dumped.

## Reading command line options from your test cases

Sometimes is necessary... So to do it you should use the macro ``CUTE_GET_OPTION(<option-name>)``. Supposing that you want to read the option ``--foobar`` try this:

```c
        char *foobar_value = CUTE_GET_OPTION("foobar");
```

Note that the options should be passed in this form to your test binary:

``somewhere/over/the/rainbow/your-test --foobar=option-with-content --flag-option``

When you try to read an unknown option the return is always ``NULL`` and ``flag options`` always return ``"1"``.

## Detecting memory leaks

Firstly be aware that the [``valgrind``](http://valgrind.org) usage is the best way to catch this kind of [raptors](https://xkcd.com/292/) inside your code. However ``cutest`` brings a minimal system that performs memory leak detection. It could be a profitable way to detect this kind of issue as soon as possible. Because you will be looking for memory leaks being still on test phase.

The usage of this system must be flagged passing the option ``--cutest-leak-check`` to your unit test binary. Something like:

``something/leaking/over/the/rainbow/your-test --cutest-leak-check``

When some memory leak is detected a report about this issue is included at the end of your general test report and the test binary exits with a non-zero ``exit code``.

The format of this memory leak report is as follows:

        cutest INTERNAL ERROR: Memory leak(s) detected!!

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

The allocation id can be used in order to force a debug break. When you pass to your test binary the option ``--cutest-leak-id=<allocation-id>`` a ``trap`` signal will be raised at the moment that the allocation id be equals to passed value. Being the debugger attached you will be able to inspect the ``callstack`` and then know more about this "superunknown" leak.

## Log customizing

It is possible to custom the test logs. ``Cutest`` divide the log in three parts which are: ``header``, ``detail`` and ``footer``.

There are three options that you need to pass to your test binary if you intend to modify the log layout. Table 2 summarizes
these options.

**Table 2**: The ``test-log`` options.

|          **Option**     |               **Receives**                  |
|:-----------------------:|--------------------------------------------:|
| ``--cutest-log-header`` | a file path pointing to the header template |
| ``--cutest-log-detail`` | a file path pointing to the detail template |
| ``--cutest-log-footer`` | a file path pointing to the footer template |

Into the template goes the specific tokens related with the chosen format well as the variables that belongs relevant infos about the logged process.

Table 3 brings a listing of these variables and what they represent.

**Table 3**: The ``test-log`` recognized variables.

|        **Variable**        |          **Represents**                    |
|:--------------------------:|-------------------------------------------:|
|        ``$FILE``           | the test file path                         |
|        ``$LINE``           | the test file line number                  |
|       ``$STATUS``          | the test status (``passed`` or ``failed``) |
|     ``$CASE_NAME``         | the test name                              |
|    ``$RAN_TEST_NR``        | the ran tests amount                       |
| ``$ASSERTION_MESSAGE``     | the user defined assertion message         |

Following you can see test template samples.

Here goes the log ``header``:

```html
        <!-- test-log-header.html -->
        <title>Log template sample</title>
        <h1>Unit test results</h1>
        <table>
            <tr><td><b>Test name</b></td><td><b>Result</b></td><td><b>Message</b></td></tr>
```


now the ``detail``...

```html
        <!-- test-log-detail.html -->
        <tr><td>$CASE_NAME in $FILE at $LINE</td><td>$STATUS</td><td>$ASSERTION_MESSAGE</td></tr>
```

and then the ``footer``.

```html
            <!-- test-log-footer -->
            </table>
            <b>Total tests executed</b>: $RAN_TESTS_NR
        </html>
```

To use these templates you must indicate them by command line in this following way:

``somewhere/over/the/rainbow/your-test --cutest-log-header=templates/test-log-header.html --cutest-log-detail=templates/test-log-detail.html --cutest-log-footer=templates/test-log-footer.html``

**Remark**: Under ``unix-like`` platforms we call ``setbuf()`` in order to set ``stdout`` and ``stderr`` to ``NULL`` when we
are logging the test's output to a file. It is done for getting rid of the initial 4k allocation done by the ``stdio`` and
never freed (for performance issues). When not skipped it can add some noise to your memory leak's reports.

### Customizing the memory leak report

It is possible too. The Table 4 brings all variables recognized by this kind of template.

**Table 4**: The ``leak-log`` recognized variables.

|          **Variable**             |                     **Represents**                              |
|:---------------------------------:|----------------------------------------------------------------:|
|          ``$LEAK_ID``             | the leak id                                                     |
|         ``$LEAK_ADDR``            | the start leak memory address                                   |
|       ``$LEAK_FILE_PATH``         | the file path where the leak is occurring                       |
|         ``$LEAK_LINE``            | the file line number of the last test assertion before the leak |
|         ``$LEAK_DATA``            | the data which is leaking                                       |
|         ``$LEAK_SIZE``            | the size of leak                                                |
|         ``$LEAK_SUM``             | the total (in bytes) that are leaking                           |

Similar the test log custom you need to pass the options specifying the file path of ``header``, ``detail`` and ``footer``.
These options are respectively: ``--cutest-leak-log-header``, ``--cutest-leak-log-detail`` and ``--cutest-leak-log-footer``.

