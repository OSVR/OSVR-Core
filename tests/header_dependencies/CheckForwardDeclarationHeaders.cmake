#
# Ensure forward-declaration headers are included in the full-declaration headers.
# That is, ensure that Blah_fwd.h is included in Blah.h.
#
# Variables:
#
#   HEADER_BASE  Set this to the root directory containing the header files to be checked.
#

file(TO_CMAKE_PATH "${HEADER_BASE}" HEADER_BASE)
file(GLOB_RECURSE fwd_headers RELATIVE "${HEADER_BASE}" ${HEADER_BASE}/*_fwd.h)
foreach(fwd_header ${fwd_headers})
    string(REPLACE "_fwd.h" ".h" full_header "${fwd_header}")
    file(STRINGS "${HEADER_BASE}/${full_header}" _found REGEX "#include [<\"]${fwd_header}[>\"]")
    if (NOT _found)
        message(FATAL_ERROR "The header file <${full_header}> does not include <${fwd_header}>.")
    endif()
endforeach()

