include(GenerateExportHeader)

set(LIB_FOLDER "OGVR Libraries")

## Based on a library name without the ogvr prefix, sets the following
## variables:
##  - LIBNAME - the input name
##  - LIBNAME_CAPS
##  - LIBNAME_FULL - the input prefixed with ogvr
##  - HEADER_LOCATION - the parallel header directory
##  - EXPORT_BASENAME - The base of macros defined by Export.h
macro(ogvr_setup_lib_vars _lib)
    set(LIBNAME ${_lib})
    string(TOUPPER ${LIBNAME} LIBNAME_CAPS)
    set(LIBNAME_FULL "ogvr${LIBNAME}")
    set(HEADER_LOCATION "${HEADER_BASE}/ogvr/${LIBNAME}")
    set(EXPORT_BASENAME "OGVR_${LIBNAME_CAPS}")
endmacro()

## Adds a library, setting up the folder, export header, static define,
## basic include directories, basic private compile options, and installation.
##
## Requires a previous call to ogvr_setup_lib_vars
## and definition of API and SOURCE variables.
macro(ogvr_add_library)
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

    target_compile_options(${LIBNAME_FULL}
        PRIVATE
        ${OGVR_CXX11_FLAGS})

    enable_extra_compiler_warnings(${LIBNAME_FULL})

    ###
    # Install library and headers
    ###
    install(TARGETS ${LIBNAME_FULL}
        EXPORT ogvrTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(FILES ${API}
        DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/ogvr/${LIBNAME}
        COMPONENT Devel)
endmacro()

add_subdirectory(Util)
add_subdirectory(PluginHost)
add_subdirectory(VRPN)
add_subdirectory(Connection)
add_subdirectory(PluginKit)
add_subdirectory(Server)


add_subdirectory(Client)
add_subdirectory(ClientKit)
