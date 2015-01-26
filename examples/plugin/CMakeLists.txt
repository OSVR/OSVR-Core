# These plugins can run on just a bare PluginHost.
set(OSVR_EXAMPLE_PLUGINS
    NullPlugin
    NullDevice
    DummyHardwareDetect
    DummyHardwareDetectCpp
    DummyHardwareDetectCpp2
    CACHE INTERNAL "" FORCE)

# These plugins need a host with a Connection.
set(OSVR_EXAMPLE_DEVICE_PLUGINS
    DummyAsync
    DummySync
    DummyDetectAndCreateAsync
    CACHE INTERNAL "" FORCE)
foreach(pluginname ${OSVR_EXAMPLE_PLUGINS} ${OSVR_EXAMPLE_DEVICE_PLUGINS})
    set(name org_opengoggles_example_${pluginname})
    add_library(${name} MODULE ${name}.cpp)
    target_link_libraries(${name} osvrPluginKitCpp)
    set_target_properties(${name} PROPERTIES
        PREFIX ""
        FOLDER "OSVR Example Plugins")
endforeach()

###
# Set up self-contained example test
set(CONFIG_FLAGS "-Dosvr_DIR=${PROJECT_BINARY_DIR}/osvr")
set(BUILD_FLAGS)
if(MSVC)
# TODO this is really for multi-config generators
    set(BUILD_FLAGS "${BUILD_FLAGS} --config $<CONFIGURATION>")
endif()
if(CMAKE_GENERATOR_TOOLSET)
    set(CONFIG_FLAGS "${CONFIG_FLAGS} -T \"${CMAKE_GENERATOR_TOOLSET}\"")
endif()


configure_file(selfcontained_test.cmake "${CMAKE_CURRENT_BINARY_DIR}/selfcontained_test.cmake" @ONLY)
add_test(NAME StandalonePluginBuilds
    COMMAND
    "${CMAKE_COMMAND}"
    "-DCONFIG_FLAGS=${CONFIG_FLAGS}"
    "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/selfcontained"
    "-DBUILD_FLAGS=${BUILD_FLAGS}"
    -P "${CMAKE_CURRENT_BINARY_DIR}/selfcontained_test.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")