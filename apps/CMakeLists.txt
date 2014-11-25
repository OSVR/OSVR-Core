foreach(server BasicServer osvr_server)
    add_executable(${server} ${server}.cpp)
    target_link_libraries(${server} osvrServer)
    set_target_properties(${server} PROPERTIES
        FOLDER "OSVR Stock Applications")

    install(TARGETS ${server}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endforeach()

target_link_libraries(osvr_server jsoncpp_lib)
add_custom_command(TARGET osvr_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/osvr_server_config.json" $<TARGET_FILE_DIR:osvr_server>
    COMMENT "Copying default JSON config file"
    VERBATIM)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/osvr_server_config.json"
    DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)