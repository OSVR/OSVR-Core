###
# NOTE: To configure the build, don't edit this file - use CMake to set the options (in the GUI or on the command line)
# These are just defaults, and are split into a separate file for easier forwarding from superbuilds.
###

###
# Configuration Options
###

# List of options, for easier consumption by superbuilds. Leaving off the BUILD_ prefix
set(OSVR_BUILD_OPTIONS SERVER SERVER_EXAMPLES SERVER_APP SERVER_PLUGINS CLIENT CLIENT_EXAMPLES CLIENT_APPS)

option(BUILD_SHARED_LIBS "Should we build as a shared library?" ON)
option(BUILD_DEV_VERBOSE "Build with verbose development messages?" ON)

option(BUILD_SERVER "Should we build the server libraries and required dev tools?" ON)
option(BUILD_SERVER_EXAMPLES "Should we build the server-related example apps and plugins?" ON)
option(BUILD_SERVER_APP "Should we build the server app?" ON)
option(BUILD_SERVER_PLUGINS "Should we build the standard plugins?" ON)

option(BUILD_CLIENT "Should we build the client libraries?" ON)
option(BUILD_CLIENT_EXAMPLES "Should we build the client example apps?" ON)
option(BUILD_CLIENT_APPS "Should we build the client apps?" ON)

option(BUILD_HEADER_DEPENDENCY_TESTS "Should we add targets to ensure that every public header compiles cleanly on its own? Increases number of targets greatly..." ON)

if(NOT BUILD_CLIENT AND NOT BUILD_SERVER)
    message(FATAL_ERROR "Must have at least one of BUILD_CLIENT and BUILD_SERVER enabled!")
endif()

# Consistency: can't build the examples, apps, etc if you don't have the library
set(dependent_options
    BUILD_SERVER_EXAMPLES
    BUILD_SERVER_APP
    BUILD_SERVER_PLUGINS
    BUILD_CLIENT_EXAMPLES
    BUILD_CLIENT_APPS)
foreach(dependent_option ${dependent_options})
    string(REGEX REPLACE "_[^_]+$" "" dependency "${dependent_option}")
    if("${${dependent_option}}" AND NOT "${${dependency}}")
        # Commented-out code updates the cache variable.
        #get_property(_help CACHE ${dependent_option} PROPERTY HELPSTRING)
        #set(${dependent_option} OFF CACHE BOOL "${_help}" FORCE)
        set(${dependent_option} OFF)
        message(STATUS "Disabling ${dependent_option}: depends on ${dependency}, which is disabled")
    endif()
endforeach()

