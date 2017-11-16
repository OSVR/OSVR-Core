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
#include <osvr/Util/LogLevel.h>

// Library/third-party includes
// - none

// Standard includes
#include <initializer_list>
#include <memory>  // for std::shared_ptr
#include <sstream> // for std::ostringstream
#include <string>  // for std::string

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
          private:
            struct PrivateConstructor;

          public:
            /// Internal-use constructor - please used a factory/named
            /// constructor.
            Logger(std::string const &name,
                   std::shared_ptr<spdlog::logger> logger,
                   PrivateConstructor *);

            /// Create from existing spdlog (implementation) logger.
            ///
            /// Always returns a valid pointer even on invalid input, though it
            /// may be a "fallback" logger.
            OSVR_UTIL_EXPORT static LoggerPtr makeFromExistingImplementation(
                std::string const &name,
                std::shared_ptr<spdlog::logger> logger);

            /// Construct with a name and an existing, single spdlog sink. (Does
            /// not use any logger registry.)
            ///
            /// Always returns a valid pointer even on invalid input, though it
            /// may be a "fallback" logger.
            OSVR_UTIL_EXPORT static LoggerPtr
            makeWithSink(std::string const &name, spdlog::sink_ptr sink);

            /// Construct with a name and an initializer list of existing spdlog
            /// sinks. (Does not use any logger registry.)
            ///
            /// Always returns a valid pointer even on invalid input, though it
            /// may be a "fallback" logger.
            OSVR_UTIL_EXPORT static LoggerPtr
            makeWithSinks(std::string const &name,
                          spdlog::sinks_init_list sinks);

            /// Non-copyable
            Logger(const Logger &) = delete;

            /// Non-copy-assignable
            Logger &operator=(const Logger &) = delete;

            /// Destructor
            OSVR_UTIL_EXPORT ~Logger();

            /// Get the minimum level at which this logger will actually forward
            /// messages on to the sinks.
            OSVR_UTIL_EXPORT LogLevel getLogLevel() const;

            /// Set the minimum level at which this logger will actually forward
            /// messages on to the sinks.
            OSVR_UTIL_EXPORT void setLogLevel(LogLevel level);

            /// Set the log level at which this logger will trigger a flush.
            OSVR_UTIL_EXPORT void flushOn(LogLevel level);

            /// An object returned the logging functions (including operator<<),
            /// serves to accumulate streamed output in a single ostringstream
            /// then write it to the logger at the end of the expression's
            /// lifetime.
            class StreamProxy {
              public:
                StreamProxy(Logger &logger, LogLevel level)
                    : logger_(logger), level_(level),
                      os_(new std::ostringstream) {}

                StreamProxy(Logger &logger, LogLevel level,
                            const std::string &msg)
                    : logger_(logger), level_(level),
                      os_(new std::ostringstream) {
                    (*os_) << msg;
                }

                /// destructor appends the finished stringstream at the end
                /// of the expression.
                ~StreamProxy() {
                    if (active_ && os_) {
                        logger_.write(level_, os_->str().c_str());
                    }
                }

                /// move construction
                StreamProxy(StreamProxy &&other)
                    : logger_(other.logger_), level_(other.level_),
                      os_(std::move(other.os_)), active_(other.active_) {
                    other.active_ = false;
                }

                StreamProxy(StreamProxy const &) = delete;
                StreamProxy &operator=(StreamProxy const &) = delete;

                operator std::ostream &() { return (*os_); }

                template <typename T> std::ostream &operator<<(T &&what) {
                    (*os_) << std::forward<T>(what);
                    return (*os_);
                }

              private:
                Logger &logger_;
                LogLevel level_;
                std::unique_ptr<std::ostringstream> os_;
                bool active_ = true;
            };

            /// @name logger->info(msg) (with optional << "more message") call
            /// style
            /// @{
            OSVR_UTIL_EXPORT StreamProxy trace(const char *msg);
            OSVR_UTIL_EXPORT StreamProxy debug(const char *msg);
            OSVR_UTIL_EXPORT StreamProxy info(const char *msg);
            OSVR_UTIL_EXPORT StreamProxy notice(const char *msg);
            OSVR_UTIL_EXPORT StreamProxy warn(const char *msg);
            OSVR_UTIL_EXPORT StreamProxy error(const char *msg);
            OSVR_UTIL_EXPORT StreamProxy critical(const char *msg);
            /// @}

            /// @name logger->info() << "msg" call style
            /// @{
            OSVR_UTIL_EXPORT StreamProxy trace();
            OSVR_UTIL_EXPORT StreamProxy debug();
            OSVR_UTIL_EXPORT StreamProxy info();
            OSVR_UTIL_EXPORT StreamProxy notice();
            OSVR_UTIL_EXPORT StreamProxy warn();
            OSVR_UTIL_EXPORT StreamProxy error();
            OSVR_UTIL_EXPORT StreamProxy critical();
            /// @}

            /// logger.log(log_level, msg) (with optional << "more message")
            /// call style
            OSVR_UTIL_EXPORT StreamProxy log(LogLevel level, const char *msg);

            /// logger.log(log_level) << "msg" call  style
            OSVR_UTIL_EXPORT StreamProxy log(LogLevel level);

            /// Make sure this logger has written out its data.
            OSVR_UTIL_EXPORT void flush();

            /// Get the logger name
            std::string const &getName() const {
                return name_;
            }

          private:
            static LoggerPtr
            makeLogger(std::string const &name,
                       std::shared_ptr<spdlog::logger> const &logger);

            /// In case a spdlog logger is not available, this will create a
            /// fallback logger instance using just ostream.
            static LoggerPtr makeFallback(std::string const &name);

            /// Pass the constructed message along to the underlying logger.
            OSVR_UTIL_EXPORT void write(LogLevel level, const char* msg);

            const std::string name_;
            std::shared_ptr<spdlog::logger> logger_;
        };

    } // namespace log
} // namespace util
} // namespace osvr

#endif // INCLUDED_Logger_h_GUID_D8ADC0E7_A358_4FF2_960F_10F098A22F4E
