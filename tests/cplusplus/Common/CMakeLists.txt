include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/examples/internals")

set(PATHTREEJSON_SOURCES)
if(HAVE_OSVR_JSON_TO_C)
    osvr_convert_json(test_path_tree_json
        test_path_tree.json
        "${CMAKE_CURRENT_BINARY_DIR}/test_path_tree_json.h")
    set(PATHTREEJSON_SOURCES
        test_path_tree.json
        PathTreeJSON.cpp
        "${CMAKE_CURRENT_BINARY_DIR}/test_path_tree_json.h")
endif()

add_executable(TestCommon
    DummyTree.h
    CommonComponent.cpp
    PathTreeResolution.cpp
    RegStringMap.cpp
    Serialization.cpp
    SerializationExamples.cpp
    "${PROJECT_SOURCE_DIR}/examples/internals/SerializationTraitExample_Simple.h"
    "${PROJECT_SOURCE_DIR}/examples/internals/SerializationTraitExample_Complicated.h"
    ${PATHTREEJSON_SOURCES})

target_link_libraries(TestCommon osvrCommon JsonCpp::JsonCpp vendored-vrpn)
osvr_setup_gtest(TestCommon)
