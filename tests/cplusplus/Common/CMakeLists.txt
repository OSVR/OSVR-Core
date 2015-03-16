include_directories("${CMAKE_CURRENT_BINARY_DIR}")

osvr_convert_json(test_path_tree_json
    test_path_tree.json
    "${CMAKE_CURRENT_BINARY_DIR}/test_path_tree_json.h")

add_executable(TestCommon
    PathTreeJSON.cpp
    Serialization.cpp
    "${CMAKE_CURRENT_BINARY_DIR}/test_path_tree_json.h")

target_link_libraries(TestCommon osvrCommon)
setup_gtest(TestCommon)
