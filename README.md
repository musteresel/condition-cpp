# condition-cpp [![Build Status](https://travis-ci.com/musteresel/condition-cpp.svg?branch=master)](https://travis-ci.com/musteresel/condition-cpp)

*A lightweight condition system for C++, inspired by Common Lisp.*

## What is a condition system?

Think exceptions just better :) Exceptions provide a way to move
control from one point in the code - where the exception is thrown -
to another point int the code - where the exception is catched and
handled - via [structured non-local control flow][non-local-c-flow].
This allows "low level" code (deep in the call stack) to transfer
control directly up to "higher level" code - for example when some
error occurs.

~~~
high level --> mid level --> low level
  ^                              |
  |------------------------------|
       transfer control back up
         without bothering with
          the mid level code
~~~


This is a great improvement compared to e.g. moving an error code all
the way up through each and every "mid level" function:

~~~
high level --> mid level --> low level
    ^            |   ^           |
    |------------|   |-----------|
      mid level        error code
      needs to pass    needs to be
      the error up     passed up
~~~

A condition system provides even greater flexibility: Low level code
can *signal* a condition which is then *handled* (= deciding *what* to
do) by high level code.  This transfers control "back down" to mid
level code to a restart (which knows *how* to do it):

~~~
    high level decided WHAT
    to do, not let mid level
    actually do it!
    |-------------|
    |             v
high level --> mid level --> low level
       ^                        |
       |------------------------|
          transfer control back
           up without bothering
            with the mid level
~~~


[non-local-c-flow]: https://en.wikipedia.org/wiki/Control_flow#Structured_non-local_control_flow
