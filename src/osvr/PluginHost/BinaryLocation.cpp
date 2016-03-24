/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "BinaryLocation.h"
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#ifdef OSVR_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // OSVR_WINDOWS

#ifdef OSVR_MACOSX
#include <mach-o/dyld.h>
#endif // OSVR_MACOSX

#include <boost/filesystem.hpp>

// Standard includes
// - none

namespace osvr {
namespace pluginhost {

    namespace fs = boost::filesystem;
    static inline std::string getCanonicalGenericString(const char path[]) {
        return fs::canonical(path).generic_string();
    }
#if defined(OSVR_WINDOWS)
    std::string getBinaryLocation() {
        char buf[512] = {0};
        DWORD len = GetModuleFileNameA(nullptr, buf, sizeof(buf));
        std::string ret;
        if (0 != len) {
            ret.assign(buf, len);
        }
        return ret;
    }
#elif defined(OSVR_LINUX) || defined(OSVR_ANDROID)
    std::string getBinaryLocation() {
        return getCanonicalGenericString("/proc/self/exe");
    }
#elif defined(OSVR_NETBSD)
    std::string getBinaryLocation() {
        return getCanonicalGenericString("/proc/curproc/exe");
    }
#elif defined(OSVR_FREEBSD)
    std::string getBinaryLocation() {
        if (fs::exists("proc/curproc/file")) {
            return getCanonicalGenericString("/proc/curproc/file");
        }
        return std::string{};
        /// @todo sysctl CTL_KERN KERN_PROC KERN_PROC_PATHNAME -1
    }
#elif defined(OSVR_MACOSX)
    std::string getBinaryLocation() {
        char buf[1024] = {0};
        uint32_t len = sizeof(buf);
        std::string ret;
        if (0 == _NSGetExecutablePath(buf, &len)) {
            ret.assign(buf, len);
        }
        return ret;
    }
#else
#error "getBinaryLocation() not yet implemented for this platform!"
    std::string getBinaryLocation() { return std::string{}; }

/// @todo Solaris: getexecname()
#endif

} // namespace pluginhost
} // namespace osvr
