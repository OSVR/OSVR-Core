# These plugins can run on just a bare PluginHost.
set(OSVR_TEST_PLUGINS
    org_opengoggles_example_DummyHardwareDetect
    org_opengoggles_example_DummyHardwareDetectCpp
    org_opengoggles_example_DummyHardwareDetectCpp2
    org_opengoggles_example_NullDevice
    org_opengoggles_example_NullPlugin)

# These plugins need a host with a Connection.
set(OSVR_TEST_DEVICE_PLUGINS
    org_opengoggles_example_DummyAsync
    org_opengoggles_example_DummySync)

# Build the test-only plugins
foreach(pluginname ${OSVR_TEST_PLUGINS} ${OSVR_TEST_DEVICE_PLUGINS})
    osvr_add_plugin_no_install(${pluginname} ${pluginname}.cpp)
    target_link_libraries(${pluginname} osvrPluginKitCpp)
    set_target_properties(${pluginname} PROPERTIES
        FOLDER "OSVR Test Plugins")
endforeach()

# Add tests for the plugins that require just a PluginHost
foreach(HOST BasicPluginLoader BasicPluginLoaderWithDetect ConnectionHost)
    foreach(PLUGIN ${OSVR_TEST_PLUGINS})
        add_test(NAME "run_${HOST}_${PLUGIN}"
            COMMAND ${HOST} ${PLUGIN})
    endforeach()
endforeach()

# Add tests for the plugins that require a Connection too
set(HOST ConnectionHost)
foreach(PLUGIN ${OSVR_EXAMPLE_DEVICE_PLUGINS} ${OSVR_TEST_DEVICE_PLUGINS})
    add_test(NAME "run_${HOST}_${PLUGIN}"
        COMMAND ${HOST} ${PLUGIN})
endforeach()
