# Call with a target name and one or more source files, and a target
# will be created that will compile the named plugin, set up correct filenames,
# and set up build and install directories. (Note that if you want to distribute
# additional files with your plugin, such as a README or a JSON file in plain text,
# you'll need to install those yourself using CMake commands.)
# If you are using the C++ wrapper, you'll also need to link to osvr::osvrPluginKitCpp
# to be sure to get the right includes.
function(osvr_add_plugin NAME)
    osvr_add_plugin_no_install(${NAME} ${ARGN})
    install(TARGETS ${NAME}
        LIBRARY DESTINATION ${OSVR_CACHED_PLUGIN_DIR} COMPONENT Runtime)
endfunction()

# Same as osvr_add_plugin, but doesn't install the plugin - used primarily
# in the source tree for test or example plugins.
function(osvr_add_plugin_no_install NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} osvr::osvrPluginKit)
    set_target_properties(${NAME} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OSVR_CACHED_PLUGIN_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OSVR_CACHED_PLUGIN_DIR}")
endfunction()
