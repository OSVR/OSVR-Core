#!/bin/sh

# Makes a list of source files to paste into a CMakeLists.txt file

find ${1-}* -name "*.h" -o -name "*.cpp" -o -name "*.h.in" |sort |sed -e 's:^:    :'