find_package(libfunctionality)

include("${CMAKE_CURRENT_LIST_DIR}/osvrTargets.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/osvrConfigInstalledBoost.cmake" OPTIONAL)

function(osvr_add_plugin NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} osvr::osvrPluginKit)
    set_target_properties(${NAME} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/@OSVR_PLUGIN_DIR@"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/@OSVR_PLUGIN_DIR@")
    install(TARGETS ${NAME}
        LIBRARY DESTINATION @OSVR_PLUGIN_DIR@ COMPONENT Runtime)
endfunction()
