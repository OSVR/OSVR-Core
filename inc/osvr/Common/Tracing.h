/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#ifndef INCLUDED_Tracing_h_GUID_0D1E8891_2D51_4281_9CBC_2B9B4DFB28C1
#define INCLUDED_Tracing_h_GUID_0D1E8891_2D51_4281_9CBC_2B9B4DFB28C1

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/TracingConfig.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>
#include <cstdint>

namespace osvr {
namespace common {
    namespace tracing {
        typedef std::int64_t TraceBeginStamp;
#ifdef OSVR_COMMON_TRACING_ENABLED
        struct MainTracePolicy {
            OSVR_COMMON_EXPORT static TraceBeginStamp begin(const char *text);
            OSVR_COMMON_EXPORT static void end(const char *text,
                                               TraceBeginStamp stamp);
            OSVR_COMMON_EXPORT static void mark(const char *text);
        };

        struct WorkerTracePolicy {
            OSVR_COMMON_EXPORT static TraceBeginStamp begin(const char *text);
            OSVR_COMMON_EXPORT static void end(const char *text,
                                               TraceBeginStamp stamp);
            OSVR_COMMON_EXPORT static void mark(const char *text);
        };
        /// @brief Class template base for "region" tracing.
        template <typename TracePolicy> class TracingRegion {
          public:
            /// @brief Destructor, ending region
            ~TracingRegion() { TracePolicy::end(m_text, m_stamp); }

          protected:
            /// @brief Explicit constructor from string literal in subclass,
            /// starting region.
            explicit TracingRegion(const char text[]) : m_text(text) {
                m_stamp = TracePolicy::begin(m_text);
            }
            /// @brief noncopyable
            TracingRegion(TracingRegion const &) = delete;
            /// @brief nonassignable
            TracingRegion &operator=(TracingRegion const &) = delete;

          private:
            TraceBeginStamp m_stamp;
            const char *m_text;
        };
        template <typename Policy>
        inline void markConcatenation(const char *fixedString,
                                      std::string const &string) {
            Policy::mark((fixedString + string).c_str());
        }
#else  // OSVR_COMMON_TRACING_ENABLED ^^ // vv !OSVR_COMMON_TRACING_ENABLED
        struct MainTracePolicy {
            static TraceBeginStamp begin(const char *) { return 0; }
            static void end(const char *, TraceBeginStamp) {}
            static void mark(const char *) {}
        };
        struct WorkerTracePolicy {
            static TraceBeginStamp begin(const char *) { return 0; }
            static void end(const char *, TraceBeginStamp) {}
            static void mark(const char *) {}
        };

        template <typename TracePolicy> class TracingRegion {
          protected:
            explicit TracingRegion(const char *) {}
        };
        inline TraceBeginStamp driverUpdateStart(std::string const &,
                                                 std::string const &) {
            return 0;
        }
        inline void driverUpdateEnd(TraceBeginStamp) {}
        template <typename Policy>
        inline void markConcatenation(const char *, std::string const &) {}
#endif // !OSVR_COMMON_TRACING_ENABLED

        // -- Common code between dummy implementation and real implementation

        /// @brief "Guard"-type class to trace the region of a server update
        class ServerUpdate : public TracingRegion<MainTracePolicy> {
          public:
            ServerUpdate() : TracingRegion<MainTracePolicy>("ServerUpdate") {}
        };

        inline void markPathTreeBroadcast() {
            MainTracePolicy::mark("Path Tree Broadcast");
        }
        inline void markHardwareDetect() {
            MainTracePolicy::mark("Hardware Detection");
        }

        /// @brief "Guard"-type class to trace the region of a server update
        class ClientUpdate : public TracingRegion<MainTracePolicy> {
          public:
            ClientUpdate() : TracingRegion<MainTracePolicy>("ClientUpdate") {}
        };
        inline void markTimestampOutOfOrder() {
            MainTracePolicy::mark("Timestamp out of order");
        }
        inline void markNewTrackerData() {
            MainTracePolicy::mark("New tracker data");
        }
        inline void markGetState(std::string const &path) {
            markConcatenation<WorkerTracePolicy>("GetState ", path);
        }
        inline void markGetInterface(std::string const &path) {
            markConcatenation<WorkerTracePolicy>("GetInterface ", path);
        }
        inline void markReleaseInterface(std::string const &path) {
            markConcatenation<WorkerTracePolicy>("ReleaseInterface ", path);
        }

    } // namespace tracing
} // namespace common
} // namespace osvr

#endif // INCLUDED_Tracing_h_GUID_0D1E8891_2D51_4281_9CBC_2B9B4DFB28C1
