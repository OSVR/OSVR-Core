set(LIB_FOLDER "OSVR Libraries")

set(OSVR_BUILDTREE_TARGETS "" CACHE INTERNAL "" FORCE)
macro(osvr_append_target _targetlist _target)
    list(APPEND OSVR_${_targetlist}_TARGETS "${_target}")
    set(OSVR_${_targetlist}_TARGETS "${OSVR_${_targetlist}_TARGETS}" CACHE INTERNAL "" FORCE)
endmacro()

## Based on a library name without the osvr prefix, sets the following
## variables:
##  - LIBNAME - the input name
##  - LIBNAME_CAPS
##  - LIBNAME_FULL - the input prefixed with osvr
##  - LIBNAME_FULL_CPP - The name of the C++ interface library if applicable
##  - HEADER_LOCATION - the parallel header directory
##  - EXPORT_BASENAME - The base of macros defined by Export.h
macro(osvr_setup_lib_vars _lib)
    set(LIBNAME ${_lib})
    string(TOUPPER ${LIBNAME} LIBNAME_CAPS)
    set(LIBNAME_FULL "osvr${LIBNAME}")
    set(HEADER_LOCATION "${HEADER_BASE}/osvr/${LIBNAME}")
    set(EXPORT_BASENAME "OSVR_${LIBNAME_CAPS}")
endmacro()

## Adds a library, setting up the folder, export header, static define,
## basic include directories, basic private compile options, and installation.
##
## Requires a previous call to osvr_setup_lib_vars
## and definition of API and SOURCE variables.
macro(osvr_add_library)
    list(APPEND API "${CMAKE_CURRENT_BINARY_DIR}/Export.h")
    source_group(API FILES ${API})
    add_library(${LIBNAME_FULL} ${API} ${SOURCE})

    set_target_properties(${LIBNAME_FULL} PROPERTIES
        FOLDER "${LIB_FOLDER}")

    generate_export_header(${LIBNAME_FULL}
        BASE_NAME ${EXPORT_BASENAME}
        EXPORT_FILE_NAME Export.h)

    if(NOT BUILD_SHARED_LIBS)
        target_compile_definitions(${LIBNAME_FULL} PUBLIC ${EXPORT_BASENAME}_STATIC_DEFINE)
    endif()

    target_include_directories(${LIBNAME_FULL}
        PUBLIC
        $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
        $<BUILD_INTERFACE:${HEADER_BASE}>
        $<INSTALL_INTERFACE:include>)

    target_link_libraries(${LIBNAME_FULL} PRIVATE osvr_cxx11_flags)
    enable_extra_compiler_warnings(${LIBNAME_FULL})

    ###
    # Install library and headers
    ###
    install(TARGETS ${LIBNAME_FULL}
        EXPORT osvrTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Devel
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(FILES ${API}
        DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/osvr/${LIBNAME}
        COMPONENT Devel)
    osvr_append_target(BUILDTREE ${LIBNAME_FULL})
endmacro()

# For libraries with an additional optional "interface library", this creates
# the appropriate target, specifies linking, and installs.
macro(osvr_add_interface_library _suffix)
    set(LIBNAME_INTERFACE ${LIBNAME_FULL}${_suffix})
    add_library(${LIBNAME_INTERFACE} INTERFACE)
    target_link_libraries(${LIBNAME_INTERFACE} INTERFACE ${LIBNAME_FULL})
    target_include_directories(${LIBNAME_INTERFACE}
        INTERFACE
        $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
        $<BUILD_INTERFACE:${HEADER_BASE}>
        $<INSTALL_INTERFACE:include>)
    install(TARGETS ${LIBNAME_INTERFACE}
        EXPORT osvrTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Devel
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    osvr_append_target(BUILDTREE ${LIBNAME_INTERFACE})
endmacro()

## For libraries with a C++ interface "library", this creates the INTERFACE
## target, links it to the main library, and installs the target
## appropriately for CMake.
macro(osvr_add_cpp_interface_library)
    osvr_add_interface_library(Cpp)
endmacro()

## Copy and install shared libraries from imported targets as required
function(osvr_copy_dep _target _dep)
    copy_imported_targets(${_target} ${_dep})
    install_imported_target(${_dep} DESTINATION ${OSVR_SHARED_LIBRARY_DIR} COMPONENT Runtime)
endfunction()
