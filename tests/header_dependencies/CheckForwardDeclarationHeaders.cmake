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
    if (EXISTS "${HEADER_BASE}/${full_header}")
        file(STRINGS "${HEADER_BASE}/${full_header}" _found REGEX "#include [<\"]${fwd_header}[>\"]")
        if (NOT _found)
            message(FATAL_ERROR "The header file ${HEADER_BASE}/${full_header} does not include <${fwd_header}>.")
        endif()
    endif()
endforeach()

#
# Ensure that smart pointer typedef headers are inclucded in the full-declaration headers.
# That is, ensure that BlahPtr.h is included in Blah.h
#
# Variables:
#
#   HEADER_BASE  Set this to the root directory containing the header files to be checked.
#

file(GLOB_RECURSE ptr_headers RELATIVE "${HEADER_BASE}" ${HEADER_BASE}/*Ptr.h)
foreach(ptr_header ${ptr_headers})
    string(REPLACE "Ptr.h" ".h" full_header "${ptr_header}")
    if (EXISTS "${HEADER_BASE}/${full_header}")
        file(STRINGS "${HEADER_BASE}/${full_header}" _found REGEX "#include [<\"]${ptr_header}[>\"]")
        if (NOT _found)
            message(FATAL_ERROR "The header file ${HEADER_BASE}/${full_header} does not include <${ptr_header}>.")
        endif()
    endif()
endforeach()


