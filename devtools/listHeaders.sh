#!/bin/sh

# Makes a list of headers to paste into a CMakeLists.txt file
# Uses the current directory, or the subdir name (like Util) if passed.
(
  if [ $# -eq 1 ]; then
    cd $(dirname $0) && cd .. && cd inc/osvr/$1
  fi
  ls *.h |sort |sed -e 's:^:    "${HEADER_LOCATION}/:' -e 's:$:":'
)
