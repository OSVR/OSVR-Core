
# File will only exist in build trees - provides hints to dependencies
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigBuildTreePaths.cmake" OPTIONAL)

# Dependency of PluginKit
find_package(libfunctionality QUIET)

# The actual exported targets
include("${CMAKE_CURRENT_LIST_DIR}/osvrTargets.cmake")

# Fix up imported targets to add deps: these files will only exist in install trees.
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigInstalledBoost.cmake" OPTIONAL)
include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigInstalledOpenCV.cmake" OPTIONAL)

# We set this variable so that we can share functions like osvr_add_plugin
# between builds in the main source tree and external builds using this config file.
set(OSVR_CACHED_PLUGIN_DIR "@OSVR_PLUGIN_DIR@" CACHE INTERNAL
    "The OSVR_PLUGIN_DIR variable for OSVR, for use in building and installing plugins" FORCE)

# Since alias targets only work for libraries, we use this method instead to
# share the osvr_convert_json script between the main tree and external config users.
set(OSVR_JSON_TO_C_EXECUTABLE "osvr::osvr_json_to_c" CACHE INTERNAL
    "The target name for the osvr_json_to_c executable" FORCE)

# The shared scripts
include("${CMAKE_CURRENT_LIST_DIR}/osvrAddPlugin.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/osvrConvertJson.cmake")