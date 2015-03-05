cmake_minimum_required(VERSION 2.8.12)
project(osvrcore)

# Custom CMake modules from https://github.com/rpavlik/cmake-modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}")

###
# Package Options
###
include(Version)

###
# Library ABI versions
###
set(OSVR_PLUGINKIT_ABI_VERSION 0)

###
# Configuration Options
###
option(BUILD_SHARED_LIBS "Should we build as a shared library?" ON)
option(BUILD_DEV_VERBOSE "Build with verbose development messages?" ON)
option(BUILD_SERVER "Should we build the server libraries and applications?" ON)
option(BUILD_CLIENT "Should we build the client libraries and applications?" ON)
if(WIN32)
    option(Boost_USE_STATIC_LIBS "Build with Boost's static libraries?" ON)
endif()

if(NOT BUILD_CLIENT AND NOT BUILD_SERVER)
    message(FATAL_ERROR "Must have at least one of BUILD_CLIENT and BUILD_SERVER enabled!")
endif()

set(OSVR_PLUGIN_IGNORE_SUFFIX ".manualload" CACHE STRING "Suffix for plugins that shouldn't be loaded automatically")
mark_as_advanced(OSVR_PLUGIN_IGNORE_SUFFIX)

###
# CMake Modules
###

# CMake-stock modules
include(CTest)
include(CPack)
include(InstallRequiredSystemLibraries)

include(GNUInstallDirs)
if(NOT CMAKE_INSTALL_DOCDIR)
	set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME})
endif()
set(EXTRA_SAMPLE_BINDIR ${CMAKE_INSTALL_BINDIR}/extra-sample-apps)

# Shared modules from rpavlik/cmake-modules
include(MapImportedReleaseVariants)
include(EnableExtraCompilerWarnings)
include(CopyImportedTarget)
include(MSVCMultipleProcessCompile)

include(SetDefaultBuildType)
set_default_build_type(RelWithDebInfo)

include(UseFolders)
use_folders()

include(PlatformDefinitions)
define_platform_macros(OSVR)

# Modules that get shipped with an installed SDK
include(osvrAddPlugin)
include(osvrConvertJson)
include(CMakeBoostHelper)

###
# Dependencies
###

if(WIN32)
    # Hints for Windows, given the Chocolatey packages.
    if(NOT "$ENV{ChocolateyInstall}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{ChocolateyInstall}/lib" CHOCO_LIB)
        list(APPEND CMAKE_PREFIX_PATH "${CHOCO_LIB}/OpenCV/tools/opencv/build" "${CHOCO_LIB}/OpenCV.2.4.10.20150209/tools/opencv/build")
    endif()
    list(APPEND CMAKE_PREFIX_PATH "c:/OpenCV249")
endif()

find_package(libfunctionality REQUIRED)
find_package(Boost 1.44 COMPONENTS thread system date_time chrono program_options filesystem REQUIRED) # Lower version bound of 1.43 for range adapters, 1.44 for Filesystem v3
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
if(WIN32)
    set(OSVR_COPY_OPENCV ON)
else()
    set(OSVR_COPY_OPENCV OFF)
endif()

