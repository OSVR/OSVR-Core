cmake_minimum_required(VERSION 2.8.12)
project(ogvrpluginkit)
set(ogvrpluginkit_VERSION 0.1)

###
# Configuration Options
###
option(BUILD_SHARED_LIBS "Should we build as a shared library?" ON)
option(BUILD_DEV_VERBOSE "Build with verbose development messages?" ON)

# Custom CMake modules from https://github.com/rpavlik/cmake-modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

###
# CMake Modules
###
include(CTest)
include(GNUInstallDirs)

###
# Dependencies
###
find_package(libfunctionality)
find_package(Boost 1.43) # Lower version bound of 1.43 for range adapters

###
# Set up build directories
###
# Set up shared destination directories to avoid Windows hassle with DLL search paths.
set(EXECUTABLE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${EXECUTABLE_BINARY_DIR}")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
	if(WIN32)
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${EXECUTABLE_BINARY_DIR}")
	else()
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
	endif()
endif()
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_BINARY_DIR}")
endif()

###
# Building
###
add_subdirectory(vendor)
set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set(BUILDTREE_HEADER_BASE "${CMAKE_CURRENT_BINARY_DIR}/src")
add_subdirectory(src/ogvr)
if(BUILD_TESTING)
    add_subdirectory(examples)
    add_subdirectory(apps)
    add_subdirectory(tests)
endif()

include(DoxygenTargets)
add_doxygen("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile" NO_PDF)


###
# Set Up Package for External Usage
###
include(CMakePackageConfigHelpers)
set(BUILD_CONFIG_BASE "${CMAKE_CURRENT_BINARY_DIR}/ogvr/ogvr")
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/ogvr")

write_basic_package_version_file(
    "${BUILD_CONFIG_BASE}ConfigVersion.cmake"
    VERSION ${ogvrpluginkit_VERSION}
    COMPATIBILITY AnyNewerVersion)

export(EXPORT ogvrTargets
    FILE "${BUILD_CONFIG_BASE}Targets.cmake"
    NAMESPACE ogvr::)

configure_file(ogvrConfig.cmake
    "${BUILD_CONFIG_BASE}Config.cmake"
    COPY_ONLY)

install(EXPORT ogvrTargets
    FILE
    ogvrTargets.cmake
    NAMESPACE
    ogvr::
    DESTINATION
    ${CONFIG_INSTALL_DIR})

install(
    FILES
    ogvrConfig.cmake
    "${BUILD_CONFIG_BASE}ConfigVersion.cmake"
    DESTINATION
    ${CONFIG_INSTALL_DIR}
    COMPONENT
    Devel)
