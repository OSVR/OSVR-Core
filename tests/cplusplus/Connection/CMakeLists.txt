add_executable(Connection
    AsyncAccessControl.cpp)
target_link_libraries(Connection osvrConnection boost_thread)
osvr_setup_gtest(Connection)
