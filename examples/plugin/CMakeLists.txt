set(OSVR_EXAMPLE_DEVICE_PLUGINS_SIMPLE
    com_osvr_example_DummyDetectAndCreateAsync
    com_osvr_example_MultipleSync)

set(OSVR_EXAMPLE_DEVICE_PLUGINS
    com_osvr_example_AnalogSync
    com_osvr_example_EyeTracker
    com_osvr_example_selfcontainedDetectAndCreate
    ${OSVR_EXAMPLE_DEVICE_PLUGINS_SIMPLE}
    CACHE INTERNAL "" FORCE)

foreach(pluginname ${OSVR_EXAMPLE_DEVICE_PLUGINS_SIMPLE})
    osvr_add_plugin(NAME ${pluginname} NO_INSTALL MANUAL_LOAD CPP SOURCES ${pluginname}.cpp)
    set_target_properties(${pluginname} PROPERTIES
        FOLDER "OSVR Example Plugins")
endforeach()

# Needed to find our generated json headers
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

## Build the example with a json: com_osvr_example_AnalogSync
osvr_convert_json(com_osvr_example_AnalogSync_json
    com_osvr_example_AnalogSync.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_AnalogSync_json.h")
osvr_add_plugin(NAME com_osvr_example_AnalogSync
    NO_INSTALL MANUAL_LOAD CPP
    SOURCES
    com_osvr_example_AnalogSync.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_AnalogSync_json.h")
set_target_properties(com_osvr_example_AnalogSync PROPERTIES
    FOLDER "OSVR Example Plugins")

## Build the eyetracker example
osvr_convert_json(com_osvr_example_EyeTracker_json
    com_osvr_example_EyeTracker.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_EyeTracker_json.h")
osvr_add_plugin(NAME com_osvr_example_EyeTracker
    NO_INSTALL MANUAL_LOAD CPP
    SOURCES
    com_osvr_example_EyeTracker.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_EyeTracker_json.h")
set_target_properties(com_osvr_example_EyeTracker PROPERTIES
    FOLDER "OSVR Example Plugins")
target_link_libraries(com_osvr_example_EyeTracker osvr_cxx11_flags)

## Build the code from the selfcontained example
osvr_convert_json(com_osvr_example_selfcontainedDetectAndCreate_json
    selfcontained/com_osvr_example_selfcontainedDetectAndCreate.json
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_selfcontainedDetectAndCreate_json.h")
osvr_add_plugin(NAME com_osvr_example_selfcontainedDetectAndCreate
    NO_INSTALL MANUAL_LOAD CPP
    SOURCES
    selfcontained/com_osvr_example_selfcontainedDetectAndCreate.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/com_osvr_example_selfcontainedDetectAndCreate_json.h")
set_target_properties(com_osvr_example_selfcontainedDetectAndCreate PROPERTIES
    FOLDER "OSVR Example Plugins")

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
