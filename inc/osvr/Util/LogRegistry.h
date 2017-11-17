/** @file
    @brief Regstry to maintain instantiated loggers and global settings.

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

#ifndef INCLUDED_LogRegistry_h_GUID_09DDD840_389E_430C_8CBD_9AC4EE3F93FE
#define INCLUDED_LogRegistry_h_GUID_09DDD840_389E_430C_8CBD_9AC4EE3F93FE

// Internal Includes
#include <osvr/Util/Log.h> // for LoggerPtr forward declaration
#include <osvr/Util/LogLevel.h>
#include <osvr/Util/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory> // for std::shared_ptr
#include <string> // for std::string
#include <vector> // for std::vector

// Forward declarations
namespace spdlog {

namespace sinks {
    class sink;
    class filter_sink;
} // namespace sinks

using sink_ptr = std::shared_ptr<spdlog::sinks::sink>;

} // namespace spdlog

namespace osvr {
namespace util {
    namespace log {
        class filter_sink;

        class LogRegistry {
          public:
            LogRegistry(LogRegistry const &) = delete; // copy construct
            LogRegistry(LogRegistry &&) = delete;      // move construct
            LogRegistry &operator=(LogRegistry const &) = delete; // copy assign
            LogRegistry &operator=(LogRegistry &&) = delete;      // move assign

            OSVR_UTIL_EXPORT static LogRegistry &instance(std::string const * = nullptr);

            /**
             * @brief Gets or creates a logger named @c logger_name.
             *
             * If the logger named @c logger_name already exists, return
             * it, otherwise create a new logger of that name.
             *
             * @param logger_name The name of the logger.
             *
             */
            OSVR_UTIL_EXPORT LoggerPtr getOrCreateLogger(const std::string &logger_name);

            /**
             * @brief Drops a logger from the registry.
             *
             * The logger will survive until the last copy of it is destroyed
             * (e.g., goes out of scope). This function is useful if you want to
             * destroy a logger before the program terminates.
             */
            OSVR_UTIL_EXPORT void drop(const std::string &name);

            /**
             * @brief Removes all the registered loggers from the registry.
             *
             * Each logger will survive until the last copy of it is destroyed
             * (e.g., goes out of scope).
             */
            OSVR_UTIL_EXPORT void dropAll();

            /**
             * @brief Flush all sinks manually.
             */
            OSVR_UTIL_EXPORT void flush();

            /**
             * @brief Sets the output pattern on all registered loggers.
             */
            OSVR_UTIL_EXPORT void setPattern(const std::string &pattern);

            /**
             * @brief Sets the minimum level of messages to be logged on all
             * registered loggers.
             */
            OSVR_UTIL_EXPORT void setLevel(LogLevel severity);

            /**
             * @brief Sets the minimum level of messages to be logged to the
             * console.
             */
            OSVR_UTIL_EXPORT void setConsoleLevel(LogLevel severity);

            std::string const &getLogFileBaseName() const {
                return logFileBaseName_;
            }

            bool couldOpenLogFile() const { return sinks_.size() > 1; }

          protected:
            OSVR_UTIL_EXPORT LogRegistry(std::string const &logFileBaseName);
            OSVR_UTIL_EXPORT ~LogRegistry();

          private:
            void setLevelImpl(LogLevel severity);
            void setConsoleLevelImpl(LogLevel severity);
            void createFileSink();
            LogLevel minLevel_;
            LogLevel consoleLevel_;

            std::vector<spdlog::sink_ptr> sinks_;
            std::shared_ptr<filter_sink> console_filter_;
            LoggerPtr consoleOnlyLog_;
            LoggerPtr generalLog_;
            Logger *generalPurposeLog_ = nullptr;
            std::string logFileBaseName_;
        };

    } // namespace log
} // namespace util
} // namespace osvr

#endif // INCLUDED_LogRegistry_h_GUID_09DDD840_389E_430C_8CBD_9AC4EE3F93FE
