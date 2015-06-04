# Cutest

Cutest is an acronym that can be undestood as ``C`` ``u``nit ``te``st.

This library brings a bunch of macros in order to guide the implementation of unit tests on ``C`` projects.

It brings a minimal memory leak detection system (fully working on ``Linux`` and ``Windows``).

It is possible customize the logs generated by your tests if you want to.

## How to build it?

To build ``cutest`` the [hefesto](https://github.com/rafael-santiago/hefesto.git) usage is necessary. Being
``Hefesto`` installed in your system all you need to emit on a ``shell`` inside the cute's ``src`` subdirectory is:

``hefesto``

Note that the build was written based on ``GCC``. So, you need to have the ``gcc``/``mingw`` installed (and well exported)
on your system before going ahead.

After a file named ``libcutest.a`` which stands for the library will be generated under the path ``src/lib``.
Now just use this file and the header ``src/cutest.h`` to develop your further ``unit tests``.

Maybe you should read the [documentation](https://github.com/rafael-santiago/cutest/blob/master/doc/README.md) before start.
