if(WIN32)
    set(gtest_force_shared_crt ON)
endif()
add_subdirectory(../vendor/gtest-1.7.0 gtest)

#add_subdirectory(cplusplus)