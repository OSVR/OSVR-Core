find_package(libfunctionality)

include("${CMAKE_CURRENT_LIST_DIR}/ogvrTargets.cmake")

function(ogvr_add_plugin NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} ogvr::ogvrPluginKit)
    set_target_properties(${NAME} PROPERTIES PREFIX "")
endfunction()
