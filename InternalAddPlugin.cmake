# This file provides the effective equivalent of the osvr_add_plugin function
# provided by the CMake config file, but for use in tree. The difference is
# the addition of osvr_add_plugin_no_install for use in testing/example plugins.

function(osvr_add_plugin_no_install NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} osvrPluginKit)
    set_target_properties(${NAME} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OSVR_PLUGIN_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OSVR_PLUGIN_DIR}")
endfunction()

function(osvr_add_plugin NAME)
    osvr_add_plugin_no_install(${NAME} ${ARGN})
    install(TARGETS ${NAME}
        LIBRARY DESTINATION ${OSVR_PLUGIN_DIR} COMPONENT Runtime)
endfunction()