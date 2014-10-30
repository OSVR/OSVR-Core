
foreach(HOST BasicPluginLoader BasicPluginLoaderWithPoll)
    add_executable(${HOST} ${HOST}.cpp)
    target_link_libraries(${HOST} ogvrPluginHost)

    if(BUILD_TESTING)
        foreach(PLUGIN DummyAsync DummyHardwarePoll DummySync NullDevice NullPlugin)
            add_test(NAME "run_${HOST}_${PLUGIN}"
                COMMAND ${HOST} org_opengoggles_example_${PLUGIN})
        endforeach()
    endif()
endforeach()
