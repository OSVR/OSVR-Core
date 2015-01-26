
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
