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
#include <osvr/Util/GetEnvironmentVariable.h>

#include "LogDefaults.h"
#include "LogLevelTranslate.h"
#include "LogSinks.h"
#include "LogUtils.h"

#include <osvr/Util/BinaryLocation.h>
#include <osvr/Util/Log.h>
#include <osvr/Util/LogLevel.h>
#include <osvr/Util/LogNames.h>
#include <osvr/Util/Logger.h>
#include <osvr/Util/PlatformConfig.h>

// Library/third-party includes
#include <boost/filesystem.hpp>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

// Standard includes
#include <iostream>
#include <utility>

namespace osvr {
namespace util {
    namespace log {

        static const auto LOG_FILE_BASENAME = "osvr";

        static const auto LOG_FILE_EXTENSION = "log";

        /// Tries to compute a sanitized version of the executable's
        /// basename/stem, falling back to the hardcoded one above if it
        /// encounters difficulties.
        static inline std::string computeDefaultBasename() {
            namespace fs = boost::filesystem;
            auto binLoc = getBinaryLocation();
            if (binLoc.empty()) {
                return LOG_FILE_BASENAME;
            }
            auto exePath = fs::path{binLoc};
            auto fn = exePath.filename().stem().string();

            auto sanitized = sanitizeFilenamePiece(fn);
            if (sanitized.empty()) {
                return LOG_FILE_BASENAME;
            }

            return sanitized;
        }

        LogRegistry &LogRegistry::instance(std::string const *baseNamePtr) {
            static LogRegistry instance_{
                baseNamePtr ? *baseNamePtr : computeDefaultBasename()};
            return instance_;
        }

        LoggerPtr
        LogRegistry::getOrCreateLogger(const std::string &logger_name) {
            // If logger already exists, return a copy of it
            auto spd_logger = spdlog::get(logger_name);
            if (!spd_logger) {
                // Bummer, it didn't exist. We'll create one from scratch.
                try {
                    spd_logger = spdlog::details::registry::instance().create(
                        logger_name, begin(sinks_), end(sinks_));
                    spd_logger->set_pattern(DEFAULT_PATTERN);
                    /// @todo should this level be different than other levels?
                    spd_logger->set_level(convertToLevelEnum(minLevel_));
                    spd_logger->flush_on(
                        convertToLevelEnum(DEFAULT_FLUSH_LEVEL));
                } catch (const std::exception &e) {
                    generalPurposeLog_->error()
                        << "Caught exception attempting to create logger: "
                        << e.what();
                    generalPurposeLog_->error() << "Error creating logger. "
                                                   "Will only log to the "
                                                   "console.";
                } catch (...) {
                    generalPurposeLog_->error() << "Error creating logger. "
                                                   "Will only log to the "
                                                   "console.";
                }
            }

            return Logger::makeFromExistingImplementation(logger_name,
                                                          spd_logger);
        }

        void LogRegistry::flush() {
            for (auto &sink : sinks_) {
                try {
                    sink->flush();
                } catch (...) {
                    // fail silently
                }
            }
        }

        void LogRegistry::setPattern(const std::string &pattern) {
            spdlog::set_pattern(pattern.c_str());
        }

        void LogRegistry::setLevel(LogLevel severity) {
            if (severity > consoleLevel_) {
                // our min level is going above our previous console level...
                /// @todo right now this means the filtering is a harmless no-op
            }
            setLevelImpl(severity);
        }

        void LogRegistry::setConsoleLevel(LogLevel severity) {
            if (severity < minLevel_) {
                /// @todo Does it make sense that we must adjust overall level
                /// as well in this case?
                setLevelImpl(severity);
            }
            setConsoleLevelImpl(severity);
        }

