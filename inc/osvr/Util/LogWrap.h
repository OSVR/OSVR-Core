/** @file
    @brief Header wrapping the LogC interface with a nice C++ interface.
   Suitable for advanced, external usage.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_LoggerWrap_h_GUID_859CB903_76DC_468D_09E4_6645F581B8D6
#define INCLUDED_LoggerWrap_h_GUID_859CB903_76DC_468D_09E4_6645F581B8D6

// Internal Includes
#include <osvr/Util/LogC.h>

// Library/third-party includes
// - none

// Standard includes
#include <sstream>
#include <string>

namespace osvr {
namespace util {
    namespace log {
        class WrappedLogger;
        namespace detail {
            /// proxy class allowing you to stream output to a logger.
            class WrappedLoggerMessageProxy {
              public:
                ~WrappedLoggerMessageProxy() {
                    osvrLogMessage(impl_, level_, os_.str().c_str());
                }
                template <typename T> ostream &operator<<(T const &what) {
                    os_ << what;
                    return os_;
                }

              private:
                WrappedLoggerMessageProxy(WrappedLoggerMessageProxy const &);
                WrappedLoggerMessageProxy &
                operator=(WrappedLoggerMessageProxy const &);
                friend class ::osvr::util::log::WrappedLogger;
                WrappedLoggerMessageProxy(OSVR_Logger impl,
                                          OSVR_LogLevel level);
                OSVR_Logger impl_;
                OSVR_LogLevel level_;
                std::ostringstream os_;
            };
        } // namespace detail

        class WrappedLogger {
          public:
            explicit WrappedLogger(const char *name)
                : impl_(osvrLogMakeLogger(name)) {}
            explicit WrappedLogger(std::string const &name)
                : impl_(osvrLogMakeLogger(name.c_str())) {}
            ~WrappedLogger() { osvrLogFreeLogger(impl_); }

            /// use like mylogger.log(OSVR_LOGLEVEL_WARN) << "This is my sample
            /// warning" << 42;
            detail::WrappedLoggerMessageProxy log(OSVR_LogLevel level) {
                return detail::WrappedLoggerMessageProxy(impl_, level);
            }

          private:
            OSVR_Logger impl_;
        };
    } // namespace log
} // namespace util
} // namespace osvr
#endif // INCLUDED_LoggerWrap_h_GUID_859CB903_76DC_468D_09E4_6645F581B8D6
