# NOTE: When adding examples here, add them to the list in doc/libraries.dox as well!

# These we install alongside the main binaries
set(MAINCLIENTS
    AnalogCallback
    ButtonCallback
    TrackerCallback
    DisplayParameter
    Imaging)
# These are "extra"
set(CLIENTS
    MinimalInit
    TrackerState)

foreach(name ${MAINCLIENTS} ${CLIENTS})
    foreach(suffix c cpp)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.${suffix}")
            add_executable(${name}_${suffix} ${name}.${suffix})
            target_link_libraries(${name}_${suffix} osvrClientKitCpp)
            set_target_properties(${name}_${suffix} PROPERTIES
                FOLDER "OSVR Example Clients")
        endif()
    endforeach()
endforeach()

target_link_libraries(Imaging_cpp opencv_core opencv_highgui)

foreach(name ${MAINCLIENTS})
    if(TARGET ${name}_cpp)
        install(TARGETS ${name}_cpp
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
    endif()
    if(TARGET ${name}_c)
        install(TARGETS ${name}_c
            RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)
    endif()
endforeach()

foreach(name ${CLIENTS})
    if(TARGET ${name}_cpp)
        install(TARGETS ${name}_cpp
            RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)
    endif()
    if(TARGET ${name}_c)
        install(TARGETS ${name}_c
            RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)
    endif()
endforeach()

###
# Some examples should be run as tests
add_test(NAME DisplayParameter_runs
    COMMAND DisplayParameter_cpp)

###
# Set up self-contained example test
set(BUILD_FLAGS)
if(MSVC)
# TODO this is really for multi-config generators
    set(BUILD_FLAGS --config $<CONFIGURATION>)
endif()
if(CMAKE_GENERATOR_TOOLSET)
    set(CONFIG_FLAGS "${CONFIG_FLAGS} -T \"${CMAKE_GENERATOR_TOOLSET}\"")
endif()
    set(CONFIG_FLAGS "-Dosvr_DIR=${PROJECT_BINARY_DIR}")

configure_file(selfcontained_test.cmake "${CMAKE_CURRENT_BINARY_DIR}/selfcontained_test.cmake" @ONLY)
add_test(NAME StandaloneClientBuilds
    COMMAND
    "${CMAKE_COMMAND}"
    "-DCONFIG_FLAGS=${CONFIG_FLAGS}"
    "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/selfcontained"
    "-DBUILD_FLAGS=${BUILD_FLAGS}"
    -P "${CMAKE_CURRENT_BINARY_DIR}/selfcontained_test.cmake"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")