set(CMAKE_GENERATOR "@CMAKE_GENERATOR@")
set(CONFIG_FLAGS @CONFIG_FLAGS@)

set(DIR "${CMAKE_CURRENT_LIST_DIR}/StandaloneBuild")
macro(run)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E echo ${ARGN})
    execute_process(
        COMMAND ${ARGN}
        WORKING_DIRECTORY "${DIR}"
        RESULT_VARIABLE RES)
    if(NOT RES EQUAL 0)
        message(FATAL_ERROR "Got Result ${RES}")
    endif()
endmacro()

# Avoid truncating output.
message(STATUS "CTEST_FULL_OUTPUT")

message(STATUS "Clearing build...")
execute_process(COMMAND "${CMAKE_COMMAND}" -E remove_directory "${DIR}")
execute_process(COMMAND "${CMAKE_COMMAND}" -E make_directory "${DIR}")

message(STATUS "Configuring...")
run("${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" ${CONFIG_FLAGS} "${SOURCE_DIR}")

message(STATUS "Compiling...")
run("${CMAKE_COMMAND}" --build "." ${BUILD_FLAGS})

message(STATUS "Result ${RES}")