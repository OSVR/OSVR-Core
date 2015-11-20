add_executable(osvr_log_to_csv
    osvr_log_to_csv.cpp)
target_link_libraries(osvr_log_to_csv
    osvrClientKitCpp
    osvr_cxx11_flags)
set_target_properties(osvr_log_to_csv PROPERTIES
    FOLDER "OSVR Stock Applications")
#install(TARGETS osvr_log_to_csv
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime)
