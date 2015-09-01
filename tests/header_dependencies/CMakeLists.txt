#
# Ensure all headers include all dependencies
#

set(IGNORED_HEADERS osvr/Util/MSStdIntC.h)
if(NOT BUILD_USBSERIALENUM)
    list(APPEND IGNORED_HEADERS
        osvr/USBSerial/USBSerialDevice.h
        osvr/USBSerial/USBSerialEnum.h)
endif()

set(LIBRARIES_PluginKit libfunctionality::functionality)
set(LIBRARIES_VRPNServer libfunctionality::functionality)
set(LIBRARIES_TypePack osvrTypePack)

set(COMMON_TEST_LIBRARIES jsoncpp_lib eigen-headers vendored-vrpn boost_thread)
if(TARGET boost_filesystem)
    list(APPEND COMMON_TEST_LIBRARIES boost_filesystem)
endif()

add_custom_target(header_dependencies)

file(GLOB_RECURSE headers RELATIVE "${HEADER_BASE}" ${HEADER_BASE}/*.h)
set(test_index 0)
foreach(HEADER ${headers})
    # Sample of relevant variables computed here
    # HEADER: osvr/Util/TimeValue.h
    # symbolname: osvr_Util_TimeValue
    # libname: Util

    # Compute symbolname
    string(REPLACE ".h" "" symbolname "${HEADER}")
    string(MAKE_C_IDENTIFIER "${symbolname}" symbolname)

    # Compute libname
    string(REGEX REPLACE "osvr/" "" libname "${HEADER}")
    string(REGEX REPLACE "/[^/]+$" "" libname "${libname}")

    list(FIND IGNORED_HEADERS "${HEADER}" _index)
    # If we didn't explicitly ignore this and if we built this target
    if(${_index} EQUAL -1 AND TARGET "osvr${libname}")

        #message(STATUS "${HEADER}: '${symbolname}' - '${libname}'")


        set(extensions cpp)
        if (${HEADER} MATCHES "C.h$")
            # Header required to be C-safe, enforce it!
            list(APPEND extensions c)
        endif()

        foreach(extension ${extensions})
            # Name the test and output file with a number, to dodge Windows path length limits.
            # Call it header, instead of test, to avoid polluting the 'executable'
            set(test_name "header_${extension}_${test_index}")

            set(source_file "${CMAKE_CURRENT_BINARY_DIR}/main${test_index}.${extension}")
            configure_file(test_header_prereqs.${extension}.in "${source_file}" @ONLY)

            add_executable(${test_name} "${source_file}")
            target_include_directories(${test_name}
                PUBLIC
                $<BUILD_INTERFACE:${BUILDTREE_HEADER_BASE}>
                $<BUILD_INTERFACE:${HEADER_BASE}>
                $<INSTALL_INTERFACE:include>)

            set_target_properties(${test_name} PROPERTIES
                FOLDER "OSVR Header dependency tests")

            if(${extension} STREQUAL cpp)
                target_link_libraries(${test_name} PRIVATE osvr_cxx11_flags)
            endif()

            target_link_libraries(${test_name}
                PRIVATE
                ${COMMON_TEST_LIBRARIES}
                ${LIBRARIES_${symbolname}}
                ${LIBRARIES_${libname}})

            add_test(NAME ${test_name}_builds COMMAND ${test_name})
            add_dependencies(header_dependencies ${test_name})
        endforeach()
        math(EXPR test_index "${test_index} + 1")
    endif()
endforeach()

#
# Ensure forward-declaration headers are included in the full-declaration headers.
# That is, ensure that Blah_fwd.h is included in Blah.h.
#

add_test(NAME test_fwd_headers
    COMMAND ${CMAKE_COMMAND}
        -DHEADER_BASE=${HEADER_BASE}
        -P "${CMAKE_CURRENT_LIST_DIR}/CheckForwardDeclarationHeaders.cmake"
)
