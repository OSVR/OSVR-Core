add_executable(Routing
    AddDevice.cpp
    IsType.h
    PathElement.cpp
    PathNode.cpp
    PathTree.cpp)
target_link_libraries(Routing osvrCommon JsonCpp::JsonCpp)
osvr_setup_gtest(Routing)
