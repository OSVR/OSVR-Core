# These plugins require nothing more than an implementation file and possibly a JSON header.
set(OSVR_EXAMPLE_DEVICE_PLUGINS_SIMPLE
    com_osvr_example_AnalogSync
    com_osvr_example_Configured
    com_osvr_example_DummyDetectAndCreateAsync
    com_osvr_example_EyeTracker
    com_osvr_example_Locomotion
    com_osvr_example_MultipleAsync
    org_osvr_example_Tracker
    com_osvr_example_Skeleton)

# These are all the plugin targets: one listed only here need more careful configuration.
set(OSVR_EXAMPLE_DEVICE_PLUGINS
    com_osvr_example_selfcontained
    org_osvr_example_SampleCPlugin
    ${OSVR_EXAMPLE_DEVICE_PLUGINS_SIMPLE}
    CACHE INTERNAL "" FORCE)

# Needed to find our generated json headers
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

foreach(pluginname ${OSVR_EXAMPLE_DEVICE_PLUGINS_SIMPLE})
    set(json_header)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${pluginname}.json")
        set(json_header "${CMAKE_CURRENT_BINARY_DIR}/${pluginname}_json.h")
        osvr_convert_json(${pluginname}_json
            ${pluginname}.json
            "${json_header}")
    endif()
    osvr_add_plugin(NAME ${pluginname}
        NO_INSTALL
        MANUAL_LOAD
        CPP
        SOURCES
        ${pluginname}.cpp
        ${json_header})
endforeach()

# These need C++11.
foreach(pluginname
    com_osvr_example_Configured
    com_osvr_example_EyeTracker
    com_osvr_example_Locomotion
    org_osvr_example_Tracker
    com_osvr_example_MultipleAsync
    com_osvr_example_Skeleton)
    target_link_libraries(${pluginname} osvr_cxx11_flags)
endforeach()

# Extra Libraries
target_link_libraries(com_osvr_example_Configured JsonCpp::JsonCpp)

## Build the code from the selfcontained example
osvr_convert_json(com_osvr_example_selfcontained_json
    selfcontained/com_osvr_example_selfcontained.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_selfcontained_json.h")
osvr_add_plugin(NAME com_osvr_example_selfcontained
    NO_INSTALL MANUAL_LOAD CPP
    SOURCES
    selfcontained/com_osvr_example_selfcontained.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_selfcontained_json.h")
set_target_properties(com_osvr_example_selfcontained PROPERTIES
    FOLDER "OSVR Example Plugins")

## Build the C-only example
osvr_convert_json(org_osvr_example_SampleCPlugin_json
    org_osvr_example_SampleCPlugin.json
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_example_SampleCPlugin_json.h")
osvr_add_plugin(NAME org_osvr_example_SampleCPlugin
    NO_INSTALL MANUAL_LOAD
    SOURCES
    org_osvr_example_SampleCPlugin.c
    "${CMAKE_CURRENT_BINARY_DIR}/org_osvr_example_SampleCPlugin_json.h")
set_target_properties(org_osvr_example_SampleCPlugin PROPERTIES
    FOLDER "OSVR Example Plugins")


# Put all the example plugins in a suitable solution folder.
foreach(pluginname ${OSVR_EXAMPLE_DEVICE_PLUGINS})
    set_target_properties(${pluginname} PROPERTIES
        FOLDER "OSVR Example Plugins")
endforeach()

if(BUILD_TESTING)
    ###
    # Set up self-contained example test
    set(CONFIG_FLAGS "-Dosvr_DIR=${PROJECT_BINARY_DIR}")
    set(BUILD_FLAGS)
    if(MSVC)
    # TODO this is really for any multi-config generators, not just MSVC
        set(BUILD_FLAGS --config $<CONFIGURATION>)
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
endif()
