#!/bin/sh

runClangFormatOnDir() {
    find "$1" -name "*.c" -o -name "*.cpp" -o -name "*.c" -o -name "*.h" | while read fn; do
        echo "$fn"
        clang-format -style=file -i "$fn"
    done
}
(
cd $(dirname $0)
cd ..
runClangFormatOnDir apps
runClangFormatOnDir src
runClangFormatOnDir inc
runClangFormatOnDir tests
runClangFormatOnDir examples
runClangFormatOnDir plugins
runClangFormatOnDir devtools

#echo "Press enter to continue." && read
)
