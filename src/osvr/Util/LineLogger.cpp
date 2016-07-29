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
#include "LogLevelTranslate.h"
#include <osvr/Util/LineLogger.h>

// Library/third-party includes
#include <spdlog/spdlog.h>

// Standard includes
#include <algorithm> // for std::move
#include <iostream>
#include <string>  // for std::string
#include <utility> // for std::forward

namespace osvr {
namespace util {
    namespace log {
        namespace detail {

            LineLogger::LineLogger(spdlog::details::line_logger &&line_logger)
                : lineLogger_(new ::spdlog::details::line_logger(
                      std::move(line_logger))) {
                // do nothing else
            }

            LineLogger::LineLogger(std::string const &name, LogLevel level)
                : name_(&name), level_(level) {
                /// It's safe to hold on to a pointer to the string we're given,
                /// since it comes from a Logger's internal member.
            }

            LineLogger::LineLogger(std::string const &name, LogLevel level,
                                   const char *msg)
                : LineLogger(name, level) {
                write(msg);
            }

            LineLogger::LineLogger(LineLogger &&other)
                : lineLogger_(std::move(other.lineLogger_)), name_(other.name_),
                  level_(other.level_),
                  startedFallback_(other.startedFallback_) {
                /// Turn off the other's "fallback" indicators.
                other.name_ = nullptr;
                other.startedFallback_ = false;
            }

            LineLogger::~LineLogger() {
                if (startedFallback_) {
                    getFallbackStream() << std::endl;
                }
            }

            void LineLogger::write(const char *what) {
                if (lineLogger_) {
                    lineLogger_->write(what);
                    return;
                }
                if (fallbackEnabled()) {
                    startFallbackMessaging() << what;
                }
            }

            void LineLogger::write(const std::string &what) {
                if (lineLogger_) {
                    lineLogger_->operator<<(what);
                    return;
                }
                if (fallbackEnabled()) {
                    startFallbackMessaging() << what;
                }
            }

            void LineLogger::disable() {
                if (lineLogger_) {
                    lineLogger_->disable();
                } else {
                    name_ = nullptr;
                }
            }

            bool LineLogger::is_enabled() const {
                return (lineLogger_ && lineLogger_->is_enabled()) ||
                       fallbackEnabled();
            }

            std::ostream &LineLogger::startFallbackMessaging() {
                auto &os = getFallbackStream();

                if (!startedFallback_) {
                    // Write out an abbreviated beginning-of-line log header.
                    os << spdlog::level::to_str(convertToLevelEnum(level_))
                       << " [" << *name_ << "] ";
                    startedFallback_ = true;
                }

                return os;
            }

            std::ostream &LineLogger::getFallbackStream() const {
                return (level_ > LogLevel::notice) ? std::cerr : std::cout;
            }

        } // end namespace detail
    }     // end namespace log
} // end namespace util
} // end namespace osvr
