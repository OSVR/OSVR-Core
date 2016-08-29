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
#include "LogSinks.h"
#include <osvr/Util/GetEnvironmentVariable.h>
#include <osvr/Util/Log.h>
#include <osvr/Util/Logger.h>
#include <osvr/Util/LogConfig.h> // for OSVR_UTIL_LOG_SINGLETON
#include <osvr/Util/PlatformConfig.h>

#ifdef OSVR_UTIL_LOG_SINGLETON
#include "LogUtils.h"
#include <osvr/Util/LogRegistry.h>
#endif

// Library/third-party includes
#include <boost/filesystem.hpp>
#include <spdlog/spdlog.h>

// Standard includes
#include <memory> // for std::make_shared, std::shared_ptr
#include <string> // for std::string

namespace osvr {
namespace util {
    namespace log {

// TODO if OSVR_ANDROID, use android sink

#ifdef OSVR_UTIL_LOG_SINGLETON
        bool tryInitializingLoggingWithBaseName(std::string const &baseName) {
            auto sanitized = sanitizeFilenamePiece(baseName);
            // try initialization
            auto &inst = LogRegistry::instance(&sanitized);
            // test for success
            return (inst.getLogFileBaseName() == sanitized);
        }

        LoggerPtr make_logger(const std::string &logger_name) {
            return LogRegistry::instance().getOrCreateLogger(logger_name);
        }

        void flush() { LogRegistry::instance().flush(); }
#else
        /*
         * This implementation avoids using a singleton.  The downside is that
         * each call to make_logger() will generate a new logger instead of
         * reusing an existing logger of the same name (and loggers do not have
         * custom dual sinks, etc.)
         *
         * It is recommended that you store the LoggerPtr locally to avoid the
         * performance penalty if you must compile without the singleton.
         */
        LoggerPtr make_logger(const std::string &logger_name) {
            /// @todo use custom OSVR logger that splits output to STDOUT and
            /// STDERR and log files based on severity levels, etc.
            auto sink = std::make_shared<stdout_sink_mt>();
            auto spd_logger =
                std::make_shared< ::spdlog::logger>(logger_name, sink);
            spd_logger->set_pattern("%b %d %T.%e %l %n: %v");
            return Logger::makeFromExistingImplementation(logger_name,
                                                          spd_logger);
        }

        void flush() {
            // no-op in the absence of a logger registry.
        }

        bool tryInitializingLoggingWithBaseName(std::string const &) {
            // no singleton, no file sink.
            return false;
        }
#endif

        std::string getLoggingDirectory(bool make_dir) {
            namespace fs = boost::filesystem;
            using osvr::util::getEnvironmentVariable;
            fs::path log_dir;

#if defined(OSVR_LINUX)
            // There's currently no great location for storing log files in the
            // XDG system. (See the STATE proposal by Debian
            // <https://wiki.debian.org/XDGBaseDirectorySpecification#Proposal:_STATE_directory>.)
            // So for now, we'll store our log files in the $XDG_CACHE_HOME
            // directory.
            //
            // $XDG_CACHE_HOME defines the base directory relative to which user
            // specific non-essential data files should be stored. If
            // $XDG_CACHE_HOME is either not set or empty, a default equal to
            // $HOME/.cache should be used.
            auto xdg_cache_dir = getEnvironmentVariable("XDG_CACHE_HOME");
            if (xdg_cache_dir) {
                log_dir = *xdg_cache_dir;
            } else {
                auto home_dir = getEnvironmentVariable("HOME");
                log_dir = fs::path(*home_dir) / ".cache";
            }
            log_dir /= fs::path("osvr") / "logs";
#elif defined(OSVR_MACOSX)
            auto home_dir = getEnvironmentVariable("HOME");
            if (home_dir) {
                log_dir = *home_dir;
            }
            log_dir /= "Library" / fs::path("Logs") / "OSVR";
#elif defined(OSVR_WINDOWS)
            /// @todo there's actually a win32 api call to get localappdata
            /// that's preferred to the env var.
            auto local_app_dir = getEnvironmentVariable("LocalAppData");
            if (local_app_dir) {
                log_dir = *local_app_dir;
            } else {
                log_dir = "c:/";
            }
            log_dir /= fs::path("OSVR") / "Logs";
#endif

            if (fs::is_directory(log_dir))
                return log_dir.string();

            if (make_dir) {
                auto success = fs::create_directories(log_dir);
                if (!success) {
                    log_dir.clear();
                }
            }

            return log_dir.string();
        }

    } // end namespace log
} // end namespace util
} // end namespace osvr
