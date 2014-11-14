add_executable(BasicServer BasicServer.cpp)
target_link_libraries(BasicServer ogvrServer)
set_target_properties(BasicServer PROPERTIES
    FOLDER "OGVR Stock Applications")

install(TARGETS BasicServer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)