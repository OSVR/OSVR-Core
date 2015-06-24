add_executable(Routing
    AddDevice.cpp
    IsType.h
    PathElement.cpp
    PathNode.cpp
    PathTree.cpp)
target_link_libraries(Routing osvrCommon jsoncpp_lib)
osvr_setup_gtest(Routing)
