/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Util/Logger.h>

#include "LogDefaults.h"
#include "LogLevelTranslate.h"

// Library/third-party includes
#include <spdlog/spdlog.h>

// Standard includes
#include <iostream>
#include <memory>  // for std::shared_ptr
#include <string>  // for std::string
#include <utility> // for std::forward

namespace osvr {
namespace util {
    namespace log {

        inline LoggerPtr
        Logger::makeLogger(std::string const &name,
                           std::shared_ptr<spdlog::logger> const &logger) {

#ifdef OSVR_USE_UNIQUEPTR_FOR_LOGGER
            auto ret = LoggerPtr{new Logger{
                name, logger, static_cast<PrivateConstructor *>(nullptr)}};
#else
            auto ret = std::make_shared<Logger>(
                name, logger, static_cast<PrivateConstructor *>(nullptr));
#endif
            return ret;
        }

        Logger::Logger(std::string const &name,
                       std::shared_ptr<spdlog::logger> logger,
                       PrivateConstructor *)
            : name_(name), logger_(std::move(logger)) {}

        Logger::~Logger() {}

        LoggerPtr Logger::makeFallback(std::string const &name) {
            /// Passes an empty spdlog pointer.
            std::cerr << "WARNING: logger created for '" << name
                      << "' is a \"fallback\" logger - an internal error has "
                         "prevented a standard logger from being created. "
                         "Please report this issue in OSVR-Core on GitHub."
                      << std::endl;
            return makeLogger(name, std::shared_ptr<spdlog::logger>{});
        }

        LoggerPtr Logger::makeFromExistingImplementation(
            std::string const &name, std::shared_ptr<spdlog::logger> logger) {
            if (!logger) {
                std::cerr << "WARNING: "
                             "Logger::makeFromExistingImplementation(\""
                          << name << "\", logger) called "
                                     "with a null logger pointer! Will result "
                                     "in a fallback logger!"
                          << std::endl;
                return makeFallback(name);
            }
            return makeLogger(name, logger);
        }

        LoggerPtr Logger::makeWithSink(std::string const &name,
                                       spdlog::sink_ptr sink) {
            if (!sink) {
                // bad sink!
                std::cerr
                    << "WARNING: "
                       "Logger::makeWithSink(\""
                    << name
                    << "\", sink) called "
                       "with a null sink! Will result in a fallback logger!"
                    << std::endl;
                return makeFallback(name);
            }
            auto spd_logger = std::make_shared<spdlog::logger>(name, sink);
            spd_logger->set_pattern(DEFAULT_PATTERN);
            spd_logger->flush_on(convertToLevelEnum(DEFAULT_FLUSH_LEVEL));
            return makeLogger(name, spd_logger);
        }

        LoggerPtr Logger::makeWithSinks(std::string const &name,
                                        spdlog::sinks_init_list sinks) {
            for (auto &sink : sinks) {
                if (!sink) {
                    std::cerr << "WARNING: "
                                 "Logger::makeWithSinks(\""
                              << name << "\", sinks) called "
                                         "with at least one null sink! Will "
                                         "result in a fallback logger!"
                              << std::endl;
                    // got a bad sink
                    /// @todo should we be making a fallback logger here, just
                    /// hoping spdlog will deal with a bad sink pointer without
                    /// issue, or filtering the init list to a non-nullptr
                    /// vector?
                    return makeFallback(name);
                }
            }
            auto spd_logger = std::make_shared<spdlog::logger>(name, sinks);
            spd_logger->set_pattern(DEFAULT_PATTERN);
            spd_logger->flush_on(convertToLevelEnum(DEFAULT_FLUSH_LEVEL));
            return makeLogger(name, spd_logger);
        }

        LogLevel Logger::getLogLevel() const {
            return logger_ ? convertFromLevelEnum(logger_->level())
                           : DEFAULT_LEVEL;
        }

        void Logger::setLogLevel(LogLevel level) {
            if (logger_) {
                logger_->set_level(convertToLevelEnum(level));
            }
        }

