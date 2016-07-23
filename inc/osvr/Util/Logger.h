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
        OSVR_UTIL_EXPORT Logger(std::shared_ptr<spdlog::logger> logger);

        OSVR_UTIL_EXPORT virtual ~Logger();
        OSVR_UTIL_EXPORT Logger(const Logger &) = delete;
        OSVR_UTIL_EXPORT Logger &operator=(const Logger &) = delete;

        OSVR_UTIL_EXPORT LogLevel getLogLevel() const;
        OSVR_UTIL_EXPORT void setLogLevel(LogLevel level);

        OSVR_UTIL_EXPORT void flushOn(LogLevel level);
#if 0
        // These functions are not yet implemented because they expose the
        // underlying spdlog classes.

        // Logger.info(cppformat_string, arg1, arg2, arg3, ...) call style
        template <typename... Args> detail::LineLogger Logger::trace(const char* fmt, Args&&... args)
        {
            return logger_->trace(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::debug(const char* fmt, Args&&... args)
        {
            return logger_->debug(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::info(const char* fmt, Args&&... args)
        {
            return logger_->info(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::notice(const char* fmt, Args&&... args)
        {
            return logger_->notice(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::warn(const char* fmt, Args&&... args)
        {
            return logger_->warn(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::error(const char* fmt, Args&&... args)
        {
            return logger_->error(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::critical(const char* fmt, Args&&... args)
        {
            return logger_->critical(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::alert(const char* fmt, Args&&... args)
        {
            return logger_->alert(fmt, std::forward<Args>(args)...);
        }

        template <typename... Args> detail::LineLogger Logger::emerg(const char* fmt, Args&&... args)
        {
            return logger_->emerg(fmt, std::forward<Args>(args)...);
        }
#endif

        // logger.info(msg) ".." call style
        OSVR_UTIL_EXPORT detail::LineLogger trace(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger debug(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger info(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger notice(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger warn(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger error(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger critical(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger alert(const char *fmt);
        OSVR_UTIL_EXPORT detail::LineLogger emerg(const char *fmt);

#if 0
        // These functions are not yet implemented because they expose the
        // underlying spdlog classes.

        // logger.info(msg) << ".." call style
        template <typename T> detail::LineLogger trace(T&& msg)
        {
            return logger_->trace(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger debug(T&& msg)
        {
            return logger_->debug(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger info(T&& msg)
        {
            return logger_->info(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger notice(T&& msg)
        {
            return logger_->notice(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger warn(T&& msg)
        {
            return logger_->warn(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger error(T&& msg)
        {
            return logger_->error(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger critical(T&& msg)
        {
            return logger_->critical(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger alert(T&& msg)
        {
            return logger_->alert(std::forward<T>(msg));
        }

        template <typename T> detail::LineLogger emerg(T&& msg)
        {
            return logger_->emerg(std::forward<T>(msg));
        }
#endif

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

#if 0
        // These functions are not yet implemented because they expose the
        // underlying spdlog classes.

        // Logger.log(log_level, cppformat_string, arg1, arg2, arg3, ...) call
        // style
        template <typename... Args>
        OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level, const char *fmt,
                                                Args &&... args)
        {
            switch (level) {
            case LogLevel::trace:
                return trace(fmt, std::forward<Args>(args)...);
            case LogLevel::debug:
                return debug(fmt, std::forward<Args>(args)...);
            case LogLevel::info:
                return info(fmt, std::forward<Args>(args)...);
            case LogLevel::notice:
                return notice(fmt, std::forward<Args>(args)...);
            case LogLevel::warn:
                return warn(fmt, std::forward<Args>(args)...);
            case LogLevel::err:
                return error(fmt, std::forward<Args>(args)...);
            case LogLevel::critical:
                return critical(fmt, std::forward<Args>(args)...);
            case LogLevel::alert:
                return alert(fmt, std::forward<Args>(args)...);
            case LogLevel::emerg:
                return emerg(fmt, std::forward<Args>(args)...);
            }
        }
#endif

        // logger.log(log_level, msg) << ".." call style
        OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level,
                                                const char *msg);

#if 0
        // These functions are not yet implemented because they expose the
        // underlying spdlog classes.

        // logger.log(log_level, msg) << ".." call style
        template <typename T>
        detail::LineLogger log(LogLevel level, T&& msg)
        {
            switch (level) {
            case LogLevel::trace:
                return trace(std::forward<T>(msg));
            case LogLevel::debug:
                return debug(std::forward<T>(msg));
            case LogLevel::info:
                return info(std::forward<T>(msg));
            case LogLevel::notice:
                return notice(std::forward<T>(msg));
            case LogLevel::warn:
                return warn(std::forward<T>(msg));
            case LogLevel::err:
                return error(std::forward<T>(msg));
            case LogLevel::critical:
                return critical(std::forward<T>(msg));
            case LogLevel::alert:
                return alert(std::forward<T>(msg));
            case LogLevel::emerg:
                return emerg(std::forward<T>(msg));
            }

            return info(std::forward<T>(msg));
        }
#endif

        // logger.log(log_level) << ".." call  style
        OSVR_UTIL_EXPORT detail::LineLogger log(LogLevel level);

#if 0
        // These functions are not yet implemented because they expose the
        // underlying spdlog classes.

        // Create log message with the given level, no matter what is the actual
        // logger's level
        template <typename... Args>
        detail::LineLogger force_log(LogLevel level, const char* fmt, Args&&... args)
        {
            auto lvl = static_cast<spdlog::level::level_enum>(level);
            return logger_->force_log(lvl, fmt, std::forward<Args>(args)...);
        }
#endif

        OSVR_UTIL_EXPORT virtual void flush();

      protected:
        std::shared_ptr<spdlog::logger> logger_;
    };

    typedef std::shared_ptr<Logger> LoggerPtr;

} // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_Logger_h_GUID_D8ADC0E7_A358_4FF2_960F_10F098A22F4E
