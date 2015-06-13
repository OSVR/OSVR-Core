# Call with a desired variable name, a JSON input file (relative to the
# current source dir), and a desired output file path (generally in
# CMAKE_CURRENT_BINARY_DIR). Add that output file path to some target,
# and it will get generated, by the osvr_json_to_c tool, containing a
# literal (appropriately escaped) of your JSON file in compact format
# (whitespace and comments removed), not null terminated.
function(osvr_convert_json _symbol _in _out)
    if(NOT OSVR_JSON_TO_C_EXECUTABLE)
        message(FATAL_ERROR "Cannot convert a JSON file to C without osvr_json_to_c - set OSVR_JSON_TO_C_EXECUTABLE to a valid host system executable!")
    endif()
    add_custom_command(OUTPUT "${_out}"
        COMMAND ${OSVR_JSON_TO_C_EXECUTABLE} --symbol ${_symbol} "${_in}" "${_out}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_in}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "[osvr_convert_json] Generating string literal header from ${_in}"
        VERBATIM)
endfunction()
