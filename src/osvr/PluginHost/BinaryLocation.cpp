/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include "BinaryLocation.h"
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#ifdef OSVR_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // OSVR_WINDOWS

#include <boost/filesystem.hpp>

// Standard includes
// - none

namespace osvr {
namespace pluginhost {

#if defined(OSVR_WINDOWS)
    std::string getBinaryLocation() {
        char buf[512] = {0};
        DWORD len = GetModuleFileName(NULL, buf, sizeof(buf));
        std::string ret;
        if (0 != len) {
            ret.assign(buf, len);
        }
        return ret;
    }
#elif defined(OSVR_LINUX)
    std::string getBinaryLocation() {
        return boost::filesystem::canonical("/proc/self/exe").generic_string();
    }
#elif defined(OSVR_NETBSD)
    std::string getBinaryLocation() {
        return boost::filesystem::canonical("/proc/curproc/exe")
            .generic_string();
    }
#elif defined(OSVR_FREEBSD)
    std::string getBinaryLocation() {
        if (boost::filesystem::exists("proc/curproc/file")) {
            return boost::filesystem::canonical("/proc/curproc/file")
                .generic_string();
        } else {
            // sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
        }
    }
#else
#error "getBinaryLocation() not yet implemented for this platform!"
    std::string getBinaryLocation() { return ""; }

// TODO Mac OS X: _NSGetExecutablePath() (man 3 dyld)
// TODO Solaris: getexecname()
#endif

} // namespace pluginhost
} // namespace osvr
