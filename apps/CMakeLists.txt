
add_executable(BasicPluginLoader BasicPluginLoader.cpp)
target_link_libraries(BasicPluginLoader ogvrPluginHost)

add_executable(BasicPluginLoaderWithPoll BasicPluginLoaderWithPoll.cpp)
target_link_libraries(BasicPluginLoaderWithPoll ogvrPluginHost)

if(BUILD_TESTING)
    foreach(HOST BasicPluginLoader BasicPluginLoaderWithPoll)
        foreach(PLUGIN DummyAsync DummyHardwarePoll DummySync NullDevice NullPlugin)
            add_test(NAME "run_${HOST}_${PLUGIN}"
                COMMAND ${HOST} org_opengoggles_example_${PLUGIN})
        endforeach()
    endforeach()
endif()