add_library(boost_thread INTERFACE)
target_link_libraries(boost_thread
    INTERFACE
    ${Boost_THREAD_LIBRARIES}
    ${Boost_SYSTEM_LIBRARIES}
    ${Boost_DATE_TIME_LIBRARIES}
    ${Boost_CHRONO_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(boost_thread INTERFACE ${Boost_INCLUDE_DIRS})
if(WIN32 AND NOT MSVC AND Boost_USE_STATIC_LIBS)
    # Work around a foolish insistence to use DLLs on MXE that don't exist.
    target_compile_definitions(boost_thread
        INTERFACE
        BOOST_THREAD_USE_LIB)
endif()

add_library(boost_filesystem INTERFACE)
target_link_libraries(boost_filesystem
    INTERFACE
    ${Boost_FILESYSTEM_LIBRARIES}
    ${Boost_SYSTEM_LIBRARIES})
target_include_directories(boost_filesystem INTERFACE ${Boost_INCLUDE_DIRS})
target_compile_definitions(boost_filesystem INTERFACE BOOST_FILESYSTEM_VERSION=3)

find_package(jsoncpp REQUIRED)

include(CompilerFeatures)

###
# Set up build directories
###
# Set up shared destination directories to avoid Windows hassle with DLL search paths.
if(WIN32)
	set(OSVR_PLUGIN_DIR "${CMAKE_INSTALL_BINDIR}/osvr-plugins-${OSVR_PLUGINKIT_ABI_VERSION}")
	set(OSVR_SHARED_LIBRARY_DIR "${CMAKE_INSTALL_BINDIR}")
else()
	set(OSVR_PLUGIN_DIR "${CMAKE_INSTALL_LIBDIR}/osvr-plugins-${OSVR_PLUGINKIT_ABI_VERSION}")
	set(OSVR_SHARED_LIBRARY_DIR "${CMAKE_INSTALL_LIBDIR}")
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${OSVR_SHARED_LIBRARY_DIR}")
endif()
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
endif()


# We set this variable so that we can share functions like osvr_add_plugin
# between builds in this, the main source tree, and external builds using a config file.
set(OSVR_CACHED_PLUGIN_DIR "${OSVR_PLUGIN_DIR}" CACHE INTERNAL
    "The OSVR_PLUGIN_DIR variable for OSVR, for use in building and installing plugins" FORCE)

# Since alias targets only work for libraries, we use this method instead to
# share the osvr_convert_json script between the main tree and external config users.
set(OSVR_JSON_TO_C_EXECUTABLE "osvr_json_to_c" CACHE INTERNAL
    "The target name for the osvr_json_to_c executable" FORCE)

###
# Building
###
add_subdirectory(vendor)

# Set RPATH for dynamic library search.
if(NOT WIN32 AND NOT APPLE)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()
set(HEADER_BASE "${CMAKE_CURRENT_SOURCE_DIR}/inc")
set(BUILDTREE_HEADER_BASE "${CMAKE_CURRENT_BINARY_DIR}/src")
if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()
add_subdirectory(devtools)
add_subdirectory(src/osvr)
if(WIN32)
    set(CMAKE_DEBUG_POSTFIX "")
endif()

add_subdirectory(apps)
add_subdirectory(plugins)
if(BUILD_TESTING)
    add_subdirectory(examples)
    add_subdirectory(tests)
endif()

###
# Install examples
###
install(DIRECTORY examples/
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/examples"
    COMPONENT DevelExamples)

###
# Install schemas
###
install(FILES
    json-schemas/device_descriptor_schema.json
    json-schemas/README.md
    DESTINATION "${CMAKE_INSTALL_DOCDIR}/json-schemas"
    COMPONENT Doc)

###
# Build and install documentation
###
if(WIN32)
    set(README_DEST .)
else()
    set(README_DEST "${CMAKE_INSTALL_DOCDIR}")
endif()

find_package(Markdown)
if(MARKDOWN_FOUND)
    include(UseMarkdown)
    add_markdown_target(markdown-readme ${CMAKE_CURRENT_BINARY_DIR} README.md)
    install_markdown_target(markdown-readme DESTINATION "${README_DEST}" COMPONENT Runtime)
else()
    install(FILES README.md
        DESTINATION "${README_DEST}"
        COMPONENT Runtime
        RENAME Readme.txt)
endif()

include(DoxygenTargets)
# Internal Doxygen (includes implementation details) - target "doc"
add_doxygen("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
    EXTRA_INPUT "${CMAKE_CURRENT_BINARY_DIR}/src"
    EXTRA_STRIP_FROM_PATH "${CMAKE_CURRENT_BINARY_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}"
    EXTRA_STRIP_FROM_INC_PATH "${CMAKE_CURRENT_BINARY_DIR}/src"
    NO_PDF)
# External Doxygen (only includes interfaces) - target "doc_external"
add_doxygen("${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-external"
    EXTRA_STRIP_FROM_PATH "${CMAKE_CURRENT_SOURCE_DIR}"
    EXTRA_STRIP_FROM_INC_PATH "${CMAKE_CURRENT_BINARY_DIR}/src"
    OUTPUT_DIRECTORY docs-external
    DOC_TARGET doc_external
    INSTALL_DESTINATION "${CMAKE_INSTALL_DOCDIR}/api"
    INSTALL_COMPONENT DevelDocs
    NO_PDF)


###
# Set Up Package for External Usage
###
include(CMakePackageConfigHelpers)
set(PLUGINDIR ${OSVR_PLUGIN_DIR})
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/osvr")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/osvrConfigVersion.cmake"
    VERSION ${OSVR_VERSION}
    COMPATIBILITY AnyNewerVersion)

export(EXPORT osvrTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/osvrTargets.cmake"
    NAMESPACE osvr::)

# Register the current build dir as a package search location
export(PACKAGE osvr)

configure_file(osvrConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/osvrConfig.cmake"
    @ONLY)

configure_file(osvrConfigBuildTreePaths.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/osvrConfigBuildTreePaths.cmake"
    @ONLY)

install(EXPORT osvrTargets
    FILE
    osvrTargets.cmake
    NAMESPACE
    osvr::
    DESTINATION
    ${CONFIG_INSTALL_DIR}
    COMPONENT
    Devel)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/osvrConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/osvrConfigVersion.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/osvrAddPlugin.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/osvrConfigInstalledBoost.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/osvrConfigInstalledOpenCV.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/osvrConvertJson.cmake"
    DESTINATION
    ${CONFIG_INSTALL_DIR}
    COMPONENT
    Devel)

if(WIN32)
    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/CMakeBoostHelper.cmake"
        DESTINATION
        ${CONFIG_INSTALL_DIR}
        COMPONENT
        Devel)
endif()
