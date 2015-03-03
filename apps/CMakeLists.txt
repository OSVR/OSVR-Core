if(WIN32)
    set(OSVR_SERVER_RESOURCE osvr_server.rc)
else()
    set(OSVR_SERVER_RESOURCE)
endif()

add_executable(osvr_server osvr_server.cpp ${OSVR_SERVER_RESOURCE})
add_executable(BasicServer BasicServer.cpp)
add_executable(osvr_calibrate osvr_calibrate.cpp)

foreach(server BasicServer osvr_server osvr_calibrate)
    target_link_libraries(${server} osvrServer)
    set_target_properties(${server} PROPERTIES
        FOLDER "OSVR Stock Applications")
endforeach()

target_link_libraries(osvr_server jsoncpp_lib)
target_link_libraries(osvr_calibrate jsoncpp_lib osvrClientKitCpp eigen-headers boost_thread ${Boost_PROGRAM_OPTIONS_LIBRARIES})

install(TARGETS osvr_server osvr_calibrate
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
install(TARGETS BasicServer
    RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)

file(GLOB CONFIG_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/osvr_server_config*.json")
#set(CONFIG_FILES
#    osvr_server_config.json
#    osvr_server_config.xrightydown.json
#    osvr_server_config.autodetectonly.sample.json
#    osvr_server_config.externalvrpn.sample.json
#    osvr_server_config.HDK.sample.json
#    osvr_server_config.HDKnoHydra.sample.json
#    osvr_server_config.shared.sample.json)

foreach(FN ${CONFIG_FILES})
    add_custom_command(TARGET osvr_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FN}" $<TARGET_FILE_DIR:osvr_server>
        COMMENT "Copying JSON config file sample ${FN}"
        VERBATIM)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${FN}"
        DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endforeach()
