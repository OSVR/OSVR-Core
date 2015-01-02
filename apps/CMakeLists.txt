foreach(server BasicServer osvr_server osvr_calibrate)
    add_executable(${server} ${server}.cpp)
    target_link_libraries(${server} osvrServer)
    set_target_properties(${server} PROPERTIES
        FOLDER "OSVR Stock Applications")
endforeach()

target_link_libraries(osvr_server jsoncpp_lib)
target_link_libraries(osvr_calibrate jsoncpp_lib osvrClientKitCpp eigen-headers boost_thread ${Boost_PROGRAM_OPTIONS_LIBRARIES})

# TODO install osvr_calibrate when it's done.
install(TARGETS osvr_server
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
install(TARGETS BasicServer
    RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)

set(CONFIG_FILES
    osvr_server_config.json
    osvr_server_config.xrightydown.json
    osvr_server_config.autodetectonly.sample.json
    osvr_server_config.externalvrpn.sample.json
    osvr_server_config.shared.sample.json)

foreach(FN ${CONFIG_FILES})
    add_custom_command(TARGET osvr_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FN}" $<TARGET_FILE_DIR:osvr_server>
        COMMENT "Copying JSON config file sample ${FN}"
        VERBATIM)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${FN}"
        DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endforeach()
