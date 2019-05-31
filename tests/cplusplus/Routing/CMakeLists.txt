
get_filename_component(LIB_TO_TEST ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TEST_EXE Test${LIB_TO_TEST})
add_executable(${TEST_EXE}
    AddDevice.cpp
    IsType.h
    PathElement.cpp
    PathNode.cpp
    PathTree.cpp)
target_link_libraries(${TEST_EXE} osvr-catch-main)

target_link_libraries(${TEST_EXE} osvrCommon JsonCpp::JsonCpp)
add_test(NAME ${LIB_TO_TEST}
    COMMAND ${TEST_EXE})
