
foreach(HOST BasicPluginLoader BasicPluginLoaderWithDetect ConnectionHost)
    add_executable(${HOST} ${HOST}.cpp)
    target_link_libraries(${HOST} osvrPluginHost)
    set_target_properties(${HOST} PROPERTIES
        FOLDER "OSVR Example Plugin Hosts")
    if(BUILD_TESTING)
        foreach(PLUGIN ${OSVR_EXAMPLE_PLUGINS})
            add_test(NAME "run_${HOST}_${PLUGIN}"
                COMMAND ${HOST} org_opengoggles_example_${PLUGIN})
        endforeach()
    endif()
endforeach()

# These require the connection host.
set(HOST ConnectionHost)
if(BUILD_TESTING)
    target_link_libraries(${HOST} osvrConnection)
    foreach(PLUGIN ${OSVR_EXAMPLE_DEVICE_PLUGINS})
        add_test(NAME "run_${HOST}_${PLUGIN}"
            COMMAND ${HOST} org_opengoggles_example_${PLUGIN})
    endforeach()
endif()
