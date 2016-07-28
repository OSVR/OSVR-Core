/** @file
    @brief Header to include for OSVR-internal usage of the logging mechanism:
   provides the needed definition of Logger (and includes its dependencies)
   that is only forward-declared in <osvr/Util/Log.h>

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
#include <osvr/Util/Log.h> // for LoggerPtr
#include <osvr/Util/LineLogger.h>
#include <osvr/Util/LogLevel.h>

// Library/third-party includes
// - none

// Standard includes
#include <initializer_list>
#include <memory> // for std::shared_ptr
#include <string> // for std::string

// Forward declarations

namespace spdlog {
class logger;
namespace sinks {
    class sink;
} // namespace sinks

using sink_ptr = std::shared_ptr<spdlog::sinks::sink>;
using sinks_init_list = std::initializer_list<sink_ptr>;
} // namespace spdlog

namespace osvr {
namespace util {
    namespace log {

        /**
         * @brief An object allowing you to log messages with a given log source
         * name.
         *
         * Implemented as a wrapper around the spdlog::logger class.
         */
        class Logger {
          public:
#if 0
            /// Factory function: retrieves from the spdlog registry by name.
            OSVR_UTIL_EXPORT static LoggerPtr
            getFromSpdlogByName(const std::string &logger_name);
#endif

            /// Construct from an existing spdlog logger
            OSVR_UTIL_EXPORT Logger(std::shared_ptr<spdlog::logger> logger);

            /// Construct with a name and an existing, single spdlog sink. (Does
            /// not use any logger registry.)
            OSVR_UTIL_EXPORT static LoggerPtr
            makeWithSink(std::string const &name, spdlog::sink_ptr sink);

            /// Construct with a name and an initializer list of existing spdlog
            /// sinks. (Does not use any logger registry.)
            OSVR_UTIL_EXPORT static LoggerPtr
            makeWithSinks(std::string const &name,
                          spdlog::sinks_init_list sinks);

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

    } // namespace log
} // namespace util
} // namespace osvr

#endif // INCLUDED_Logger_h_GUID_D8ADC0E7_A358_4FF2_960F_10F098A22F4E
