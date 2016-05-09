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
#include <osvr/Util/Log.h>
#include <osvr/Util/Logger.h>
#include <osvr/Util/LogLevel.h>
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <boost/filesystem.hpp>

// Standard includes
// - none

namespace osvr {
namespace util {
namespace log {

LogRegistry& LogRegistry::instance()
{
    static LogRegistry instance_;
    return instance_;
}

LoggerPtr LogRegistry::getOrCreateLogger(const std::string& logger_name)
{
    // If logger already exists, return a copy of it
    auto spd_logger = spdlog::get(logger_name);
    if (spd_logger)
        return std::make_shared<Logger>(spd_logger);

    // Bummer, it didn't exist. We'll create one from scratch.

    spd_logger = spdlog::details::registry::instance().create(logger_name, begin(sinks_), end(sinks_));

    spd_logger->set_pattern("%b %d %T.%e %l [%n]: %v");
    spd_logger->set_level(spdlog::level::trace);
    spd_logger->flush_on(spdlog::level::err);

    return std::make_shared<Logger>(spd_logger);
}

void setPattern(const std::string& pattern)
{
    spdlog::set_pattern(pattern.c_str());
}

void setLevel(LogLevel severity)
{
    spdlog::set_level(static_cast<spdlog::level::level_enum>(severity));
}

LogRegistry::LogRegistry() : sinks_()
{
    // Set default pattern and level
    spdlog::set_pattern("%b %d %T.%e %l [%n]: %v");
    spdlog::set_level(static_cast<spdlog::level::level_enum>(LogLevel::info));

    // Instantiate console and file sinks

    // Console sink
    auto console_out = spdlog::sinks::stderr_sink_mt::instance();
    auto color_sink = std::make_shared<spdlog::sinks::ansicolor_sink>(console_out); // taste the rainbow!
#if defined(OSVR_LINUX) || defined(OSVR_MACOSX)
    sinks_.push_back(color_sink);
#elif defined(OSVR_ANDROID)
    auto android_sink = spdlog::sinks::android_sink_mt("OSVR");
    sinks_.push_back(android_sink);
#else
    // No color for Windows yet (and not tested on other platforms)
    sinks_.push_back(console_out);
#endif

    // File sink - rotates daily
    size_t q_size = 1048576; // queue size must be power of 2
    spdlog::set_async_mode(q_size);
    namespace fs = boost::filesystem;
    auto base_name = fs::path(getLoggingDirectory(true));
    if (!base_name.empty()) {
        base_name /= "osvr";
        auto daily_file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(base_name.string().c_str(), "log", 0, 0, false);
        sinks_.push_back(daily_file_sink);
    }
}

LogRegistry::~LogRegistry()
{
    // do nothing
}

} // end namespace log
} // end namespace util
} // end namespace osvr

