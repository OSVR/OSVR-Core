
foreach(HOST BasicPluginLoader BasicPluginLoaderWithDetect ConnectionHost)
    add_executable(${HOST} ${HOST}.cpp)
    target_link_libraries(${HOST} osvrPluginHost)
    set_target_properties(${HOST} PROPERTIES
        FOLDER "OSVR Example Plugin Hosts")
endforeach()

target_link_libraries(ConnectionHost osvrConnection)
