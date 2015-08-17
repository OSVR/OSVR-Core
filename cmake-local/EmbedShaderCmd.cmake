# Input:
# SOURCE - full path to source file
# DEST - full path to destination file
# TEMPLATE - full path to the template file
# SYMBOL - name of variable to create
file(STRINGS "${SOURCE}" lines)

set(STRING)
foreach(LINE ${lines})
    string(CONFIGURE "\${LINE}" EscapedLine ESCAPE_QUOTES)
    string(CONCAT STRING "${STRING}" "\\\n\"${EscapedLine}\\n\"")
    #string(CONFIGURE "@STRING@\\\n\"@line@\\n\"" STRING @ONLY ESCAPE_QUOTES)
endforeach()

configure_file("${TEMPLATE}" "${DEST}" @ONLY NEWLINE_STYLE LF)
