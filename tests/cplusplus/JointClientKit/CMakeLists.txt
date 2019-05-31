
get_filename_component(LIB_TO_TEST ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TEST_EXE Test${LIB_TO_TEST})
add_executable(${TEST_EXE}
    $<TARGET_OBJECTS:osvr-catch-main>

    JointClientKit.cpp
    JointClientKitWithInterface.cpp)
target_link_libraries(${TEST_EXE} osvr-catch2-interface osvr_cxx11_flags osvr${LIB_TO_TEST})

# Extra required lib
target_link_libraries(${TEST_EXE} osvrClientKitCpp)
add_test(NAME ${LIB_TO_TEST}-BasicJointClientKit-ConstructDestruct
    COMMAND ${TEST_EXE} BasicJointClientKit-ConstructDestruct)

if(BUILD_SERVER_EXAMPLES) # need the AnalogSync example
    add_test(NAME ${LIB_TO_TEST}-BasicJointClientKitWithInterface-ConstructDestruct
        COMMAND ${TEST_EXE} BasicJointClientKitWithInterface-ConstructDestruct)
endif()
