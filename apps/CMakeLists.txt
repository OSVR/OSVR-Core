if(WIN32)
    set(OSVR_SERVER_RESOURCE osvr_server.rc)
else()
    set(OSVR_SERVER_RESOURCE)
endif()

add_executable(osvr_server
    osvr_server.cpp
    ${OSVR_SERVER_RESOURCE})
add_executable(BasicServer BasicServer.cpp)
add_executable(osvr_calibrate
    osvr_calibrate.cpp
    ClientMainloop.h
    RecomposeTransform.h
    WrapRoute.h)

foreach(server BasicServer osvr_server osvr_calibrate)
    target_link_libraries(${server} osvrServer)
    set_target_properties(${server} PROPERTIES
        FOLDER "OSVR Stock Applications")
endforeach()

target_link_libraries(osvr_server jsoncpp_lib)
target_link_libraries(osvr_calibrate jsoncpp_lib osvrClientKitCpp osvrCommon eigen-headers boost_thread ${Boost_PROGRAM_OPTIONS_LIBRARIES})

add_executable(osvr_reset_yaw
    osvr_reset_yaw.cpp
    ClientMainloop.h
    RecomposeTransform.h
    WrapRoute.h)
target_link_libraries(osvr_reset_yaw jsoncpp_lib osvrClientKitCpp osvrClient osvrTransform osvrRouting osvrCommon eigen-headers boost_thread ${Boost_PROGRAM_OPTIONS_LIBRARIES})
set_target_properties(osvr_reset_yaw PROPERTIES
    FOLDER "OSVR Stock Applications")

# TODO install osvr_reset_yaw when complete
install(TARGETS osvr_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
install(TARGETS BasicServer osvr_calibrate
    RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)

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
