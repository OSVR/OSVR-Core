foreach(server BasicServer ConfigurableServer)
    add_executable(${server} ${server}.cpp)
    target_link_libraries(${server} osvrServer)
    set_target_properties(${server} PROPERTIES
        FOLDER "OSVR Stock Applications")

    install(TARGETS ${server}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endforeach()

target_link_libraries(ConfigurableServer jsoncpp_lib)
add_custom_command(TARGET ConfigurableServer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/osvrServerConfig.json" $<TARGET_FILE_DIR:ConfigurableServer>
    COMMENT "Copying default JSON config file"
    VERBATIM)