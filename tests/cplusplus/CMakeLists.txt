
add_library(osvr-catch-main OBJECT CatchMain.cpp)
target_link_libraries(osvr-catch-main PUBLIC osvr-catch2-interface)

if(TARGET osvrTypePack)
    add_subdirectory(TypePack)
endif()
if(TARGET osvrUtil)
    add_subdirectory(Util)
endif()
if(TARGET osvrCommon)
    add_subdirectory(Routing)
    add_subdirectory(Common)
endif()

if(BUILD_SERVER)
    add_subdirectory(Connection)
    add_subdirectory(Kalman)
endif()

if(BUILD_CLIENT)
    add_subdirectory(ClientKit)
endif()


if(BUILD_SERVER AND BUILD_CLIENT)
    add_subdirectory(JointClientKit)
endif()
