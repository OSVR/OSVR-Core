
# File will only exist in build trees - provides hints to dependencies
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigBuildTreePaths.cmake" OPTIONAL)

# Hook for a super-build to optionally inject hints before target import.
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigSuperBuildPrefix.cmake" OPTIONAL)

# Dependency of PluginKit
find_package(libfunctionality QUIET)

# The actual exported targets
include("${CMAKE_CURRENT_LIST_DIR}/osvrTargets.cmake")

# Fix up imported targets to add deps: these files will only exist in install trees.
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigInstalledBoost.cmake" OPTIONAL)
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigInstalledOpenCV.cmake" OPTIONAL)

# We set these variables so that we can share functions like osvr_add_plugin
# between builds in the main source tree and external builds using this config file.
set(OSVR_CACHED_PLUGIN_DIR "@OSVR_PLUGIN_DIR@" CACHE INTERNAL
    "The OSVR_PLUGIN_DIR variable for OSVR, for use in building and installing plugins" FORCE)

set(OSVR_PLUGIN_IGNORE_SUFFIX "@OSVR_PLUGIN_IGNORE_SUFFIX@" CACHE INTERNAL
    "The additional suffix for OSVR plugins that are not to be auto-loaded" FORCE)

set(OSVR_CACHED_CONFIG_ROOT "@OSVR_CONFIG_ROOT@" CACHE INTERNAL
    "The OSVR_CONFIG_ROOT variable for OSVR, for use in installing plugins' sample configs, display descriptors, etc." FORCE)

# Hook for a super-build to optionally inject configuration after target import.
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigSuperBuildSuffix.cmake" OPTIONAL)

# Since alias targets only work for libraries, we use this method instead to
# share the osvr_convert_json script between the main tree and external config users.
if(TARGET osvr::osvr_json_to_c)
    set(OSVR_JSON_TO_C_EXECUTABLE "osvr::osvr_json_to_c" CACHE INTERNAL
        "The target name for the osvr_json_to_c executable" FORCE)
else()
    # This stuff to handle primarily cross-compiling, like for Android.
    if(NOT DEFINED OSVR_JSON_TO_C_EXECUTABLE)
        # The latter variable might be set by a script in the build tree config,
        # or by a command line.
        set(OSVR_JSON_TO_C_EXECUTABLE "${OSVR_JSON_TO_C_COMMAND}")
    endif()
    if(COMMAND find_host_program)
        # If we are in an environment like android-cmake, use find_host_program
        # to find the program
        find_host_program(OSVR_JSON_TO_C_EXECUTABLE osvr_json_to_c)
    else()
        find_program(OSVR_JSON_TO_C_EXECUTABLE osvr_json_to_c)
    endif()
endif()
# The shared scripts
include("${CMAKE_CURRENT_LIST_DIR}/osvrAddPlugin.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/osvrConvertJson.cmake")
