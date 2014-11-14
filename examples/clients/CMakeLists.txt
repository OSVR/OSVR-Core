set(CLIENTS
    MinimalInit
    TrackerCallback
    #sample-tracker-reader
)
foreach(name ${CLIENTS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} ogvrClientKit)
    set_target_properties(${name} PROPERTIES
        FOLDER "OGVR Example Clients")TrackerCallback
    install(TARGETS ${name}
        LIBRARY DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ExampleClients)
endforeach()