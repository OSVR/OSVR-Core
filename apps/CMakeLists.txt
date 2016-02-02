option(BUILD_EXPERIMENTAL_APPS "Should we build the experimental and possibly broken extra apps?" OFF)
if(BUILD_EXPERIMENTAL_APPS AND (NOT BUILD_SERVER OR NOT BUILD_CLIENT))
    set(BUILD_EXPERIMENTAL_APPS OFF)
    message(STATUS "Disabled BUILD_EXPERIMENTAL_APPS: requires a build with both client and server")
endif()

if(BUILD_CLIENT_APPS)
    ###
    # PathTreeExport - installed
    ###
    add_subdirectory(PathTreeExport)
    add_subdirectory(osvr_log_to_csv)

    ###
    # osvr_print_tree - installed
    ###
    add_executable(osvr_print_tree
        osvr_print_tree.cpp)
    target_link_libraries(osvr_print_tree
        osvrClientKitCpp
        osvrCommon
        JsonCpp::JsonCpp
        boost_program_options
        osvr_cxx11_flags)
    set_target_properties(osvr_print_tree PROPERTIES
        FOLDER "OSVR Stock Applications")
    install(TARGETS osvr_print_tree
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

    ###
    # osvr_dump_tree_json - NOT installed
    ###
    add_executable(osvr_dump_tree_json
        osvr_dump_tree_json.cpp)
    target_link_libraries(osvr_dump_tree_json
        osvrClientKitCpp
        osvrCommon
        JsonCpp::JsonCpp
        boost_program_options
        osvr_cxx11_flags)
    set_target_properties(osvr_dump_tree_json PROPERTIES
        FOLDER "OSVR Stock Applications")
    #install(TARGETS osvr_dump_tree_json
    #    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

    ###
    # osvr_reset_yaw - installed
    ###
    add_executable(osvr_reset_yaw
        osvr_reset_yaw.cpp
        ClientMainloop.h
        ClientMainloopThread.h)
    target_link_libraries(osvr_reset_yaw
        JsonCpp::JsonCpp
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
    ###
    # BasicServer - installed to ExtraSampleBinaries
    ###
    add_executable(BasicServer BasicServer.cpp)
    target_link_libraries(BasicServer
        osvrServer
        JsonCpp::JsonCpp)
    set_target_properties(BasicServer PROPERTIES
        FOLDER "OSVR Stock Applications")
    if(NOT WIN32)
        set_target_properties(BasicServer PROPERTIES
            INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${EXTRABINARIES_INSTALL_RPATH}")
    endif()

    install(TARGETS BasicServer
        RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExtraSampleBinaries)
endif()

if(BUILD_EXPERIMENTAL_APPS)
    ###
    # osvr_calibrate - EXPERIMENTAL - not built by default but installed in ExtraSampleBinaries if built
    ###
    add_executable(osvr_calibrate
        osvr_calibrate.cpp
        ClientMainloop.h
        RecomposeTransform.h
        WrapRoute.h)
    target_link_libraries(osvr_calibrate
        osvrServer
        JsonCpp::JsonCpp
        osvrClientKitCpp
        osvrCommon
        eigen-headers
        boost_thread
        boost_program_options)
    set_target_properties(osvr_calibrate PROPERTIES
        FOLDER "OSVR Experimental Applications")
    if(NOT WIN32)
        set_target_properties(osvr_calibrate PROPERTIES
            INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${EXTRABINARIES_INSTALL_RPATH}")
    endif()

    ###
    # diagnose-rotation - EXPERIMENTAL - not built by default but installed in ExtraSampleBinaries if built
    ###
    add_executable(diagnose-rotation
        diagnose-rotation.cpp
        ClientMainloop.h
        ClientMainloopThread.h
        RecomposeTransform.h)
    target_link_libraries(diagnose-rotation
        JsonCpp::JsonCpp
        osvrClientKitCpp
        osvrClient
        osvrCommon
        eigen-headers
        boost_thread
        boost_program_options)
    set_target_properties(diagnose-rotation PROPERTIES
        FOLDER "OSVR Experimental Applications")
    if(NOT WIN32)
        set_target_properties(diagnose-rotation PROPERTIES
            INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${EXTRABINARIES_INSTALL_RPATH}")
    endif()

    # Install the experimental apps
    install(TARGETS osvr_calibrate diagnose-rotation
        RUNTIME DESTINATION ${EXTRA_SAMPLE_BINDIR} COMPONENT ExperimentalApps)
endif()

if(BUILD_SERVER_APP)

    ###
    # osvr_server - installed
    ###
    set(OSVR_SERVER_RESOURCE)
    if(WIN32)
        set(OSVR_SERVER_RESOURCE osvr_server.rc)
    endif()

    add_executable(osvr_server
        osvr_server.cpp
        ${OSVR_SERVER_RESOURCE})
    target_link_libraries(osvr_server osvrServer JsonCpp::JsonCpp)
    set_target_properties(osvr_server PROPERTIES
        FOLDER "OSVR Stock Applications")
    install(TARGETS osvr_server
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)

    # Macro:
    # Copy contents of dir for both build and install trees - directories of JSON configs and descriptors
    macro(osvr_copy_dir _dirname _glob _builddir _installdir _comment)
        add_custom_command(OUTPUT "${_dirname}-stamp"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${_builddir}/${_dirname}"
            VERBATIM)
        set_source_files_properties("${_dirname}-stamp" PROPERTIES SYMBOLIC TRUE)
        # Grab all the files with a glob, to avoid missing one.
        file(GLOB _files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${_dirname}" "${CMAKE_CURRENT_SOURCE_DIR}/${_dirname}/${_glob}")

        foreach(FN ${_files})
            list(APPEND FILE_OUTPUTS "${_dirname}/${FN}")
            add_custom_command(OUTPUT "${_dirname}/${FN}"
                COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_dirname}/${FN}" "${_builddir}/${_dirname}/"
                MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_dirname}/${FN}"
                DEPENDS "${_dirname}-stamp"
                COMMENT "Copying ${_comment} ${FN}"
                VERBATIM)
            install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${_dirname}/${FN}"
                DESTINATION "${_installdir}/${_dirname}" COMPONENT Server)
        endforeach()
    endmacro()

    # Set up variables to begin the JSON copying
    if(WIN32)
        set(data_buildtree_dir "$<TARGET_FILE_DIR:osvr_server>")
        set(DEFAULT_CONFIG_EXTRA_ARGS)
    else()
        set(data_buildtree_dir "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")
        # Here, we need to create the directory to copy to.
        add_custom_command(OUTPUT "data_buildtree_dir-stamp"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${data_buildtree_dir}"
            VERBATIM)
        set_source_files_properties("data_buildtree_dir-stamp" PROPERTIES SYMBOLIC TRUE)
        set(DEFAULT_CONFIG_EXTRA_ARGS DEPENDS "data_buildtree_dir-stamp")
    endif()
    set(data_installtree_dir ${SAMPLE_CONFIGS_DIR})
    set(FILE_OUTPUTS)

    # Macro to easily copy for build and install a single file.
    # Put in a macro because we do different things on Windows vs. other platforms,
    # including installing different number of files.
    macro(osvr_copy_file _infile _outfile)
        list(APPEND FILE_OUTPUTS "${_infile}")
        set(__comment "Copying ${_infile}")
        if(NOT "${_infile}" STREQUAL "${_outfile}")
            set(__comment "${__comment} to ${_outfile}")
        endif()
        add_custom_command(OUTPUT "${_infile}"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${_infile}" "${data_buildtree_dir}/${_outfile}"
            MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/${_infile}"
            ${DEFAULT_CONFIG_EXTRA_ARGS}
            COMMENT "${__comment}"
            VERBATIM)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/${_infile}"
            RENAME "${_outfile}"
            DESTINATION "${data_installtree_dir}" COMPONENT Server)
    endmacro()

    # The name of the default config file.
    set(DEFAULT_CONFIG_FILE osvr_server_config.json)
    # Config file to use by default on Windows
    set(DEFAULT_CONFIG_FILE_WIN sample-configs/osvr_server_config.HDK13DirectMode.sample.json)
    # What we should call the normally-default (empty-ish) config file on Windows.
    set(DEFAULT_CONFIG_FILE_ALTNAME osvr_server_config.autodetectall.json)

    # Copy/install the default config file from the current directory.
    if(WIN32)
        # Copy the default empty config file to an alternate name.
        osvr_copy_file(${DEFAULT_CONFIG_FILE} ${DEFAULT_CONFIG_FILE_ALTNAME})
        # Copy the designated default config file for Windows to the right name to be the default.
        osvr_copy_file(${DEFAULT_CONFIG_FILE_WIN} ${DEFAULT_CONFIG_FILE})
    else()
        # If we're not Windows, we just use the standard (autoconfiguring) config file.
        osvr_copy_file(${DEFAULT_CONFIG_FILE} ${DEFAULT_CONFIG_FILE})
    endif()

    # Copy/install subdirectories of JSON files
    osvr_copy_dir(displays *.json ${data_buildtree_dir} ${SAMPLE_CONFIGS_DIR} "JSON display descriptor")
    osvr_copy_dir(sample-configs *.json ${data_buildtree_dir} ${SAMPLE_CONFIGS_DIR} "sample OSVR Server config")
    osvr_copy_dir(external-devices *.json ${data_buildtree_dir} ${SAMPLE_CONFIGS_DIR} "OSVR Server Configs for External VRPN devices")
    osvr_copy_dir(external-devices/device-descriptors *.json ${data_buildtree_dir} ${SAMPLE_CONFIGS_DIR} "Device Descriptors for External VRPN devices")

    # Have to set them as symbolic because we can't use a generator expression in add_custom_command(OUTPUT
    set_source_files_properties(${FILE_OUTPUTS} PROPERTIES SYMBOLIC TRUE)
    add_custom_target(copy_osvr_server_config_files ALL
        DEPENDS ${FILE_OUTPUTS})
endif()
