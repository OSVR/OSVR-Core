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

#ifndef INCLUDED_Logger_h_GUID_D8ADC0E7_A358_4FF2_960F_10F098A22F4E
#define INCLUDED_Logger_h_GUID_D8ADC0E7_A358_4FF2_960F_10F098A22F4E

// Internal Includes
#include <osvr/Util/Export.h>
#include <osvr/Util/LineLogger.h>
#include <osvr/Util/LogLevel.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory> // for std::shared_ptr
#include <string> // for std::string
#include <vector> // for std::vector

// Forward declarations

namespace spdlog {
class logger;
} // end namespace spdlog

namespace osvr {
namespace util {
    namespace log {
        class Logger;

        typedef std::shared_ptr<Logger> LoggerPtr;

        /**
         * @brief A wrapper around the spdlog::logger class.
         */
        class Logger {
          public:
            /// Factory function: retrieves from the spdlog registry by name.
            OSVR_UTIL_EXPORT static LoggerPtr
            getFromSpdlogByName(const std::string &logger_name);

            /// Construct from an existing spdlog logger
            OSVR_UTIL_EXPORT Logger(std::shared_ptr<spdlog::logger> logger);

            /// Non-copyable
            OSVR_UTIL_EXPORT Logger(const Logger &) = delete;

            /// Non-copy-assignable
            OSVR_UTIL_EXPORT Logger &operator=(const Logger &) = delete;

            /// Get the minimum level at which this logger will actually forward
            /// messages on to the sinks.
            OSVR_UTIL_EXPORT LogLevel getLogLevel() const;
            /// Set the minimum level at which this logger will actually forward
            /// messages on to the sinks.
            OSVR_UTIL_EXPORT void setLogLevel(LogLevel level);

            /// Set the log level at which this logger will trigger a flush.
            OSVR_UTIL_EXPORT void flushOn(LogLevel level);

            /// @name logger->info(msg) (with optional << "more message") call style
			/// @{
            OSVR_UTIL_EXPORT detail::LineLogger trace(const char *msg);
            OSVR_UTIL_EXPORT detail::LineLogger debug(const char *msg);
            OSVR_UTIL_EXPORT detail::LineLogger info(const char *msg);
            OSVR_UTIL_EXPORT detail::LineLogger notice(const char *msg);
            OSVR_UTIL_EXPORT detail::LineLogger warn(const char *msg);
            OSVR_UTIL_EXPORT detail::LineLogger error(const char *msg);
            OSVR_UTIL_EXPORT detail::LineLogger critical(const char *msg);
			/// @}

            /// @name logger->info() << "msg" call style
			/// @{
            OSVR_UTIL_EXPORT detail::LineLogger trace();
            OSVR_UTIL_EXPORT detail::LineLogger debug();
            OSVR_UTIL_EXPORT detail::LineLogger info();
            OSVR_UTIL_EXPORT detail::LineLogger notice();
            OSVR_UTIL_EXPORT detail::LineLogger warn();
            OSVR_UTIL_EXPORT detail::LineLogger error();
            OSVR_UTIL_EXPORT detail::LineLogger critical();
			/// @}

            /// logger.log(log_level, msg) << ".." call style
            OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level,
                                                    const char *msg);

            /// logger.log(log_level) << "msg" call  style
            OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level);

            OSVR_UTIL_EXPORT void flush();

          private:
            std::shared_ptr<spdlog::logger> logger_;
        };

    } // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_Logger_h_GUID_D8ADC0E7_A358_4FF2_960F_10F098A22F4E
