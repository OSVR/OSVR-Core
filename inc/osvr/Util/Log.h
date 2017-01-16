/** @file
    @brief Header for basic internal log reference. To actually log to the
   produced loggers, include <osvr/Util/Logger.h>

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

#ifndef INCLUDED_Log_h_GUID_B6053E2B_593D_4ECA_8C3B_2C55D0FE6A49
#define INCLUDED_Log_h_GUID_B6053E2B_593D_4ECA_8C3B_2C55D0FE6A49

// Internal Includes
#include <osvr/Util/Export.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>

/// @todo try this out - it does build, and I think it makes logical sense as
/// well.
#undef OSVR_USE_UNIQUEPTR_FOR_LOGGER

namespace osvr {
namespace util {
    namespace log {
        class Logger;
#ifdef OSVR_USE_UNIQUEPTR_FOR_LOGGER
        typedef unique_ptr<Logger> LoggerPtr;
        typedef shared_ptr<Logger> LoggerSharedPtr;
#else
        typedef shared_ptr<Logger> LoggerPtr;
#endif

        /// @brief Attempt to initialize the logging system with the indicated
        /// base filename (will be stripped of all characters outside of
        /// [a-zA-Z0-9_]). If it succeeds (because you called it before any
        /// logging calls occurred), it returns true.
        ///
        /// Not typically needed, since the log file will usually be named using
        /// a sanitized version of the executable file name's stem, but provided
        /// just in case that's not a reasonable default.
        OSVR_UTIL_EXPORT bool
        tryInitializingLoggingWithBaseName(std::string const &baseName);

        /// Make (or get from the registry) a logger by name.
        ///
        /// Always returns a valid pointer even on invalid input or state,
        /// though it may be a "fallback" logger.
        OSVR_UTIL_EXPORT LoggerPtr make_logger(const std::string &logger_name);

        /// @brief Drops a logger from the registry.
        ///
        /// The logger will survive until the last copy of it is destroyed
        /// (e.g., goes out of scope). This function is useful if you want to
        /// destroy a logger before the program terminates.
        OSVR_UTIL_EXPORT void drop(const std::string &logger_name);

        /// @brief Removes all the registered loggers from the registry.
        ///
        /// Each logger will survive until the last copy of it is destroyed
        /// (e.g., goes out of scope).
        OSVR_UTIL_EXPORT void dropAll();

        /// @brief For implementations with a centralized logger registry, flush
        /// all logger sinks.
        OSVR_UTIL_EXPORT void flush();

        OSVR_UTIL_EXPORT std::string getLoggingDirectory(bool make_dir = false);

    } // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_Log_h_GUID_B6053E2B_593D_4ECA_8C3B_2C55D0FE6A49
