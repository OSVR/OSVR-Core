
# File will only exist in build trees
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigBuildTreePaths.cmake" OPTIONAL)

find_package(libfunctionality)

include("${CMAKE_CURRENT_LIST_DIR}/osvrTargets.cmake")

# File will only exist in install trees.
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigInstalledBoost.cmake" OPTIONAL)

function(osvr_add_plugin NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} osvr::osvrPluginKit)
    set_target_properties(${NAME} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/@PLUGINDIR@"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/@PLUGINDIR@")
    install(TARGETS ${NAME}
        LIBRARY DESTINATION @PLUGINDIR@ COMPONENT Runtime)
endfunction()

function(osvr_convert_json _symbol _in _out)
    add_custom_command(OUTPUT "${_out}"
        COMMAND osvr::osvr_json_to_c --symbol ${_symbol} "${_in}" "${_out}"
        MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_in}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Generating string literal header from ${_in}"
        VERBATIM)
endfunction()