        LogRegistry::LogRegistry(std::string const &logFileBaseName)
            : minLevel_(std::min(DEFAULT_LEVEL, DEFAULT_CONSOLE_LEVEL)),
              consoleLevel_(std::max(DEFAULT_LEVEL, DEFAULT_CONSOLE_LEVEL)),
              logFileBaseName_(logFileBaseName) {
            // Set default pattern and level
            spdlog::set_pattern(DEFAULT_PATTERN);
            spdlog::set_level(convertToLevelEnum(minLevel_));

            // Instantiate console and file sinks. These sinks will be used with
            // each logger that is created via the registry.

#if defined(OSVR_ANDROID)
            // Android doesn't have a console, it has logcat.
            // We won't filter because we won't log to file on Android.
            auto android_sink = getDefaultUnfilteredSink();
            sinks_.push_back(android_sink);
            auto &main_sink = android_sink;
#else
            // Console sink
            console_filter_ =
                getDefaultFilteredSink(convertToLevelEnum(consoleLevel_));
            sinks_.push_back(console_filter_);
            auto &main_sink = console_filter_;
#endif
            consoleOnlyLog_ =
                Logger::makeWithSink(OSVR_GENERAL_LOG_NAME, main_sink);
            generalPurposeLog_ = consoleOnlyLog_.get();

            createFileSink();

            auto binLoc = getBinaryLocation();
            if (!binLoc.empty()) {
                generalPurposeLog_->notice("Logging for ") << binLoc;
            }
        }

        LogRegistry::~LogRegistry() {
            // do nothing but flush
            flush();
        }

        void LogRegistry::setLevelImpl(LogLevel severity) {
            spdlog::set_level(convertToLevelEnum(severity));
            minLevel_ = severity;
        }

        void LogRegistry::setConsoleLevelImpl(LogLevel severity) {
            consoleLevel_ = severity;
            if (console_filter_) {
                console_filter_->set_level(convertToLevelEnum(severity));
            }
        }

        static inline bool shouldLogToFile() {
            using osvr::util::getEnvironmentVariable;
            auto fileLoggingEnabled = getEnvironmentVariable("OSVR_FILE_LOGGING_ENABLED");
            if (!fileLoggingEnabled || *fileLoggingEnabled == "0") {
                return false;
            }
            return true;
        }

        void LogRegistry::createFileSink() {
#if defined(OSVR_ANDROID)
            // Not logging to file on Android.
            return;
#else
            if (!shouldLogToFile()) {
                return;
            }

            // File sink - rotates daily
            std::string logDir;
            try {
                size_t q_size = 65536; // queue size must be power of 2
                spdlog::set_async_mode(q_size);
                namespace fs = boost::filesystem;
                auto base_name = fs::path(getLoggingDirectory(true));
                if (!base_name.empty()) {
                    logDir = base_name.string();
                    base_name /= logFileBaseName_;
                    auto daily_file_sink =
                        std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                            base_name.string().c_str(), LOG_FILE_EXTENSION, 0,
                            0, false);
                    sinks_.push_back(daily_file_sink);
                }
            } catch (const std::exception &e) {
                if (consoleOnlyLog_) {
                    consoleOnlyLog_->error()
                        << "Error creating log file sink: " << e.what()
                        << ". Will log to console only.";
                } else {
                    std::cerr
                        << "[OSVR] Error creating log file sink: " << e.what()
                        << ". Will log to console only." << std::endl;
                }
                return;
            } catch (...) {
                if (consoleOnlyLog_) {
                    consoleOnlyLog_->error(
                        "Error creating log file sink. Will log to "
                        "console only.");
                } else {
                    std::cerr
                        << "[OSVR] Error creating log file sink. Will log to "
                           "console only."
                        << std::endl;
                }
                return;
            }

            // If we succeeded in making a file sink, make a general logger that
            // uses both sinks, then announce as much as useful about the log
            // file location to it.
            generalLog_ = Logger::makeWithSinks(OSVR_GENERAL_LOG_NAME,
                                                {sinks_[0], sinks_[1]});

            if (generalLog_) {
                generalPurposeLog_ = generalLog_.get();
            } else {
                // this shouldn't happen...
                consoleOnlyLog_->error(
                    "Could not make a general log with both sinks!");
            }

            generalPurposeLog_->notice() << "Log file created in " << logDir;
            generalPurposeLog_->notice() << "Log file name starts with \""
                                         << logFileBaseName_ << "\"";
#endif // OSVR_ANDROID
        }

    } // end namespace log
} // end namespace util
} // end namespace osvr
