include_directories("${CMAKE_CURRENT_BINARY_DIR}")

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
    PathTreeResolution.cpp
    Serialization.cpp
    ${PATHTREEJSON_SOURCES})

target_link_libraries(TestCommon osvrCommon jsoncpp_lib)
setup_gtest(TestCommon)

# shared memory test executables - not automated.
#add_executable(SharedMemoryServer SharedMemoryServer.cpp)
#target_link_libraries(SharedMemoryServer osvrCommon)
#add_executable(SharedMemoryClient SharedMemoryClient.cpp)
#target_link_libraries(SharedMemoryClient osvrCommon)