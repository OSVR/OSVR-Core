add_executable(Routing
    AddDevice.cpp
    IsType.h
    PathElement.cpp
    PathNode.cpp
    PathTree.cpp)
target_link_libraries(Routing osvrCommon)
setup_gtest(Routing)
