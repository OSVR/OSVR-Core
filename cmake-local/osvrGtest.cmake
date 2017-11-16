function(osvr_setup_gtest target)
    target_include_directories(${target} PRIVATE "${gtest_SOURCE_DIR}/include")
    target_link_libraries(${target} gtest_main)
    if(OSVR_GTEST_SHARED_LIB)
        target_compile_definitions(${target} PRIVATE GTEST_LINKED_AS_SHARED_LIBRARY)
        if(MSVC)
            # Disable the "bla bla" must have DLL interface to...
            # warnings.
            target_compile_options(${target} PRIVATE /wd4251 /wd4275)
        endif()
    endif()
    add_test(NAME gtest-${target} COMMAND ${target} --gtest_output=xml:test_details.${target}.$<CONFIG>.xml ${ARGN})
    set_target_properties(${target} PROPERTIES
        FOLDER "OSVR gtest Tests")
endfunction()
