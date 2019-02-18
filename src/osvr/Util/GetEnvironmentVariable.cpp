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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef _MSC_VER
// Don't warn about getenv
#define _CRT_SECURE_NO_WARNINGS
#endif

// Internal Includes
#include <osvr/Util/GetEnvironmentVariable.h>
#include <osvr/Util/WindowsVariantC.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstdlib>

namespace osvr {
namespace util {
#if defined(OSVR_WINDOWS) && !defined(OSVR_WINDOWS_DESKTOP)
    /// On Windows, only desktop apps actually have the getenv function
    /// available to them, so this implementation just always returns not
    /// defined.
    boost::optional<std::string> getEnvironmentVariable(std::string const &) {
        return boost::optional<std::string>();
    }
#else
    /// A real implementation of the functionality.
    boost::optional<std::string>
    getEnvironmentVariable(std::string const &var) {
        boost::optional<std::string> ret;

        auto initialRet = std::getenv(var.c_str());
        if (nullptr == initialRet) {
            return ret;
        }
        std::string val(initialRet);
#ifdef OSVR_WINDOWS
        // Windows doesn't distinguish between empty and not defined, so assume
        // empty means not defined.
        if (val.empty()) {
            return ret;
        }
#endif
        ret = val;
        return ret;
    }
#endif // end of if windows non-desktop

    std::string
    getEnvironmentVariable(std::string const &var, std::string const &fallback)
    {
        const auto value = getEnvironmentVariable(var);
        if (!value)
            return fallback;
        return *value;
    }


} // namespace util
} // namespace osvr

