# NOTE: When adding examples here, add them to the list in doc/libraries.dox as well!

set(CLIENTS
    MinimalInit
    AnalogCallback
    ButtonCallback
    TrackerCallback
    TrackerState
    DisplayParameter)

foreach(name ${CLIENTS})
    foreach(suffix c cpp)
        add_executable(${name}_${suffix} ${name}.${suffix})
        target_link_libraries(${name}_${suffix} osvrClientKitCpp)
        set_target_properties(${name}_${suffix} PROPERTIES
            FOLDER "OSVR Example Clients")
        install(TARGETS ${name}_${suffix}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
    endforeach()
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
add_test(NAME StandaloneClientBuilds
    COMMAND
    "${CMAKE_COMMAND}"
    "-DCONFIG_FLAGS=${CONFIG_FLAGS}"
    "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/selfcontained"
    -P "${CMAKE_CURRENT_BINARY_DIR}/selfcontained_test.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")