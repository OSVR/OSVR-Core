/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/Tracing.h>

#ifdef OSVR_COMMON_TRACING_ENABLED
// Library/third-party includes
#if OSVR_COMMON_TRACING_ETW
#include <ETWProviders/etwprof.h>
#include <vrpn_WindowsH.h>
#endif

// Standard includes
#include <sstream>

namespace osvr {
namespace common {
    namespace tracing {

#if OSVR_COMMON_TRACING_ETW
        TraceBeginStamp MainTracePolicy::begin(const char *text) {
            return ETWBegin(text);
        }
        void MainTracePolicy::end(const char *text, TraceBeginStamp stamp) {
            ETWEnd(text, stamp);
        }

        void MainTracePolicy::mark(const char *text) { ETWMark(text); }

        TraceBeginStamp WorkerTracePolicy::begin(const char *text) {
            return ETWWorkerBegin(text);
        }
        void WorkerTracePolicy::end(const char *text, TraceBeginStamp stamp) {
            ETWWorkerEnd(text, stamp);
        }

        void WorkerTracePolicy::mark(const char *text) { ETWWorkerMark(text); }
#endif
    } // namespace tracing
} // namespace common
} // namespace osvr

#endif
