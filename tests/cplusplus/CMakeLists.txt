include_directories("${gtest_SOURCE_DIR}/include")
function(setup_gtest target)
    target_link_libraries(${target} gtest_main)
    add_test(NAME gtest-${target} COMMAND ${target} --gtest_output=xml:test_details.${target}.$<CONFIG>.xml)
    set_target_properties(${target} PROPERTIES
        FOLDER "OSVR gtest Tests")
endfunction()

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
