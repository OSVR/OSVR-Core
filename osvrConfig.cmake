find_package(libfunctionality)
find_package(Boost QUIET)

include("${CMAKE_CURRENT_LIST_DIR}/osvrTargets.cmake")
set_property(TARGET osvr::osvrUtilCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET osvr::osvrClientKitCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})
set_property(TARGET osvr::osvrPluginKitCpp APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIRS})

function(osvr_add_plugin NAME)
    add_library(${NAME} MODULE ${ARGN})
    target_link_libraries(${NAME} osvr::osvrPluginKit)
    set_target_properties(${NAME} PROPERTIES PREFIX "")
endfunction()
