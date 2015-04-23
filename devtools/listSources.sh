#!/bin/sh -e

# Makes a list of source files to paste into a CMakeLists.txt file
# Uses the current directory, or the subdir name (like Util) if passed.

(
  if [ $# -eq 1 ]; then
    cd $(dirname $0) && cd .. && cd src/osvr/$1
  fi
  find * \( -name "*.h" -o -name "*.cpp" -o -name "*.h.*" \) -print |sort |sed -e 's:^:    :'
)
