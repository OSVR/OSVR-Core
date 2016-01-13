# - Find jsoncpp - stopgap find module
# This is a stopgap find module to find older jsoncpp versions and those sadly built
# without JSONCPP_WITH_CMAKE_PACKAGE=ON. It also wraps the different versions of the module.
#
#  JsonCpp::JsonCpp - Imported target (possibly an interface/alias) to use:
#  if anything is populated, this is. If both shared and static are found, then
#  this will be the static version on DLL platforms and shared on non-DLL platforms.
#  JsonCpp::JsonCppShared - Imported target (possibly an interface/alias) for a
#  shared library version.
#  JsonCpp::JsonCppStatic - Imported target (possibly an interface/alias) for a
#  static library version.
#  JSONCPP_FOUND - True if JsonCpp was found.
#
# Original Author:
# 2016 Ryan Pavlik <ryan.pavlik@gmail.com>
# Incorporates work from the module contributed to VRPN under the same license:
# 2011 Philippe Crassous (ENSAM ParisTech / Institut Image) p.crassous _at_ free.fr
#
# Copyright Philippe Crassous 2011.
# Copyright Sensics, Inc. 2016.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# We will always try to produce at least one imported target, and e haven't found an imported target.
if(NOT TARGET JsonCpp::JsonCpp AND NOT TARGET jsoncpp_lib AND NOT TARGET jsoncpp_lib_static)
	# See if we find a CMake config file.
	find_package(jsoncpp QUIET NO_MODULE)
	if(jsoncpp_FOUND)
		# OK, so we found something.
		add_library(JsonCpp::JsonCpp INTERFACE IMPORTED)
		if(TARGET jsoncpp_lib AND TARGET jsoncpp_lib_static)
			# A veritable cache of riches - we have both shared and static!
			add_library(JsonCpp::JsonCppShared INTERFACE IMPORTED)
			set_property(TARGET JsonCpp::JsonCppShared PROPERTY INTERFACE_LINK_LIBRARIES jsoncpp_lib)

			add_library(JsonCpp::JsonCppStatic INTERFACE IMPORTED)
			set_property(TARGET JsonCpp::JsonCppStatic PROPERTY INTERFACE_LINK_LIBRARIES jsoncpp_lib_static)

			if(WIN32 OR CYGWIN OR MINGW)
				# DLL platforms: static library should be default
				set_property(TARGET JsonCpp::JsonCpp PROPERTY INTERFACE_LINK_LIBRARIES jsoncpp_lib_static)
			else()
				# Other platforms - might require PIC to be linked into shared libraries, so safest to prefer shared.
				set_property(TARGET JsonCpp::JsonCpp PROPERTY INTERFACE_LINK_LIBRARIES jsoncpp_lib)
			endif()
		elseif(TARGET jsoncpp_lib_static)
			# Well, only one variant, but we know for sure that it's static.
			add_library(JsonCpp::JsonCppStatic INTERFACE IMPORTED)
			set_property(TARGET JsonCpp::JsonCppStatic PROPERTY INTERFACE_LINK_LIBRARIES jsoncpp_lib_static)
			set_property(TARGET JsonCpp::JsonCpp PROPERTY INTERFACE_LINK_LIBRARIES jsoncpp_lib_static)
		else() #elseif(TARGET jsoncpp_lib)
			# One variant, and we have no idea if this is just an old version or if
			# this is shared based on the target name alone. Hmm.
			# TODO figure out if this is shared or static?
			set_property(TARGET JsonCpp::JsonCpp PROPERTY INTERFACE_LINK_LIBRARIES jsoncpp_lib)
		endif()

		# As a convenience...
		if(TARGET jsoncpp_lib_static AND NOT TARGET jsoncpp_lib)
			add_library(jsoncpp_lib INTERFACE)
			target_link_libraries(jsoncpp_lib INTERFACE jsoncpp_lib_static)
		endif()

		set(JSONCPP_LIBRARY JsonCpp::JsonCpp)

		include(FindPackageHandleStandardArgs)
		find_package_handle_standard_args(JsonCpp
			DEFAULT_MSG
			JSONCPP_LIBRARY)
	endif()
endif()

