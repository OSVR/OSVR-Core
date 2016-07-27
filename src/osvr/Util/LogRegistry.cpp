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

// Internal Includes
#include <osvr/Util/LogRegistry.h>

#include "LogDefaults.h"
#include "LogLevelTranslate.h"

#include <osvr/Util/Log.h>
#include <osvr/Util/LogLevel.h>
#include <osvr/Util/Logger.h>
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#include <boost/filesystem.hpp>
#include <spdlog/common.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/spdlog.h>

// Standard includes
#include <iostream>
#include <utility>

namespace spdlog {
namespace sinks {
    /// A decorator around another sink that applies a custom log level filter.
    class filter_sink : public sink {
      public:
        filter_sink(sink_ptr wrapped_sink, level::level_enum filter_level);
        virtual ~filter_sink() {}
        void log(const details::log_msg &msg) override;
        void flush() override { sink_->flush(); }
        void set_level(level::level_enum filter_level);

      private:
        sink_ptr sink_;
        level::level_enum level_;
    };
    inline filter_sink::filter_sink(sink_ptr wrapped_sink,
                                    level::level_enum filter_level)
        : sink_(std::move(wrapped_sink)), level_(filter_level) {}

    inline void filter_sink::log(const details::log_msg &msg) {
        if (msg.level < level_) {
            return;
        }
        sink_->log(msg);
    }

    void filter_sink::set_level(level::level_enum filter_level) {
        level_ = filter_level;
    }

} // namespace sinks
} // namespace spdlog

namespace osvr {
namespace util {
    namespace log {
        LogRegistry &LogRegistry::instance() {
            static LogRegistry instance_;
            return instance_;
        }

        LoggerPtr
        LogRegistry::getOrCreateLogger(const std::string &logger_name) {
            // If logger already exists, return a copy of it
            auto spd_logger = spdlog::get(logger_name);
            if (!spd_logger) {
                // Bummer, it didn't exist. We'll create one from scratch.
                spd_logger = spdlog::details::registry::instance().create(
                    logger_name, begin(sinks_), end(sinks_));
            }

            spd_logger->set_pattern(DEFAULT_PATTERN);
            spd_logger->set_level(convertToLevelEnum(DEFAULT_LEVEL));
            spd_logger->flush_on(convertToLevelEnum(DEFAULT_FLUSH_LEVEL));

            return std::make_shared<Logger>(spd_logger);
        }

        void LogRegistry::flush() {
            for (auto &sink : sinks_) {
                sink->flush();
            }
        }

        void LogRegistry::setPattern(const std::string &pattern) {
            spdlog::set_pattern(pattern.c_str());
        }

        void LogRegistry::setLevel(LogLevel severity) {
            spdlog::set_level(convertToLevelEnum(severity));
        }

        static inline spdlog::sink_ptr getConsoleSink() {
            // Console sink
            auto console_out = spdlog::sinks::stderr_sink_mt::instance();
#if defined(OSVR_LINUX) || defined(OSVR_MACOSX)
            auto color_sink = std::make_shared<spdlog::sinks::ansicolor_sink>(
                console_out); // taste the rainbow!
            return color_sink;
#else
            // No color for Windows yet (and not tested on other platforms)
            return console_out;
#endif
        }

        void LogRegistry::setConsoleLevel(LogLevel severity) {
            if (console_filter_) {
                console_filter_->set_level(convertToLevelEnum(severity));
            }
        }

        LogRegistry::LogRegistry() : sinks_() {
            // Set default pattern and level
            spdlog::set_pattern(DEFAULT_PATTERN);
            spdlog::set_level(convertToLevelEnum(DEFAULT_LEVEL));

// Instantiate console and file sinks

#if defined(OSVR_ANDROID)
            // Android doesn't have a console, it has logcat.
            auto android_sink = spdlog::sinks::android_sink_mt("OSVR");
            sinks_.push_back(android_sink);
#else
            // Console sink
            auto console_sink = getConsoleSink();
            console_filter_ = std::make_shared<spdlog::sinks::filter_sink>(
                console_sink, convertToLevelEnum(DEFAULT_CONSOLE_LEVEL));
            sinks_.push_back(console_filter_);
#endif

            // File sink - rotates daily
            try {
                size_t q_size = 1048576; // queue size must be power of 2
                spdlog::set_async_mode(q_size);
                namespace fs = boost::filesystem;
                auto base_name = fs::path(getLoggingDirectory(true));
                if (!base_name.empty()) {
                    base_name /= "osvr";
                    auto daily_file_sink =
                        std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                            base_name.string().c_str(), "log", 0, 0, false);
                    sinks_.push_back(daily_file_sink);
                }
            } catch (const std::exception &e) {
                std::cerr << "[OSVR] Error creating log file sink: " << e.what()
                          << ". Will log to console only." << std::endl;
            } catch (...) {
                std::cerr << "[OSVR] Error creating log file sink. Will log to "
                             "console only."
                          << std::endl;
            }
        }

        LogRegistry::~LogRegistry() {
            // do nothing but flush
            flush();
        }

    } // end namespace log
} // end namespace util
} // end namespace osvr
