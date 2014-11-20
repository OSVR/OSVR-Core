include_directories("${gtest_SOURCE_DIR}/include")
function(setup_gtest target)
    target_link_libraries(${target} gtest_main)
    add_test(NAME gtest-${target} COMMAND ${target} --gtest_output=xml:test_details.${target}.$<CONFIG>.xml)
endfunction()

add_subdirectory(Util)