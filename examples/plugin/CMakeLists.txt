foreach(name
    org_opengoggles_example_NullPlugin
    org_opengoggles_example_NullDevice
    org_opengoggles_example_DummyHardwarePoll
    org_opengoggles_example_DummyAsync
    org_opengoggles_example_DummySync)
    add_library(${name} MODULE ${name}.cpp)
    target_link_libraries(${name} ogvrPluginKitCpp)
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        FOLDER "OGVR Example Plugins")
endforeach()