# Shared implementation between build-time and config-time x-macro generation.
function(__osvr_generate_xmacro_contents_now INFILE OUTFILE INVOKE)
    set(OSVR_XMACRO_CONTENTS)
    foreach(elt ${ARGN})
        set(OSVR_XMACRO_CONTENTS "${OSVR_XMACRO_CONTENTS} \\\nOSVR_X(${elt})")
    endforeach()
    set(OSVR_XMACRO_INVOKE ${INVOKE})
    configure_file("${INFILE}" "${OUTFILE}" @ONLY NEWLINE_STYLE LF)
endfunction()
