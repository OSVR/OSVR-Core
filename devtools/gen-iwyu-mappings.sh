#!/bin/sh

(
    cd $(dirname $0)
    cd ../inc
    echo "["
    find osvr -name "*.h" | while read header; do
        echo "{ \"include\": [\"${header}\", \"private\", \"<${header}>\", \"public\"] },"
    done
    echo "]"
) > $(cd $(dirname $0) && pwd)/anglebrackets.imp