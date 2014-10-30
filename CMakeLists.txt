# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryandnet>
# http://academic.cleardefinition.com/
# Iowa State University HCI Graduate Program/VRAC

cmake_minimum_required(VERSION 2.6)

# Set package properties
project(qvrpn)

set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_VERSION
	"${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

###
# Options
###

if(WIN32)
	set(RUNTIME_LIBRARY_INSTALL_DIR bin)
else()
	set(RUNTIME_LIBRARY_INSTALL_DIR lib)
endif()

###
# Perform build configuration of dependencies
###

# Locally-developed modules dist'ed with this app - always have this first.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(UseBackportedModules)


###
# Build the project
###
add_subdirectory(qvrpn)

###
# Installer setup
###
include(InstallRequiredSystemLibraries)

if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	# We're a sub project.
	set(QVRPN_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/qvrpn PARENT_SCOPE)
	set(QVRPN_LIBRARIES qvrpn PARENT_SCOPE)
endif()
