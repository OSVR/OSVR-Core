include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/examples/internals")

osvr_convert_json(test_path_tree_json
    test_path_tree.json
    "${CMAKE_CURRENT_BINARY_DIR}/test_path_tree_json.h")

add_executable(TestCommon
    DummyTree.h
    PathTreeJSON.cpp
    PathTreeResolution.cpp
    Serialization.cpp
    SerializationExamples.cpp
    "${PROJECT_SOURCE_DIR}/examples/internals/SerializationTraitExample_Simple.h"
    "${PROJECT_SOURCE_DIR}/examples/internals/SerializationTraitExample_Complicated.h"
    "${CMAKE_CURRENT_BINARY_DIR}/test_path_tree_json.h")

target_link_libraries(TestCommon osvrCommon jsoncpp_lib)
setup_gtest(TestCommon)
