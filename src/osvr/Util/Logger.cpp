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

// Library/third-party includes
#include <spdlog/spdlog.h>

// Standard includes
#include <memory>           // for std::shared_ptr
#include <utility>          // for std::forward
#include <string>           // for std::string

namespace osvr {
namespace util {
namespace log {

Logger::Logger(const std::string& logger_name)
{
    logger_ = spdlog::get(logger_name);
}

Logger::Logger(std::shared_ptr<spdlog::logger> logger) : logger_(logger)
{
    // do nothing
}

Logger::~Logger()
{
    // FIXME
}

/*
Logger::Logger(spdlog::logger* logger) : logger_(logger)
{
    // do nothing
}
*/

LogLevel Logger::getLogLevel() const
{
    return static_cast<LogLevel>(logger_->level());
}

void Logger::setLogLevel(LogLevel level)
{
    logger_->set_level(static_cast<spdlog::level::level_enum>(level));
}

void Logger::flush_on(LogLevel level)
{
    logger_->flush_on(static_cast<spdlog::level::level_enum>(level));
}

detail::LineLogger Logger::trace(const char* fmt)
{
    return logger_->trace(fmt);
}

detail::LineLogger Logger::debug(const char* fmt)
{
    return logger_->debug(fmt);
}

detail::LineLogger Logger::info(const char* fmt)
{
    return logger_->info(fmt);
}

detail::LineLogger Logger::notice(const char* fmt)
{
    return logger_->notice(fmt);
}

detail::LineLogger Logger::warn(const char* fmt)
{
    return logger_->warn(fmt);
}

detail::LineLogger Logger::error(const char* fmt)
{
    return logger_->error(fmt);
}

detail::LineLogger Logger::critical(const char* fmt)
{
    return logger_->critical(fmt);
}

detail::LineLogger Logger::alert(const char* fmt)
{
    return logger_->alert(fmt);
}

detail::LineLogger Logger::emerg(const char* fmt)
{
    return logger_->emerg(fmt);
}


// logger.info() << ".." call  style
detail::LineLogger Logger::trace()
{
    return logger_->trace();
}

detail::LineLogger Logger::debug()
{
    return logger_->debug();
}

detail::LineLogger Logger::info()
{
    return logger_->info();
}

detail::LineLogger Logger::notice()
{
    return logger_->notice();
}

detail::LineLogger Logger::warn()
{
    return logger_->warn();
}

detail::LineLogger Logger::error()
{
    return logger_->error();
}

detail::LineLogger Logger::critical()
{
    return logger_->critical();
}

detail::LineLogger Logger::alert()
{
    return logger_->alert();
}

detail::LineLogger Logger::emerg()
{
    return logger_->emerg();
}

// logger.log(log_level, msg) << ".." call style
detail::LineLogger Logger::log(LogLevel level, const char* msg)
{
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
    case LogLevel::err:
        return error(msg);
    case LogLevel::critical:
        return critical(msg);
    case LogLevel::alert:
        return alert(msg);
    case LogLevel::emerg:
        return emerg(msg);
    }

    return info(msg);
}

// logger.log(log_level) << ".." call  style
detail::LineLogger Logger::log(LogLevel level)
{
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
    case LogLevel::err:
        return error();
    case LogLevel::critical:
        return critical();
    case LogLevel::alert:
        return alert();
    case LogLevel::emerg:
        return emerg();
    }

    return info();
}

void Logger::flush()
{
    logger_->flush();
}

} // end namespace log
} // end namespace util
} // end namespace osvr

