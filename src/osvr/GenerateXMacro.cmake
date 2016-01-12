set(OSVR_X_MACRO_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/GenerateXMacroScript.cmake" CACHE INTERNAL "" FORCE)
set(OSVR_X_MACRO_TEMPLATE "${CMAKE_CURRENT_LIST_DIR}/XMacro.h.in" CACHE INTERNAL "" FORCE)
include(CMakeParseArguments)

# Generate an x-macro header as a custom command: the output must be listed as an input to a target.
# Args:
#  OUTPUT complete_path_to_outfilename (required)
#  INVOCATION_NAME invocation_macro_name (required)
#  ELEMENTS the elements of the list to generate the X macro for.
#  DEPENDS optional depends to forward to add_custom_command
function(osvr_generate_x_macro)
    set(__func__ osvr_generate_x_macro)
    cmake_parse_arguments(XMACRO "" "OUTPUT;INVOCATION_NAME" "DEPENDS;ELEMENTS" ${ARGN})
    foreach(required OUTPUT INVOCATION_NAME ELEMENTS)
        if(NOT XMACRO_${required})
            message(FATAL_ERROR "${__func__} requires the ${required} argument to be passed!")
        endif()
    endforeach()
    if(NOT IS_ABSOLUTE "${XMACRO_OUTPUT}")
        message(FATAL_ERROR "${__func__} requires the OUTPUT argument to be an absolute path!")
    endif()
    if(XMACRO_DEPENDS)
        set(XMACRO_DEPENDS DEPENDS ${XMACRO_DEPENDS})
    endif()
    add_custom_command(OUTPUT "${XMACRO_OUTPUT}"
        COMMAND
        "${CMAKE_COMMAND}"
        "-DOUTFILE=${XMACRO_OUTPUT}"
        "-DINFILE=${OSVR_X_MACRO_TEMPLATE}"
        "-DOSVR_XMACRO_INVOKE=${XMACRO_INVOCATION_NAME}"
        "-DELEMENTS=${XMACRO_ELEMENTS}"
        -P "${OSVR_X_MACRO_SCRIPT}"
        # optional dependencies
        ${XMACRO_DEPENDS}
        VERBATIM
        COMMENT "Generating X-Macro file ${XMACRO_OUTPUT}")
endfunction()

include(GenerateXMacroFunction.cmake)

# Generate an x-macro header at cmake/configure time.
# Args:
#  OUTPUT complete_path_to_outfilename (required)
#  INVOCATION_NAME invocation_macro_name (required)
#  ELEMENTS the elements of the list to generate the X macro for.
function(osvr_generate_x_macro_now)
    set(__func__ osvr_generate_x_macro_now)
    cmake_parse_arguments(XMACRO "" "OUTPUT;INVOCATION_NAME" "ELEMENTS" ${ARGN})
    foreach(required OUTPUT INVOCATION_NAME ELEMENTS)
        if(NOT XMACRO_${required})
            message(FATAL_ERROR "${__func__} requires the ${required} argument to be passed!")
        endif()
    endforeach()
    if(NOT IS_ABSOLUTE "${XMACRO_OUTPUT}")
        message(FATAL_ERROR "${__func__} requires the OUTPUT argument to be an absolute path!")
    endif()
    message(STATUS "Generating ${XMACRO_OUTPUT}")
    __osvr_generate_xmacro_contents_now("${OSVR_X_MACRO_TEMPLATE}"
        "${XMACRO_OUTPUT}"
        "${XMACRO_INVOCATION_NAME}"
        ${XMACRO_ELEMENTS})
endfunction()
