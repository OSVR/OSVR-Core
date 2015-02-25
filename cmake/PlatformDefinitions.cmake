# Defines a series of preprocessor variables based on the current platform.
#
# Usage: define_platform_macros(PREFIX)
#
#  where PREFIX is the macro prefix (i.e., if PREFIX is XYZZY then the macros
#  will be named XYZZY_LINUX, XYZZY_WINDOWS, etc.).
#
# Author:
#   Kevin M. Godby <kevin@godby.org>
#

function(_define_macro prefix platform_string variable_name)
	if (${CMAKE_SYSTEM_NAME} MATCHES "${platform_string}")
		string(TOUPPER "${prefix}_${variable_name}" _varname)
		set(${_varname} TRUE PARENT_SCOPE)
	endif()
endfunction()

macro(define_platform_macros _prefix)
	_define_macro(${_prefix} "AIX" AIX)
	_define_macro(${_prefix} "BS/DOS" BSDOS)
	_define_macro(${_prefix} "FreeBSD" FREEBSD)
	_define_macro(${_prefix} "HP-UX" HPUX)
	_define_macro(${_prefix} "IRIX" IRIX)
	_define_macro(${_prefix} "Linux" LINUX)
	_define_macro(${_prefix} "GNU/kFreeBSD" KFREEBSD)
	_define_macro(${_prefix} "NetBSD" NETBSD)
	_define_macro(${_prefix} "OpenBSD" OPENBSD)
	_define_macro(${_prefix} "OFS1" OFS1)
	_define_macro(${_prefix} "SCO_SV" SCO_SV)
	_define_macro(${_prefix} "UnixWare" UNIXWARE)
	_define_macro(${_prefix} "Xenix" XENIX)
	_define_macro(${_prefix} "SunOS" SUNOS)
	_define_macro(${_prefix} "Tru64" TRU64)
	_define_macro(${_prefix} "ULTRIX" ULTRIX)
	_define_macro(${_prefix} "CYGWIN" CYGWIN)
	_define_macro(${_prefix} "Darwin" MACOSX)
endmacro()

