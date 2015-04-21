#!/bin/sh
BUILDDIR=$(cd $1 && pwd)
IWYUTOOL=iwyu_tool
DEVTOOLS=$(cd $(dirname $0) && pwd)
LIBS=$(cd $(dirname $0)
    cd ../inc/osvr
    for lib in *; do
        echo $lib
    done)
for lib in $LIBS; do
(
    cd $DEVTOOLS/..
    $IWYUTOOL -p $BUILDDIR src/osvr/$lib/*.cpp src/osvr/$lib/*.h inc/osvr/$lib/*.h -- --check_also=*.h --mapping_file=$DEVTOOLS/iwyu.imp 2>&1 |tee iwyu-$lib.txt
)
done