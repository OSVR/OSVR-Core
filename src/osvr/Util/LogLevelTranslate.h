/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_LogLevelTranslate_h_GUID_24C67818_0BC8_41B9_7003_1098631ED86F
#define INCLUDED_LogLevelTranslate_h_GUID_24C67818_0BC8_41B9_7003_1098631ED86F

// Internal Includes
#include <osvr/Util/LogLevel.h>

// Library/third-party includes
#include <spdlog/common.h>

// Standard includes
// - none

namespace osvr {
namespace util {
    namespace log {

        /// Maps OSVR log levels into spdlog levels.
        inline spdlog::level::level_enum
        convertToLevelEnum(OSVR_LogLevel level) {
            if (level <= OSVR_LOGLEVEL_TRACE) {
                return spdlog::level::trace;
            }
            if (level <= OSVR_LOGLEVEL_DEBUG) {
                return spdlog::level::debug;
            }
            if (level <= OSVR_LOGLEVEL_INFO) {
                return spdlog::level::info;
            }
            if (level <= OSVR_LOGLEVEL_NOTICE) {
                return spdlog::level::notice;
            }
            if (level <= OSVR_LOGLEVEL_WARN) {
                return spdlog::level::warn;
            }
            if (level <= OSVR_LOGLEVEL_ERROR) {
                return spdlog::level::err;
            }
            return spdlog::level::critical;
        }

        inline spdlog::level::level_enum convertToLevelEnum(LogLevel level) {
            return convertToLevelEnum(
                static_cast<OSVR_LogLevel>(static_cast<int>(level)));
        }

        /// The reverse mapping.
        inline LogLevel convertFromLevelEnum(spdlog::level::level_enum level) {
            switch (level) {
            case spdlog::level::trace:
                return LogLevel::trace;
            case spdlog::level::debug:
                return LogLevel::debug;
            case spdlog::level::info:
                return LogLevel::info;
            case spdlog::level::notice:
                return LogLevel::notice;
            case spdlog::level::warn:
                return LogLevel::warn;
            case spdlog::level::err:
                return LogLevel::error;
            case spdlog::level::critical:
            case spdlog::level::alert:
            case spdlog::level::emerg:
                return LogLevel::critical;
            default:
                return LogLevel::critical;
            }
        }

    } // namespace log
} // namespace util
} // namespace osvr

#endif // INCLUDED_LogLevelTranslate_h_GUID_24C67818_0BC8_41B9_7003_1098631ED86F
