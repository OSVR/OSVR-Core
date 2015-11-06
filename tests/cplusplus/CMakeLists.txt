add_subdirectory(TypePack)
add_subdirectory(Util)
add_subdirectory(Routing)
add_subdirectory(Common)

if(BUILD_SERVER)
    add_subdirectory(Connection)
endif()

if(BUILD_CLIENT)
    add_subdirectory(ClientKit)
endif()


if(BUILD_SERVER AND BUILD_CLIENT)
    add_subdirectory(JointClientKit)
endif()
