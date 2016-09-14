# Cutest

Cutest is an acronym that can be undestood as ``C`` ``u``nit ``test``.

This library brings a bunch of macros in order to guide the implementation of unit tests for ``C`` projects.

``Cutest`` brings a minimal memory leak detection system (fully working on ``Linux``, ``Windows``, ``FreeBSD``, ``Solaris``, ``NetBSD``, ``MINIX`` and ``OpenBSD``).

All supported platforms for this library and other general features are listed on **Table 1**.

On ``Cutest`` is also possible customize the logs generated by your tests if you want to.

**Table 1**: Places where I have been running it. A.k.a. supported platforms. In additional, the compiler/linker options to be passed when using ``cutest`` as your unit testing library.

| **Operating System** | **Has *GNU* Backtracing?** |  **Thread safe?**    |   **Compiler/Linker flags to use (besides ``-lcutest``)** |
|:--------------------:|:--------------------------:|:--------------------:|:---------------------------------------------------------:|
|        ``Linux``     |         *Yes*              |        *Yes*         |                                 ``-ldl``, ``-lpthread``   |
|        ``FreeBSD``   |         *Yes*              |        *Yes*         |                           ``-lexecinfo``, ``-lpthread``   |
|        ``NetBSD``    |         *Yes*              |        *Yes*         |                           ``-lexecinfo``, ``-lpthread``   |
|        ``OpenBSD``   |         *Yes*              |        *Yes*         |                           ``-lexecinfo``, ``-lpthread``   |
|        ``MINIX``     |         *Yes*              |        *No*          |                                   ``-lexecinfo``          |
|        ``Solaris``   |         *No*               |        *Yes*         |                 ``-DNO_CUTEST_BACKTRACING``, ``-lpthread``|
|        ``Windows``   |         *Yes*              |        *Yes*         |                                                           |

Of course, that it should run in a bunch of ``UNIX`` boxes, however I prefer to list the places where I actually saw it running
and working pretty fine. If there is a specific non-listed platform that you want to use ``cutest``, let me know, maybe I
can adjust the library's build to accomplish your requirements ;)

## How to build it?

To build ``cutest`` the [hefesto](https://github.com/rafael-santiago/hefesto.git) usage is necessary. Being
``Hefesto`` installed in your system all you need to emit on a ``shell`` inside the cutest's ``src`` subdirectory is:

``hefesto``

After a file named ``libcutest.a`` which stands for the library will be generated under the path ``src/lib``.
Now just use this file and the header ``src/cutest.h`` to develop your further ``unit tests``.

Note that the build was written based on ``GCC``. So, you need to have the ``gcc``/``mingw`` installed (and well exported)
on your system before going ahead.

Maybe you should read the [documentation](https://github.com/rafael-santiago/cutest/blob/master/doc/README.md) before starting.

**Tip**: In case of trying to compile it under a ``UNIX`` environment with no support for ``PTHREADS`` before calling ``hefesto``
you need to edit the file ``src/.ivk`` and add the following ``C Macro`` definition ``-DHAS_NO_PTHREAD`` to ``--cflags`` option:

```
--cflags=-DHAS_NO_PTHREAD
```
### Note for Visual Studio users

If you want to build ``cutest`` under ``MSVC`` you need to use ``Forgefile-msvc.hsl`` instead of the default ``Forgefile.hsl`` script.
For this goal the build command changes a little. Being inside the cutest's ``src`` subdirectory:

``hefesto --forgefile=Forgefile-msvc.hsl --Forgefile-msvc-projects=cutest``

After run the command above a file named ``libcutest.lib`` will be generated inside ``src/lib`` path.
Note that use ``cutest`` on ``MSVC`` is a little bit tricky. It envolves composite the ``cutest`` with your current ``msvcrt.lib``
(it explains why the ``cutest.lib`` is so huge).

Another point is that this experimental build was written for ``Visual Studio 2012`` only.

If you want to generate a debug version try:

``hefesto --forgefile=Forgefile-msvc.hsl --Forgefile-msvc-projects=cutest --compile-model=debug``
