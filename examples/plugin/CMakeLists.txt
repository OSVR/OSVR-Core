# These plugins can run on just a bare PluginHost.
set(OGVR_EXAMPLE_PLUGINS
    NullPlugin
    NullDevice
    DummyHardwareDetect
    DummyHardwareDetectCpp
    DummyHardwareDetectCpp2
    CACHE INTERNAL "" FORCE)

# These plugins need a host with a Connection.
set(OGVR_EXAMPLE_DEVICE_PLUGINS
    DummyAsync
    DummySync
    CACHE INTERNAL "" FORCE)
foreach(pluginname ${OGVR_EXAMPLE_PLUGINS} ${OGVR_EXAMPLE_DEVICE_PLUGINS})
    set(name org_opengoggles_example_${pluginname})
    add_library(${name} MODULE ${name}.cpp)
    target_link_libraries(${name} ogvrPluginKitCpp)
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        FOLDER "OGVR Example Plugins")
endforeach()