option(BUILD_EXPERIMENTAL_APPS "Should we build the experimental and possibly broken extra apps?" OFF)

if(WIN32)
    set(OSVR_SERVER_RESOURCE osvr_server.rc)
else()
    set(OSVR_SERVER_RESOURCE)
endif()

add_executable(osvr_server
    osvr_server.cpp
    ${OSVR_SERVER_RESOURCE})
target_link_libraries(osvr_server osvrServer jsoncpp_lib)
set_target_properties(osvr_server PROPERTIES
    FOLDER "OSVR Stock Applications")
install(TARGETS osvr_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

add_executable(BasicServer BasicServer.cpp)
target_link_libraries(BasicServer osvrServer)
set_target_properties(BasicServer PROPERTIES
    FOLDER "OSVR Stock Applications")

install(TARGETS BasicServer
    RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)

if(BUILD_EXPERIMENTAL_APPS)
    add_executable(osvr_calibrate
        osvr_calibrate.cpp
        ClientMainloop.h
        RecomposeTransform.h
        WrapRoute.h)
    target_link_libraries(osvr_calibrate
        osvrServer
        jsoncpp_lib
        osvrClientKitCpp
        osvrCommon
        eigen-headers
        boost_thread
        ${Boost_PROGRAM_OPTIONS_LIBRARIES})
    set_target_properties(osvr_calibrate PROPERTIES
        FOLDER "OSVR Experimental Applications")

    add_executable(osvr_reset_yaw
        osvr_reset_yaw.cpp
        ClientMainloop.h
        ClientMainloopThread.h
        RecomposeTransform.h
        WrapRoute.h)
    target_link_libraries(osvr_reset_yaw
        jsoncpp_lib
        osvrClientKitCpp
        osvrClient
        osvrCommon
        eigen-headers
        boost_thread
        ${Boost_PROGRAM_OPTIONS_LIBRARIES})
    set_target_properties(osvr_reset_yaw PROPERTIES
        FOLDER "OSVR Experimental Applications")

    add_executable(diagnose-rotation
        diagnose-rotation.cpp
        ClientMainloop.h
        ClientMainloopThread.h
        RecomposeTransform.h)
    target_link_libraries(diagnose-rotation
        jsoncpp_lib
        osvrClientKitCpp
        osvrClient
        osvrCommon
        eigen-headers
        boost_thread
        ${Boost_PROGRAM_OPTIONS_LIBRARIES})
    set_target_properties(diagnose-rotation PROPERTIES
        FOLDER "OSVR Experimental Applications")

    install(TARGETS osvr_calibrate osvr_reset_yaw diagnose-rotation
        RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExperimentalApps)
endif()

# Grab all the config files with a glob, to avoid missing one.
file(GLOB CONFIG_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/osvr_server_config*.json")

foreach(FN ${CONFIG_FILES})
    add_custom_command(TARGET osvr_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FN}" $<TARGET_FILE_DIR:osvr_server>
        COMMENT "Copying JSON config file sample ${FN}"
        VERBATIM)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${FN}"
        DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endforeach()
