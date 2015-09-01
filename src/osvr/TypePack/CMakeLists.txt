osvr_setup_lib_vars(TypePack)

set(API
    "${HEADER_LOCATION}/TypePack.h"
    "${HEADER_LOCATION}/TypePackRuntime.h")

###
# Add the dummy target to make the headers show up in IDEs
add_custom_target(${LIBNAME_FULL}_Headers SOURCES ${API})
set_target_properties(${LIBNAME_FULL}_Headers PROPERTIES
    FOLDER "${LIB_FOLDER}")

###
# Add the interface target to actually use the files.
add_library(${LIBNAME_FULL} INTERFACE)
target_include_directories(${LIBNAME_FULL}
    INTERFACE
    #$<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
    $<BUILD_INTERFACE:${HEADER_BASE}>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${LIBNAME_FULL}
    INTERFACE
    $<BUILD_INTERFACE:osvr_cxx11_flags>)

osvr_append_target(BUILDTREE ${LIBNAME_FULL})

###
# Install things properly.
install(TARGETS ${LIBNAME_FULL}
    EXPORT osvrTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${API}
    DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/osvr/${LIBNAME}
    COMPONENT Devel)
