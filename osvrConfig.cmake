find_package(libfunctionality)

include("${CMAKE_CURRENT_LIST_DIR}/osvrTargets.cmake")

function(osvr_add_plugin NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} osvr::osvrPluginKit)
    set_target_properties(${NAME} PROPERTIES PREFIX "")
endfunction()