        void Logger::flushOn(LogLevel level) {
            if (logger_) {
                logger_->flush_on(convertToLevelEnum(level));
            }
        }

        detail::LineLogger Logger::trace(const char *msg) {
            return logger_ ? detail::LineLogger{logger_->trace(msg)}
                           : detail::LineLogger{name_, LogLevel::trace, msg};
        }

        detail::LineLogger Logger::debug(const char *msg) {
            return logger_ ? detail::LineLogger{logger_->debug(msg)}
                           : detail::LineLogger{name_, LogLevel::debug, msg};
        }

        detail::LineLogger Logger::info(const char *msg) {
            return logger_ ? detail::LineLogger{logger_->info(msg)}
                           : detail::LineLogger{name_, LogLevel::info, msg};
        }

        detail::LineLogger Logger::notice(const char *msg) {
            return logger_ ? detail::LineLogger{logger_->notice(msg)}
                           : detail::LineLogger{name_, LogLevel::notice, msg};
        }

        detail::LineLogger Logger::warn(const char *msg) {
            return logger_ ? detail::LineLogger{logger_->warn(msg)}
                           : detail::LineLogger{name_, LogLevel::warn, msg};
        }

        detail::LineLogger Logger::error(const char *msg) {
            return logger_ ? detail::LineLogger{logger_->error(msg)}
                           : detail::LineLogger{name_, LogLevel::error, msg};
        }

        detail::LineLogger Logger::critical(const char *msg) {
            return logger_ ? detail::LineLogger{logger_->critical(msg)}
                           : detail::LineLogger{name_, LogLevel::critical, msg};
        }

        // logger.info() << ".." call  style
        detail::LineLogger Logger::trace() {
            return logger_ ? detail::LineLogger{logger_->trace()}
                           : detail::LineLogger{name_, LogLevel::trace};
        }

        detail::LineLogger Logger::debug() {
            return logger_ ? detail::LineLogger{logger_->debug()}
                           : detail::LineLogger{name_, LogLevel::debug};
        }

        detail::LineLogger Logger::info() {
            return logger_ ? detail::LineLogger{logger_->info()}
                           : detail::LineLogger{name_, LogLevel::info};
        }

        detail::LineLogger Logger::notice() {
            return logger_ ? detail::LineLogger{logger_->notice()}
                           : detail::LineLogger{name_, LogLevel::notice};
        }

        detail::LineLogger Logger::warn() {
            return logger_ ? detail::LineLogger{logger_->warn()}
                           : detail::LineLogger{name_, LogLevel::warn};
        }

        detail::LineLogger Logger::error() {
            return logger_ ? detail::LineLogger{logger_->error()}
                           : detail::LineLogger{name_, LogLevel::error};
        }

        detail::LineLogger Logger::critical() {
            return logger_ ? detail::LineLogger{logger_->critical()}
                           : detail::LineLogger{name_, LogLevel::critical};
        }

        // logger.log(log_level, msg) << ".." call style
        detail::LineLogger Logger::log(LogLevel level, const char *msg) {
            switch (level) {
            case LogLevel::trace:
                return trace(msg);
            case LogLevel::debug:
                return debug(msg);
            case LogLevel::info:
                return info(msg);
            case LogLevel::notice:
                return notice(msg);
            case LogLevel::warn:
                return warn(msg);
            case LogLevel::error:
                return error(msg);
            case LogLevel::critical:
                return critical(msg);
            }

            return info(msg);
        }

        // logger.log(log_level) << ".." call  style
        detail::LineLogger Logger::log(LogLevel level) {
            switch (level) {
            case LogLevel::trace:
                return trace();
            case LogLevel::debug:
                return debug();
            case LogLevel::info:
                return info();
            case LogLevel::notice:
                return notice();
            case LogLevel::warn:
                return warn();
            case LogLevel::error:
                return error();
            case LogLevel::critical:
                return critical();
            }

            return info();
        }

        void Logger::flush() {
            if (logger_) {
                logger_->flush();
            } else {
                std::cout << std::flush;
                std::cerr << std::flush;
            }
        }

    } // namespace log
} // namespace util
} // namespace osvr
