#!/usr/bin/env bash
# Handy ROOT aliases. Get them with "source root-aliases.sh"

# Since you're probably calling ROOT a thousand times a day, it could make sense, to shorten the call to the executable. Since "r" is free, I took this one.
alias r="root -l"
alias rb="r -b"
alias rq="r -b -q"

# Don't know why your ROOT macro is crashing? Run it through your compiler and get some proper warnings and errors
#  * --cxx will get the compile with which ROOT was compiled (usually g++, on OS X clang++)
#  * --glibs tells to the compiler the location and names of the libraries used
#  * --cflags lists the flags for the compiler, e.g. compile for C++11, but also ROOT's include dir
alias compileWithRoot="`root-config --cxx --glibs --cflags`"
