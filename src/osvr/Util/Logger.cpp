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
#include <memory>  // for std::shared_ptr
#include <string>  // for std::string
#include <utility> // for std::forward

namespace osvr {
namespace util {
    namespace log {
#if 0
        LoggerPtr Logger::getFromSpdlogByName(const std::string &logger_name) {
			auto spdlogPtr = spdlog::get(logger_name);
			if (!spdlogPtr) {
				return LoggerPtr();
			}
            return std::make_shared<Logger>(spdlogPtr);
        }
#endif

        Logger::Logger(std::shared_ptr<spdlog::logger> logger)
            : logger_(logger) {}

        LoggerPtr Logger::makeWithSink(std::string const &name,
                                       spdlog::sink_ptr sink) {
            if (!sink) {
				// bad sink!
                return LoggerPtr();
            }
            auto spdlogPtr = std::make_shared<spdlog::logger>(name, sink);
            return std::make_shared<Logger>(spdlogPtr);
        }

        LoggerPtr Logger::makeWithSinks(std::string const &name,
                                        spdlog::sinks_init_list sinks) {
            for (auto & sink : sinks) {
				if (!sink) {
					// got a bad sink
					return LoggerPtr();
				}
			}
            auto spdlogPtr = std::make_shared<spdlog::logger>(name, sinks);
            return std::make_shared<Logger>(spdlogPtr);
		}

        LogLevel Logger::getLogLevel() const {
            return convertFromLevelEnum(logger_->level());
        }

        void Logger::setLogLevel(LogLevel level) {
            logger_->set_level(convertToLevelEnum(level));
        }

        void Logger::flushOn(LogLevel level) {
            logger_->flush_on(convertToLevelEnum(level));
        }

        detail::LineLogger Logger::trace(const char *fmt) {
            return logger_->trace(fmt);
        }

        detail::LineLogger Logger::debug(const char *fmt) {
            return logger_->debug(fmt);
        }

        detail::LineLogger Logger::info(const char *fmt) {
            return logger_->info(fmt);
        }

        detail::LineLogger Logger::notice(const char *fmt) {
            return logger_->notice(fmt);
        }

        detail::LineLogger Logger::warn(const char *fmt) {
            return logger_->warn(fmt);
        }

        detail::LineLogger Logger::error(const char *fmt) {
            return logger_->error(fmt);
        }

        detail::LineLogger Logger::critical(const char *fmt) {
            return logger_->critical(fmt);
        }

        // logger.info() << ".." call  style
        detail::LineLogger Logger::trace() { return logger_->trace(); }

        detail::LineLogger Logger::debug() { return logger_->debug(); }

        detail::LineLogger Logger::info() { return logger_->info(); }

        detail::LineLogger Logger::notice() { return logger_->notice(); }

        detail::LineLogger Logger::warn() { return logger_->warn(); }

        detail::LineLogger Logger::error() { return logger_->error(); }

        detail::LineLogger Logger::critical() { return logger_->critical(); }

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

        void Logger::flush() { logger_->flush(); }

    } // namespace log
} // namespace util
} // namespace osvr
