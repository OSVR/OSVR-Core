if(BUILD_SERVER_EXAMPLES)
    add_subdirectory(plugin)
    add_subdirectory(plugin-hosts)
endif()

if(BUILD_CLIENT_EXAMPLES)
    add_subdirectory(clients)
endif()
