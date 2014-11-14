set(CMAKE_GENERATOR "@CMAKE_GENERATOR@")
set(CONFIG_FLAGS @CONFIG_FLAGS@)

set(DIR StandaloneBuild)
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E echo "Clearing build..."
    COMMAND "${CMAKE_COMMAND}" -E remove_directory "${DIR}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIR}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -E echo "Configuring..."
    COMMAND "${CMAKE_COMMAND}" -E echo "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" ${CONFIG_FLAGS} "${SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" ${CONFIG_FLAGS} "${SOURCE_DIR}"
    COMMAND "${CMAKE_COMMAND}" -E echo "Compiling..."
    COMMAND "${CMAKE_COMMAND}" -E echo "${CMAKE_COMMAND}" --build "." ${BUILD_FLAGS}
    COMMAND "${CMAKE_COMMAND}" --build "." ${BUILD_FLAGS}
    WORKING_DIRECTORY "${DIR}"
    RESULT_VARIABLE RES)
    
if(NOT RES EQUAL 0)
    message(FATAL_ERROR "Got Result ${RES}")
endif()

message(STATUS "Result ${RES}")