if(BUILD_SERVER_EXAMPLES)
    add_subdirectory(plugin)
    add_subdirectory(plugin-hosts)
    add_subdirectory(internals)
endif()

if(BUILD_CLIENT_EXAMPLES)
    add_subdirectory(clients)
endif()
