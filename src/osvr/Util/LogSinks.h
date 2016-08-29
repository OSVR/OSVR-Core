/** @file
    @brief Header

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

#ifndef INCLUDED_LogSinks_h_GUID_C582966D_EA04_42D2_83FF_19483537D704
#define INCLUDED_LogSinks_h_GUID_C582966D_EA04_42D2_83FF_19483537D704

// Internal Includes
#include "LogDefaults.h"
#include <osvr/Util/PlatformConfig.h>

#if defined(OSVR_LINUX) || defined(OSVR_MACOSX)
#define OSVR_GET_ANSICOLOR_SINK
#endif

// Library/third-party includes
#include <spdlog/sinks/ostream_sink.h>
#ifdef OSVR_GET_ANSICOLOR_SINK
#include <spdlog/sinks/ansicolor_sink.h>
#endif
#include <spdlog/common.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/android_sink.h>
#include <spdlog/spdlog.h>

// Standard includes
#include <iostream>
#include <mutex>
#include <utility> // for std::move

namespace osvr {
namespace util {
    namespace log {

        /**
         * @brief A sink which sends its output to std::cout. This sink is a
         * drop-in replacement for spdlog::stdout_sink and differs only in that
         * this sink isn't a singleton.
         */
        template <typename Mutex>
        class stdout_sink : public ::spdlog::sinks::ostream_sink<Mutex> {
          public:
            stdout_sink()
                : ::spdlog::sinks::ostream_sink<Mutex>(std::cout, true) {
                // do nothing
            }
        };

        typedef stdout_sink<::spdlog::details::null_mutex> stdout_sink_st;
        typedef stdout_sink<std::mutex> stdout_sink_mt;

        /**
         * @brief A sink which sends its output to std::cerr. This sink is a
         * drop-in replacement for spdlog::stderr_sink and differs only in that
         * this sink isn't a singleton.
         */
        template <typename Mutex>
        class stderr_sink : public ::spdlog::sinks::ostream_sink<Mutex> {
          public:
            stderr_sink()
                : ::spdlog::sinks::ostream_sink<Mutex>(std::cerr, true) {
                // do nothing
            }
        };

        typedef stderr_sink<std::mutex> stderr_sink_mt;
        typedef stderr_sink<::spdlog::details::null_mutex> stderr_sink_st;

        /// A decorator around another sink that applies a custom log level
        /// filter.
        class filter_sink : public ::spdlog::sinks::sink {
          public:
            /// Construct from a wrapped sink rvalue-ref and a filter level
            /// threshold.
            filter_sink(spdlog::sink_ptr &&wrapped_sink,
                        spdlog::level::level_enum filter_level)
                : sink_(std::move(wrapped_sink)), level_(filter_level) {}

            virtual ~filter_sink() {}

            /// Change our internal level threshold, below which we will not
            /// pass messages on to the wrapped sink.
            void set_level(spdlog::level::level_enum filter_level) {
                level_ = filter_level;
            }

            /// This is the guts of the implementation: we check the level
            /// against our internal level setting and early-out return if it's
            /// below our setting.
            void log(const spdlog::details::log_msg &msg) override {
                if (msg.level < level_) {
                    return;
                }
                sink_->log(msg);
            }

            /// Obligatory implementation, just passed through.
            void flush() override { sink_->flush(); }

          private:
            spdlog::sink_ptr sink_;
            spdlog::level::level_enum level_;
        };

        static inline spdlog::sink_ptr getUnfilteredConsoleSink() {
            // Console sink
            auto console_out = spdlog::sinks::stderr_sink_mt::instance();
#if defined(OSVR_GET_ANSICOLOR_SINK)
            auto color_sink = std::make_shared<spdlog::sinks::ansicolor_sink>(
                console_out); // taste the rainbow!
            return color_sink;
#else
            // No color for Windows yet (and not tested on other platforms)
            return console_out;
#endif
        }

        static inline spdlog::sink_ptr getDefaultUnfilteredSink() {
#if defined(OSVR_ANDROID)
            // Android doesn't have a console, it has logcat.
            auto android_sink =
                std::make_shared<spdlog::sinks::android_sink_mt>(
                    ANDROID_LOG_TAG);
            return android_sink;
#else
            return getUnfilteredConsoleSink();
#endif
        }

        static inline std::shared_ptr<filter_sink>
        getDefaultFilteredSink(spdlog::level::level_enum filter_level) {
            return std::make_shared<filter_sink>(getDefaultUnfilteredSink(),
                                                 filter_level);
        }

    } // end namespace log
} // end namespace util
} // end namespace osvr

#endif // INCLUDED_LogSinks_h_GUID_C582966D_EA04_42D2_83FF_19483537D704
