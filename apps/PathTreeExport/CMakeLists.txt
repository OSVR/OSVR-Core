add_executable(PathTreeExport
    DOTGraphOutput.cpp
    DOTGraphOutput.h
    GraphFactoryRegistration.h
    GraphOutputInterface.cpp
    GraphOutputInterface.h
    PathTreeExport.cpp)
target_link_libraries(PathTreeExport
    osvrCommon
    JsonCpp::JsonCpp
    osvrClientKitCpp
    ${Boost_PROGRAM_OPTIONS_LIBRARIES})
set_target_properties(PathTreeExport PROPERTIES
    FOLDER "OSVR Stock Applications")
install(TARGETS PathTreeExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
