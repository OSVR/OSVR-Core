#
# Ensure all headers include all dependencies
#

set(IGNORED_HEADERS osvr/Util/MSStdIntC.h)

set(COMMON_TEST_LIBRARIES jsoncpp_lib eigen-headers vendored-vrpn boost_thread boost_filesystem)

add_custom_target(header_dependencies)

file(GLOB_RECURSE headers RELATIVE "${HEADER_BASE}" ${HEADER_BASE}/*.h)
set(test_index 0)
foreach(HEADER ${headers})
    list (FIND IGNORED_HEADERS "${HEADER}" _index)
    if (${_index} EQUAL -1)
        string(REPLACE ".h" "" basename "${HEADER}")
        string(MAKE_C_IDENTIFIER "${basename}" basename)
        set(test_name "test_${test_index}")

        set(extension cpp)
        if (${HEADER} MATCHES "C.h$")
            set(extension c)
        endif()

        set(source_file "${CMAKE_CURRENT_BINARY_DIR}/${test_name}.${extension}")
        configure_file(test_header_prereqs.${extension}.in "${source_file}" @ONLY)

        add_executable(${test_name} "${source_file}")
        target_include_directories(${test_name}
            PUBLIC
            $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
            $<BUILD_INTERFACE:${HEADER_BASE}>
            $<INSTALL_INTERFACE:include>)

        set_target_properties(${test_name} PROPERTIES
            FOLDER "OSVR Header dependency tests")

        if (${extension} STREQUAL cpp)
            target_compile_options(${test_name} PUBLIC ${OSVR_CXX11_FLAGS})
        endif()

        target_link_libraries(${test_name} ${COMMON_TEST_LIBRARIES})
        if (LIBRARIES_${basename})
            target_link_libraries(${test_name} ${LIBRARIES_${basename}})
        endif()

        add_test(NAME ${test_name}_builds COMMAND ${test_name})
        add_dependencies(header_dependencies ${test_name})

        math(EXPR test_index "${test_index} + 1")
    endif()
endforeach()

#
# Ensure forward-declaration headers are included in the full-declaration headers.
# That is, ensure that Blah_fwd.h is included in Blah.h.
#

file(GLOB_RECURSE fwd_headers RELATIVE "${HEADER_BASE}" ${HEADER_BASE}/*_fwd.h)
foreach(fwd_header ${fwd_headers})
    string(REPLACE "_fwd.h" ".h" full_header "${fwd_header}")
    file(STRINGS "${HEADER_BASE}/${full_header}" header_contents)
    set(_found OFF)
    foreach(line ${header_contents})
        if ("${line}" MATCHES "#include [<\"]${fwd_header}[>\"]")
            set(_found ON)
            break()
        endif()
    endforeach()
    if (NOT _found)
        message(FATAL_ERROR "The header file <${full_header}> does not include <${fwd_header}>.")
    endif()
endforeach()



