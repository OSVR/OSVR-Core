cmake_minimum_required(VERSION 2.8.12)
project(ogvr-pluginkit)

# Custom CMake modules from https://github.com/rpavlik/cmake-modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(BUILD_SHARED_LIBS "Should we build as a shared library?" ON)

include(CTest)

# Set up shared destination directories to avoid Windows hassle with DLL search paths.
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif()
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif()

set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/inc")
add_subdirectory(src/ogvr)
if(BUILD_TESTING)
    add_subdirectory(examples)
    add_subdirectory(tests)
endif()

include(DoxygenTargets)
add_doxygen("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile" NO_PDF)
