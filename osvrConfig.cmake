find_package(libfunctionality)
find_package(Boost QUIET)

include("${CMAKE_CURRENT_LIST_DIR}/osvrTargets.cmake")
target_link_libraries(osvr::osvrUtilCpp INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(osvr::osvrClientKitCpp INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(osvr::osvrPluginKitCpp INTERFACE ${Boost_INCLUDE_DIRS})

function(osvr_add_plugin NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} osvr::osvrPluginKit)
    set_target_properties(${NAME} PROPERTIES PREFIX "")
endfunction()
