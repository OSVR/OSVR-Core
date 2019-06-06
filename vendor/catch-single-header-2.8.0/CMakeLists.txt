# Copyright (c) 2018-2019 Collabora, Ltd.
#
# SPDX-License-Identifier: BSL-1.0

find_package(Catch2 CONFIG QUIET)
find_package(Threads)
add_library(osvr-catch2-interface INTERFACE)
target_link_libraries(osvr-catch2-interface
    INTERFACE
    ${CMAKE_THREAD_LIBS_INIT})
# Create an interface target to consume Catch
if(CATCH2_FOUND)
    target_link_libraries(osvr-catch2-interface INTERFACE Catch2::Catch2)
else()
    target_include_directories(osvr-catch2-interface SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
