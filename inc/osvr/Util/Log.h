/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com>

*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_Log_h_GUID_B6053E2B_593D_4ECA_8C3B_2C55D0FE6A49
#define INCLUDED_Log_h_GUID_B6053E2B_593D_4ECA_8C3B_2C55D0FE6A49

// Internal Includes
#include <osvr/Util/LogLevel.h>
#include <osvr/Util/LineLogger.h>
#include <osvr/Util/Logger.h>
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>
#include <memory>
#include <string>

namespace osvr {
namespace util {
namespace log {

    OSVR_UTIL_EXPORT LoggerPtr make_logger(const std::string &logger_name);

    OSVR_UTIL_EXPORT std::string getLoggingDirectory(bool make_dir = false);

} // end namespace log
} // end namespace util
} // end namespace osvr

#ifdef BUILD_DEV_VERBOSE
#define OSVR_TRACE(...)                                                        \
    ::osvr::util::log::make_logger("OSVR")->trace()                            \
        << __FILE__ << ":" << __LINE__ << ": " << __VA_ARGS__;
#else
#define OSVR_TRACE(...)
#endif

#ifdef BUILD_DEV_VERBOSE
#define OSVR_DEBUG(...)                                                        \
    ::osvr::util::log::make_logger("OSVR")->debug()                            \
        << __FILE__ << ":" << __LINE__ << ": " << __VA_ARGS__;
#else
#define OSVR_DEBUG(...)
#endif

#endif // INCLUDED_Log_h_GUID_B6053E2B_593D_4ECA_8C3B_2C55D0FE6A49

