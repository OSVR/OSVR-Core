include(CMakeParseArguments) # Bundled with CMake 2.8.3 and newer

# Call with a target name and one or more source files, and a target
# will be created that will compile the named plugin, set up correct filenames,
# and set up build and install directories. (Note that if you want to distribute
# additional files with your plugin, such as a README or a JSON file in plain text,
# you'll need to install those yourself using CMake commands.)
#
# Preferred usage:
#  osvr_add_plugin(NAME <pluginname> [NO_INSTALL] [MANUAL_LOAD] [CPP] SOURCES <source files...>) - where the flags indicate as follows:
#    - NO_INSTALL skips the install step,
#    - MANUAL_LOAD names the plugin output file so the auto-loader skips it (can still be loaded by name)
#    - CPP links with the osvr::osvrPluginKitCpp (C++ wrapper) instead of osvr::osvrPluginKit
#
function(osvr_add_plugin)
    cmake_parse_arguments(OSVR_ADD_PLUGIN "NO_INSTALL;MANUAL_LOAD;CPP" "NAME" "SOURCES" ${ARGN})
    # See if we got NAME, or if we have to figure them out from unparsed arguments.
    if(NOT OSVR_ADD_PLUGIN_NAME)
        list(GET OSVR_ADD_PLUGIN_UNPARSED_ARGUMENTS 0 OSVR_ADD_PLUGIN_NAME)
        list(REMOVE_AT OSVR_ADD_PLUGIN_UNPARSED_ARGUMENTS 0)
    endif()
    if(NOT OSVR_ADD_PLUGIN_SOURCES)
        set(OSVR_ADD_PLUGIN_SOURCES ${OSVR_ADD_PLUGIN_UNPARSED_ARGUMENTS})
    endif()
    
    if(ANDROID)
        # @TODO temporary workaround for Android plugins. Our toolchain file
        # seems to build Android MODULE libraries without a SONAME, but the SONAME
        # is required on the latest Android dynamic linker, so we build them as
        # SHARED for now.
        add_library(${OSVR_ADD_PLUGIN_NAME} SHARED ${OSVR_ADD_PLUGIN_SOURCES})
    else()
        add_library(${OSVR_ADD_PLUGIN_NAME} MODULE ${OSVR_ADD_PLUGIN_SOURCES})
    endif()

    if(OSVR_ADD_PLUGIN_CPP)
        target_link_libraries(${OSVR_ADD_PLUGIN_NAME} osvr::osvrPluginKitCpp)
    else()
        target_link_libraries(${OSVR_ADD_PLUGIN_NAME} osvr::osvrPluginKit)
    endif()

    if(ANDROID)
        set(OSVR_PLUGIN_PREFIX "lib")
        target_link_libraries(${OSVR_ADD_PLUGIN_NAME} "-z global")
    else()
        set(OSVR_PLUGIN_PREFIX "")
    endif()

    set_target_properties(${OSVR_ADD_PLUGIN_NAME} PROPERTIES
        PREFIX "${OSVR_PLUGIN_PREFIX}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OSVR_CACHED_PLUGIN_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OSVR_CACHED_PLUGIN_DIR}")
    if(OSVR_ADD_PLUGIN_MANUAL_LOAD)
        set_property(TARGET ${OSVR_ADD_PLUGIN_NAME} PROPERTY SUFFIX "${OSVR_PLUGIN_IGNORE_SUFFIX}${CMAKE_SHARED_MODULE_SUFFIX}")
    endif()
    if(MSVC)
        # Must distinguish plugins built against debug runtime
        set_property(TARGET ${OSVR_ADD_PLUGIN_NAME} PROPERTY DEBUG_POSTFIX ".debug")
    endif()
    if(NOT OSVR_ADD_PLUGIN_NO_INSTALL)
        install(TARGETS ${OSVR_ADD_PLUGIN_NAME}
            LIBRARY DESTINATION ${OSVR_CACHED_PLUGIN_DIR} COMPONENT Runtime)
    endif()
endfunction()
