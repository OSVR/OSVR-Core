add_executable(BasicServer BasicServer.cpp)
target_link_libraries(BasicServer osvrServer)
set_target_properties(BasicServer PROPERTIES
    FOLDER "OSVR Stock Applications")

install(TARGETS BasicServer
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)