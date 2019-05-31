
get_filename_component(LIB_TO_TEST ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TEST_EXE Test${LIB_TO_TEST})
add_executable(${TEST_EXE}
    $<TARGET_OBJECTS:osvr-catch-main>
    AsyncAccessControl.cpp)
target_link_libraries(${TEST_EXE} osvr-catch2-interface osvr_cxx11_flags)

target_link_libraries(${TEST_EXE} osvrConnection boost_thread)
add_test(NAME ${LIB_TO_TEST}
    COMMAND ${TEST_EXE})
