
foreach(HOST BasicPluginLoader BasicPluginLoaderWithPoll ConnectionHost)
    add_executable(${HOST} ${HOST}.cpp)
    target_link_libraries(${HOST} ogvrPluginHost)
    set_target_properties(${HOST} PROPERTIES
        FOLDER "OGVR Example Plugin Hosts")
    if(BUILD_TESTING)
        foreach(PLUGIN DummyHardwarePoll NullDevice NullPlugin)
            add_test(NAME "run_${HOST}_${PLUGIN}"
                COMMAND ${HOST} org_opengoggles_example_${PLUGIN})
        endforeach()
    endif()
endforeach()

# These require the connection host.
set(HOST ConnectionHost)
if(BUILD_TESTING)
    target_link_libraries(${HOST} ogvrConnection)
    foreach(PLUGIN DummyAsync DummySync)
        add_test(NAME "run_${HOST}_${PLUGIN}"
            COMMAND ${HOST} org_opengoggles_example_${PLUGIN})
    endforeach()
endif()
