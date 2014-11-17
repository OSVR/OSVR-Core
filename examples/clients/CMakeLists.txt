set(CLIENTS
    MinimalInit
    TrackerCallback)

foreach(name ${CLIENTS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} osvrClientKit)
    set_target_properties(${name} PROPERTIES
        FOLDER "OSVR Example Clients")
    install(TARGETS ${name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
endforeach()