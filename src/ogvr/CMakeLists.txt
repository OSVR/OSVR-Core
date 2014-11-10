#set(HEADER_LOCATION "${HEADER_BASE}/ogvr")
include(GenerateExportHeader)

add_subdirectory(Util)
add_subdirectory(PluginHost)
add_subdirectory(Connection)
add_subdirectory(PluginKit)
add_subdirectory(Server)


add_subdirectory(Client)
