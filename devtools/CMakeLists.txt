if(BUILD_OSVR_JSON_TO_C)
    add_executable(osvr_json_to_c osvr_json_to_c.cpp)
    set_target_properties(osvr_json_to_c PROPERTIES
        FOLDER "OSVR Stock Applications")
    target_include_directories(osvr_json_to_c PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(osvr_json_to_c jsoncpp_lib ${Boost_PROGRAM_OPTIONS_LIBRARIES})
    install(TARGETS osvr_json_to_c
        EXPORT osvrTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT DevTools)
endif()
