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
