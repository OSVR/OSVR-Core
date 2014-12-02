set(CLIENTS
    MinimalInit
    TrackerCallback
    DisplayParameter)

foreach(name ${CLIENTS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} osvrClientKitCpp)
    set_target_properties(${name} PROPERTIES
        FOLDER "OSVR Example Clients")
    install(TARGETS ${name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
endforeach()