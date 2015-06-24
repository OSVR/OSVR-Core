function(osvr_setup_gtest target)
    target_include_directories(${target} PRIVATE "${gtest_SOURCE_DIR}/include")
    target_link_libraries(${target} gtest_main)
    add_test(NAME gtest-${target} COMMAND ${target} --gtest_output=xml:test_details.${target}.$<CONFIG>.xml ${ARGN})
    set_target_properties(${target} PROPERTIES
        FOLDER "OSVR gtest Tests")
endfunction()
