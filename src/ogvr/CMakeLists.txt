#set(HEADER_LOCATION "${HEADER_BASE}/ogvr")
include(GenerateExportHeader)

set(LIB_FOLDER "OGVR Libraries")
macro(ogvr_setup_lib_vars _lib)
    set(LIBNAME ${_lib})
    string(TOUPPER ${LIBNAME} LIBNAME_CAPS)
    set(LIBNAME_FULL "ogvr${LIBNAME}")
    set(HEADER_LOCATION "${HEADER_BASE}/ogvr/${LIBNAME}")
    set(EXPORT_BASENAME "OGVR_${LIBNAME_CAPS}")
endmacro()

add_subdirectory(Util)
add_subdirectory(PluginHost)
add_subdirectory(VRPN)
add_subdirectory(Connection)
add_subdirectory(PluginKit)
add_subdirectory(Server)


add_subdirectory(Client)
add_subdirectory(ClientKit)
