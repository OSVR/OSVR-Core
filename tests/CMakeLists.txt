add_subdirectory(cplusplus)

if(BUILD_SERVER_EXAMPLES)
    add_subdirectory(plugins)
endif()

if(BUILD_HEADER_DEPENDENCY_TESTS)
    add_subdirectory(header_dependencies)
endif()