# Still nothing after looking for the config file.
if(NOT TARGET JsonCpp::JsonCpp)

	# Invoke pkgconfig for hints
	find_package(PkgConfig QUIET)
	set(_JSONCPP_INCLUDE_HINTS)
	set(_JSONCPP_LIB_HINTS)
	if(PKG_CONFIG_FOUND)
		pkg_search_module(_JSONCPP_PC QUIET jsoncpp)
		if(_JSONCPP_PC_INCLUDE_DIRS)
			set(_JSONCPP_INCLUDE_HINTS ${_JSONCPP_PC_INCLUDE_DIRS})
		endif()
		if(_JSONCPP_PC_LIBRARY_DIRS)
			set(_JSONCPP_LIB_HINTS ${_JSONCPP_PC_LIBRARY_DIRS})
		endif()
		if(_JSONCPP_PC_LIBRARIES)
			set(_JSONCPP_LIB_NAMES ${_JSONCPP_PC_LIBRARIES})
		endif()
	endif()

	if(NOT _JSONCPP_LIB_NAMES)
		# OK, if pkg-config wasn't able to give us a library name suggestion, then we may
		# have to resort to some intense old logic.
		set(_JSONCPP_LIB_NAMES jsoncpp)
		set(_JSONCPP_PATHSUFFIXES)

		if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
			list(APPEND _JSONCPP_PATHSUFFIXES
				linux-gcc) # bit of a generalization but close...
		endif()
		if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
			list(APPEND
				_JSONCPP_LIB_NAMES
				json_linux-gcc-${CMAKE_CXX_COMPILER_VERSION}_libmt
				json_linux-gcc_libmt)
			list(APPEND _JSONCPP_PATHSUFFIXES
				linux-gcc-${CMAKE_CXX_COMPILER_VERSION})

		elseif(MSVC)
			if(MSVC_VERSION EQUAL 1200)
				list(APPEND _JSONCPP_LIB_NAMES json_vc6_libmt)
				list(APPEND _JSONCPP_PATHSUFFIXES msvc6)
			elseif(MSVC_VERSION EQUAL 1300)
				list(APPEND _JSONCPP_LIB_NAMES json_vc7_libmt)
				list(APPEND _JSONCPP_PATHSUFFIXES msvc7)
			elseif(MSVC_VERSION EQUAL 1310)
				list(APPEND _JSONCPP_LIB_NAMES json_vc71_libmt)
				list(APPEND _JSONCPP_PATHSUFFIXES msvc71)
			elseif(MSVC_VERSION EQUAL 1400)
				list(APPEND _JSONCPP_LIB_NAMES json_vc8_libmt)
				list(APPEND _JSONCPP_PATHSUFFIXES msvc80)
			elseif(MSVC_VERSION EQUAL 1500)
				list(APPEND _JSONCPP_LIB_NAMES json_vc9_libmt)
				list(APPEND _JSONCPP_PATHSUFFIXES msvc90)
			elseif(MSVC_VERSION EQUAL 1600)
				list(APPEND _JSONCPP_LIB_NAMES json_vc10_libmt)
				list(APPEND _JSONCPP_PATHSUFFIXES msvc10 msvc100)
			endif()

		elseif(MINGW)
			list(APPEND _JSONCPP_LIB_NAMES
				json_mingw_libmt)
			list(APPEND _JSONCPP_PATHSUFFIXES mingw)

		else()
			list(APPEND _JSONCPP_LIB_NAMES
				json_suncc_libmt
				json_vacpp_libmt)
		endif()
	endif() # end of old logic

	# Actually go looking.
	find_path(JsonCpp_INCLUDE_DIR
		NAMES
		json/json.h
		PATH_SUFFIXES jsoncpp
		HINTS ${_JSONCPP_INCLUDE_HINTS})
	find_library(JsonCpp_LIBRARY
		NAMES
		${_JSONCPP_LIB_NAMES}
		PATHS libs
		PATH_SUFFIXES ${_JSONCPP_PATHSUFFIXES}
		HINTS ${_JSONCPP_LIB_HINTS})
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(JsonCpp
		DEFAULT_MSG
		JsonCpp_INCLUDE_DIR
		JsonCpp_LIBRARY)
	if(JSONCPP_FOUND)

		# We already know that the target doesn't exist, let's make it.
		# TODO don't know why we get errors like:
		# error: 'JsonCpp::JsonCpp-NOTFOUND', needed by 'bin/osvr_json_to_c', missing and no known rule to make it
		# when we do the imported target commented out below. So, instead, we make an interface
		# target with an alias. Hmm.

		#add_library(JsonCpp::JsonCpp UNKNOWN IMPORTED)
		#set_target_properties(JsonCpp::JsonCpp PROPERTIES
		#	IMPORTED_LOCATION "${JsonCpp_LIBRARY}"
		#	INTERFACE_INCLUDE_DIRECTORIES "${JsonCpp_INCLUDE_DIR}"
		#	IMPORTED_LINK_INTERFACE_LANGUAGES "CXX")

		add_library(jsoncpp_interface INTERFACE)
		set_target_properties(jsoncpp_interface PROPERTIES
			INTERFACE_LINK_LIBRARIES "${JsonCpp_LIBRARY}"
			INTERFACE_INCLUDE_DIRECTORIES "${JsonCpp_INCLUDE_DIR}")
		add_library(JsonCpp::JsonCpp ALIAS jsoncpp_interface)
		mark_as_advanced(JsonCpp_INCLUDE_DIR JsonCpp_LIBRARY)
	endif()
endif()

if(JSONCPP_FOUND)
	mark_as_advanced(jsoncpp_DIR)
endif()
