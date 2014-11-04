#!/bin/sh

# Makes a list of headers to paste into a CMakeLists.txt file

ls ${1}*.h |sort |sed -e 's:^:    "${HEADER_LOCATION}/:' -e 's:$:":'