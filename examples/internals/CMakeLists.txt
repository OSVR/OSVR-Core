
# Add a "custom target" for just putting these two headers in the IDE.
# They still get built over in /tests/cplusplus/Common, in the target TestCommon.
add_custom_target(SerializationExamples
    SOURCES
    SerializationTraitExample_Simple.h
    SerializationTraitExample_Complicated.h)

# Projection matrix sample code.
add_executable(ProjectionSample ProjectionSample.cpp)
target_link_libraries(ProjectionSample osvrUtilCpp eigen-headers)

# shared memory test executables - not automated.
add_executable(SharedMemoryServer SharedMemoryServer.cpp)
target_link_libraries(SharedMemoryServer osvrCommon)
add_executable(SharedMemoryClient SharedMemoryClient.cpp)
target_link_libraries(SharedMemoryClient osvrCommon)

foreach(target SerializationExamples ProjectionSample SharedMemoryServer SharedMemoryClient)
    set_target_properties(${target} PROPERTIES
        FOLDER "OSVR Core Internal Examples")
endforeach()
