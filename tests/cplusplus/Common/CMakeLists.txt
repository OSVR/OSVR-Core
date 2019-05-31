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
get_filename_component(LIB_TO_TEST ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(TEST_EXE Test${LIB_TO_TEST})
add_executable(${TEST_EXE}
    $<TARGET_OBJECTS:osvr-catch-main>

    DummyTree.h
    CommonComponent.cpp
    PathTreeResolution.cpp
    RegStringMap.cpp
    Serialization.cpp
    SerializationExamples.cpp
    "${PROJECT_SOURCE_DIR}/examples/internals/SerializationTraitExample_Simple.h"
    "${PROJECT_SOURCE_DIR}/examples/internals/SerializationTraitExample_Complicated.h"
    ${PATHTREEJSON_SOURCES})
target_link_libraries(${TEST_EXE} osvr-catch2-interface osvr_cxx11_flags osvr${LIB_TO_TEST})
target_link_libraries(${TEST_EXE} osvrCommon JsonCpp::JsonCpp vendored-vrpn)

add_test(NAME ${LIB_TO_TEST}
    COMMAND ${TEST_EXE})
