# condition-cpp [![Build Status](https://travis-ci.com/musteresel/condition-cpp.svg?branch=master)](https://travis-ci.com/musteresel/condition-cpp)

*A lightweight condition system for C++, inspired by Common Lisp.*

## What is a condition system?

Think exceptions just better :) In a condition system low level code
can *signal* a condition which is then *handled* (= deciding *what* to
do) by high level code.  This transfers control "back down" to mid
level code to a restart (which knows *how* to do it, e.g. by calling
alternative low level code):

~~~
    high level decided WHAT
    to do, now let mid level
    actually do it!            /----> alternative low level
    |-------------|      /----/
    |             v     /
high level --> mid level --> low level
       ^                        |
       |------------------------|
          transfer control back
           up without bothering
            with the mid level
~~~

As such a condition system is a form of [structured non-local control
flow][non-local-c-flow] and provides more flexibility than exceptions:
High level code can basically determine "how far up the call stack" an
"exception is allowed to be thrown".  In the actual implementation it
looks more like this:

~~~
         --> Installs Handler                  --> Installs restart 2
        /             -> Installs restart 1   /
       /             /                       /
high level ------> mid level 1 ----> mid level 2 ---> low level
                        ^                  ^            |
                        |                  |            | signals a
                        |                  |            | condition
                        |                  |            |
                        |                  |            v
                        |         (either) |-----   handler decides
                        |-----------------------     which restart
                                                      to use
~~~

## How can I use this?

This is a header only library, so basically all you need is to grab
the [condition-cpp.hh](./condition-cpp.hh) file and include it in your project.  But
there's also a CMake based build system, which provides installable
exports so that you can use `find_package`:

~~~bash
# clone sources / get archive with sources and unpack

cmake -Bbuild -H. # plus options regarding tests:
#
#  -DCatch2_DIR=path_to_catch2_cmake_files
#
#  and additionally: -DSEARCH_CATCH=yes to search for Catch2 in
#  subdirectories of Catch2_DIR
#
# Or, if you don't want to run the tests: -DBUILD_TESTING=OFF

cmake --build build/
cmake --build build/ --target install # for install, or
cmake --build build/ --target package # to get a zip file
~~~

If you have Doxygen in your path you'll get html documentation and man
pages, too.

All in all this is pretty much an alpha state project, and things
could likely change.  If you have suggestions, then don't hesitate to
...

## Contribute

With ideas! Criticsm! Examples, Questions, Suggestions.

The best place for this are the [Github issues][gh-issues] for the
project.

[non-local-c-flow]: https://en.wikipedia.org/wiki/Control_flow#Structured_non-local_control_flow
[gh-issues]: https://github.com/musteresel/condition-cpp/issues
