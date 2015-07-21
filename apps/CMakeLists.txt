option(BUILD_EXPERIMENTAL_APPS "Should we build the experimental and possibly broken extra apps?" OFF)
if(BUILD_EXPERIMENTAL_APPS AND (NOT BUILD_SERVER OR NOT BUILD_CLIENT))
    set(BUILD_EXPERIMENTAL_APPS OFF)
    message(STATUS "Disabled BUILD_EXPERIMENTAL_APPS: requires a build with both client and server")
endif()

if(BUILD_CLIENT_APPS)
    add_subdirectory(PathTreeExport)

    add_executable(osvr_print_tree
        osvr_print_tree.cpp)
    target_link_libraries(osvr_print_tree
        osvrClientKitCpp
        osvrCommon
        jsoncpp_lib
        boost_program_options)
    set_target_properties(osvr_print_tree PROPERTIES
        FOLDER "OSVR Stock Applications")
    install(TARGETS osvr_print_tree
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

    add_executable(osvr_reset_yaw
        osvr_reset_yaw.cpp
        ClientMainloop.h
        ClientMainloopThread.h)
    target_link_libraries(osvr_reset_yaw
        jsoncpp_lib
        osvrClientKitCpp
        osvrClient
        osvrCommon
        eigen-headers
        boost_thread
        boost_program_options)
    set_target_properties(osvr_reset_yaw PROPERTIES
        FOLDER "OSVR Stock Applications")
    install(TARGETS osvr_reset_yaw
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
endif()

if(BUILD_SERVER_EXAMPLES)
    add_executable(BasicServer BasicServer.cpp)
    target_link_libraries(BasicServer
        osvrServer
        jsoncpp_lib)
    set_target_properties(BasicServer PROPERTIES
        FOLDER "OSVR Stock Applications")

    install(TARGETS BasicServer
        RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)
endif()

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
        boost_program_options)
    set_target_properties(osvr_calibrate PROPERTIES
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
        boost_program_options)
    set_target_properties(diagnose-rotation PROPERTIES
        FOLDER "OSVR Experimental Applications")

    install(TARGETS osvr_calibrate osvr_reset_yaw diagnose-rotation
        RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExperimentalApps)
endif()

if(BUILD_SERVER_APP)
    set(OSVR_SERVER_RESOURCE)
    if(WIN32)
        set(OSVR_SERVER_RESOURCE osvr_server.rc)
    endif()

    add_executable(osvr_server
        osvr_server.cpp
        ${OSVR_SERVER_RESOURCE})
    target_link_libraries(osvr_server osvrServer jsoncpp_lib)
    set_target_properties(osvr_server PROPERTIES
        FOLDER "OSVR Stock Applications")
    install(TARGETS osvr_server
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

    set(FILE_OUTPUTS)

    # Grab all the config files with a glob, to avoid missing one.
    file(GLOB CONFIG_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/osvr_server_config*.json")

    foreach(FN ${CONFIG_FILES})
        list(APPEND FILE_OUTPUTS "${FN}")
        add_custom_command(OUTPUT "${FN}"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${FN}" "$<TARGET_FILE_DIR:osvr_server>"
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${FN}"
            COMMENT "Copying JSON config file ${FN}"
            VERBATIM)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${FN}"
            DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Server)
    endforeach()

    # Copy contents of dir for both build and install trees.
    macro(osvr_copy_dir _dirname _glob _comment)
        add_custom_command(OUTPUT "${_dirname}-stamp"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:osvr_server>${_dirname}"
            COMMENT "Making ${_comment} directory"
            VERBATIM)
        set_source_files_properties("${_dirname}-stamp" PROPERTIES SYMBOLIC TRUE)
        # Grab all the files with a glob, to avoid missing one.
        file(GLOB _files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}${_dirname}" "${CMAKE_CURRENT_SOURCE_DIR}${_dirname}/${_glob}")

        foreach(FN ${_files})
            list(APPEND FILE_OUTPUTS "${_dirname}/${FN}")
            add_custom_command(OUTPUT "${_dirname}/${FN}"
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}${_dirname}/${FN}" "$<TARGET_FILE_DIR:osvr_server>${_dirname}/"
                MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}${_dirname}/${FN}"
                DEPENDS "${_dirname}-stamp"
                COMMENT "Copying ${_comment} ${FN}"
                VERBATIM)
            install(FILES "${CMAKE_CURRENT_SOURCE_DIR}${_dirname}/${FN}"
                DESTINATION ${CMAKE_INSTALL_BINDIR}${_dirname} COMPONENT Server)
        endforeach()
    endmacro()

    osvr_copy_dir(/displays *.json "JSON display descriptor")
    osvr_copy_dir(/sample-configs *.json "sample OSVR Server config")
    osvr_copy_dir(/external-devices *.json "OSVR Server Configs for External VRPN devices")
    osvr_copy_dir(/external-devices/device-descriptors *.json "Device Descriptors for External VRPN devices")

    # Have to set them as symbolic because we can't use a generator expression in add_custom_command(OUTPUT
    set_source_files_properties(${FILE_OUTPUTS} PROPERTIES SYMBOLIC TRUE)
    add_custom_target(copy_osvr_server_config_files ALL
        DEPENDS ${FILE_OUTPUTS})
endif()
