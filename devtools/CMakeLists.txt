
add_executable(osvr_json_to_c osvr_json_to_c.cpp)
set_target_properties(osvr_json_to_c PROPERTIES
    FOLDER "OSVR Stock Applications")
target_include_directories(osvr_json_to_c PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(osvr_json_to_c jsoncpp_lib ${Boost_PROGRAM_OPTIONS_LIBRARIES})
install(TARGETS osvr_json_to_c
    EXPORT osvrTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT DevTools)

function(osvr_convert_json _symbol _in _out)
    add_custom_command(OUTPUT "${_out}"
        COMMAND osvr_json_to_c --symbol ${_symbol} "${_in}" "${_out}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_in}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating string literal header from ${_in}"
        VERBATIM)
endfunction()