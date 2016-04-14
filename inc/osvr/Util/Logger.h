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
#include <osvr/Util/LogLevel.h>
#include <osvr/Util/LineLogger.h>
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>           // for std::string
#include <memory>           // for std::shared_ptr
#include <vector>           // for std::vector

// Forward declarations

namespace spdlog {
class logger;
} // end namespace spdlog

namespace osvr {
namespace util {
namespace log {

    /**
     * @brief A wrapper around the spdlog::logger class.
     */
    class Logger {
      public:
        OSVR_UTIL_EXPORT Logger(const std::string &logger_name);
        OSVR_UTIL_EXPORT Logger(spdlog::logger *logger);
        OSVR_UTIL_EXPORT Logger(std::shared_ptr<spdlog::logger> logger);

        OSVR_UTIL_EXPORT virtual ~Logger();
        OSVR_UTIL_EXPORT Logger(const Logger &) = delete;
        OSVR_UTIL_EXPORT Logger &operator=(const Logger &) = delete;

        OSVR_UTIL_EXPORT LogLevel getLogLevel() const;
        OSVR_UTIL_EXPORT void setLogLevel(LogLevel);

        // logger.info(cppformat_string, arg1, arg2, arg3, ...) call style
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger trace(const char *fmt,
                                                  Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger debug(const char *fmt,
                                                  Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger info(const char *fmt,
                                                 Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger notice(const char *fmt,
                                                   Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger warn(const char *fmt,
                                                 Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger error(const char *fmt,
                                                  Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger critical(const char *fmt,
                                                     Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger alert(const char *fmt,
                                                  Args &&... args);
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger emerg(const char *fmt,
                                                  Args &&... args);

        OSVR_UTIL_EXPORT detail::LineLogger trace(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger debug(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger info(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger notice(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger warn(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger error(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger critical(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger alert(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger emerg(const char *fmt);

        // logger.info(msg) << ".." call style
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger trace(T &&msg);
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger debug(T &&msg);
        template <typename T> OSVR_UTIL_EXPORT detail::LineLogger info(T &&msg);
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger notice(T &&msg);
        template <typename T> OSVR_UTIL_EXPORT detail::LineLogger warn(T &&msg);
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger error(T &&msg);
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger critical(T &&msg);
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger alert(T &&msg);
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger emerg(T &&msg);

        // logger.info() << ".." call  style
        OSVR_UTIL_EXPORT detail::LineLogger trace();
        OSVR_UTIL_EXPORT detail::LineLogger debug();
        OSVR_UTIL_EXPORT detail::LineLogger info();
        OSVR_UTIL_EXPORT detail::LineLogger notice();
        OSVR_UTIL_EXPORT detail::LineLogger warn();
        OSVR_UTIL_EXPORT detail::LineLogger error();
        OSVR_UTIL_EXPORT detail::LineLogger critical();
        OSVR_UTIL_EXPORT detail::LineLogger alert();
        OSVR_UTIL_EXPORT detail::LineLogger emerg();

        // Logger.log(log_level, cppformat_string, arg1, arg2, arg3, ...) call
        // style
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level, const char *fmt,
                                                Args &&... args);

        // logger.log(log_level, msg) << ".." call style
        OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level,
                                                const char *msg);

        // logger.log(log_level, msg) << ".." call style
        template <typename T>
        OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level, T &&msg);

        // logger.log(log_level) << ".." call  style
        OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level);

        // Create log message with the given level, no matter what is the actual
        // logger's level
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger
        force_log(LogLevel level, const char *fmt, Args &&... args);

        OSVR_UTIL_EXPORT virtual void flush();

      protected:
        std::shared_ptr<spdlog::logger> logger_;
    };

    typedef std::shared_ptr<Logger> LoggerPtr;

} // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_Logger_h_GUID_D8ADC0E7_A358_4FF2_960F_10F098A22F4E
