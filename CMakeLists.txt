cmake_minimum_required(VERSION 2.8.12)
project(ogvrcore)
set(OGVR_VERSION 0.1)

###
# Configuration Options
###
option(BUILD_SHARED_LIBS "Should we build as a shared library?" ON)
option(BUILD_DEV_VERBOSE "Build with verbose development messages?" ON)
if(WIN32)
    option(Boost_USE_STATIC_LIBS "Build with Boost's static libraries?" ON)
endif()

# Custom CMake modules from https://github.com/rpavlik/cmake-modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}")

###
# CMake Modules
###
include(CTest)
include(GNUInstallDirs)
include(EnableExtraCompilerWarnings)

include(CMakeBoostHelper)

if(MSVC_IDE AND MSVC_VERSION LESS 1700)
    # At least VS 2013 Express has folder support...
    option(BUILD_WITH_PROJECT_FOLDERS "Enable project folders in the IDE. May only work in non-Express Editions!" OFF)
else()
    option(BUILD_WITH_PROJECT_FOLDERS "Enable project folders in the IDE." ON)
endif()
set_property(GLOBAL PROPERTY
    USE_FOLDERS ${BUILD_WITH_PROJECT_FOLDERS})

###
# Dependencies
###
find_package(libfunctionality REQUIRED)
find_package(Boost 1.43 COMPONENTS thread system date_time chrono REQUIRED) # Lower version bound of 1.43 for range adapters
find_package(Threads REQUIRED)

include(CompilerFeatures)

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
if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "add a postfix, usually d on windows" FORCE)
endif()

# Set RPATH for dynamic library search.
if(NOT WIN32 AND NOT APPLE)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()
set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set(BUILDTREE_HEADER_BASE "${CMAKE_CURRENT_BINARY_DIR}/src")
add_subdirectory(src/ogvr)
add_subdirectory(apps)
add_subdirectory(plugins)
if(BUILD_TESTING)
    add_subdirectory(examples)
    add_subdirectory(tests)
endif()

include(DoxygenTargets)
# Internal Doxygen (includes implementation details) - target "doc"
add_doxygen("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
    EXTRA_INPUT "${CMAKE_CURRENT_BINARY_DIR}/src"
    EXTRA_STRIP_FROM_PATH "${CMAKE_CURRENT_BINARY_DIR}/src"
    EXTRA_STRIP_FROM_INC_PATH "${CMAKE_CURRENT_BINARY_DIR}/src"
    NO_PDF)
# External Doxygen (only includes interfaces) - target "doc_external"
add_doxygen("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-external"
    EXTRA_INPUT "${CMAKE_CURRENT_BINARY_DIR}/src"
    EXTRA_STRIP_FROM_PATH "${CMAKE_CURRENT_BINARY_DIR}/src"
    EXTRA_STRIP_FROM_INC_PATH "${CMAKE_CURRENT_BINARY_DIR}/src"
    OUTPUT_DIRECTORY docs-external
    DOC_TARGET doc_external
    NO_PDF)


###
# Set Up Package for External Usage
###
include(CMakePackageConfigHelpers)
set(BUILD_CONFIG_BASE "${CMAKE_CURRENT_BINARY_DIR}/ogvr/ogvr")
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/ogvr")

write_basic_package_version_file(
    "${BUILD_CONFIG_BASE}ConfigVersion.cmake"
    VERSION ${OGVR_VERSION}
    COMPATIBILITY AnyNewerVersion)

export(EXPORT ogvrTargets
    FILE "${BUILD_CONFIG_BASE}Targets.cmake"
    NAMESPACE ogvr::)

configure_file(ogvrConfig.cmake
    "${BUILD_CONFIG_BASE}Config.cmake"
    COPYONLY)

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
