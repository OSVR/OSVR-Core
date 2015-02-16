cmake_minimum_required(VERSION 2.8.12)
project(osvrcore)

# Custom CMake modules from https://github.com/rpavlik/cmake-modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_CURRENT_SOURCE_DIR}")

###
# Package Options
###
set(CPACK_PACKAGE_NAME "osvr-core")
set(CPACK_PACKAGE_VENDOR "Sensics, Inc")
set(CPACK_PACKAGE_CONTACT "<support@opengoggles.org>")
set(CPACK_PACKAGE_VERSION_MAJOR 0)
set(CPACK_PACKAGE_VERSION_MINOR 1)
set(OSVR_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION ${OSVR_VERSION})

include(GetGitRevisionDescription)
git_describe(GIT_REVISION_DESCRIPTION --tags --match v*)
if(GIT_REVISION_DESCRIPTION)
	string(REPLACE "v" ""
		GIT_REVISION_DESCRIPTION
		"${GIT_REVISION_DESCRIPTION}")
	set(CPACK_PACKAGE_VERSION "${GIT_REVISION_DESCRIPTION}")
	message(STATUS "Git reports version ${CPACK_PACKAGE_VERSION}")
else()
	message(STATUS "Could not get revision information from Git!")
endif()

if(WIN32)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_CXX_COMPILER_ID}")
    if(MSVC)
		math(EXPR _vs_ver "${MSVC_VERSION} / 100 - 6")
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}${_vs_ver}")
	endif()
	if(CMAKE_CXX_SIZEOF_DATA_PTR EQUAL 8)
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-64bit")
	else()
		set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-32bit")
	endif()
endif()

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

###
# CMake Modules
###
include(CTest)
include(CPack)
include(GNUInstallDirs)
if(NOT CMAKE_INSTALL_DOCDIR)
	set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME})
endif()
set(EXTRA_SAMPLE_BINDIR ${CMAKE_INSTALL_BINDIR}/extra-sample-apps)
include(EnableExtraCompilerWarnings)

include(SetDefaultBuildType)
set_default_build_type(RelWithDebInfo)

include(MSVCMultipleProcessCompile)

include(CMakeBoostHelper)

include(InstallRequiredSystemLibraries)

if(MSVC_IDE AND MSVC_VERSION LESS 1600)
    # VS 2012 Express and newer has folder support...
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

if(ANDROID)
        # Android cmake toolchain bugfix for finding Boos
        find_host_package(Boost 1.43 COMPONENTS thread system date_time chrono program_options REQUIRED) # Lower version bound of 1.43 for range adapters
else()
        find_package(Boost 1.43 COMPONENTS thread system date_time chrono program_options REQUIRED) # Lower version bound of 1.43 for range adapters       
endif()

find_package(Threads REQUIRED)

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

find_package(jsoncpp REQUIRED)

include(CompilerFeatures)

###
# Set up build directories
###
# Set up shared destination directories to avoid Windows hassle with DLL search paths.
if(WIN32)
    set(OSVR_PLUGIN_DIR "${CMAKE_INSTALL_BINDIR}")
else()
    set(OSVR_PLUGIN_DIR "${CMAKE_INSTALL_LIBDIR}")
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${OSVR_PLUGIN_DIR}")
endif()
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
endif()

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
install(DIRECTORY json-schemas/
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
set(BUILD_CONFIG_BASE "${CMAKE_CURRENT_BINARY_DIR}/osvr/osvr")
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/osvr")

write_basic_package_version_file(
    "${BUILD_CONFIG_BASE}ConfigVersion.cmake"
    VERSION ${OSVR_VERSION}
    COMPATIBILITY AnyNewerVersion)

export(EXPORT osvrTargets
    FILE "${BUILD_CONFIG_BASE}Targets.cmake"
    NAMESPACE osvr::)

configure_file(osvrConfig.cmake
    "${BUILD_CONFIG_BASE}Config.cmake"
    @ONLY)

configure_file(osvrConfigBuildTreePaths.cmake
    "${BUILD_CONFIG_BASE}ConfigBuildTreePaths.cmake"
    @ONLY)

configure_file(osvrConfigInstalledBoost.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/osvrConfigInstalledBoost.cmake"
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

install(
    FILES
    "${BUILD_CONFIG_BASE}Config.cmake"
    "${BUILD_CONFIG_BASE}ConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/osvrConfigInstalledBoost.cmake"
    DESTINATION
    ${CONFIG_INSTALL_DIR}
    COMPONENT
    Devel)
