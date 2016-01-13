if(BUILD_OSVR_JSON_TO_C)
    add_executable(osvr_json_to_c osvr_json_to_c.cpp)
    set_target_properties(osvr_json_to_c PROPERTIES
        FOLDER "OSVR Stock Applications")
    target_link_libraries(osvr_json_to_c PRIVATE JsonCpp::JsonCpp boost_program_options)
    install(TARGETS osvr_json_to_c
        EXPORT osvrTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT DevTools)
endif()
