/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/LowLatency.h>

#ifdef _WIN32
#define NO_MINMAX
#include <windows.h>
#endif

namespace osvr {
namespace common {

#ifdef _WIN32
#define OSVR_HAVE_LOWLATENCY_CODE
    /// Drop timer period to 1ms when in low latency mode.
    static const UINT TIMER_PERIOD = 1;
    struct LowLatency::Impl {
        bool beginSucceeded = false;
    };

/// @todo Unclear from docs whether a failed call to timeEndPeriod must also be
/// matched
/// https://msdn.microsoft.com/en-us/library/windows/desktop/dd757624(v=vs.85).aspx
#if 0
    LowLatency::LowLatency() : m_impl(new Impl) {
        auto result = timeBeginPeriod(TIMER_PERIOD);
        if (TIMERR_NOERROR == result) {
            m_impl->beginSucceeded = true;
        }
    }
#endif

    LowLatency::LowLatency() { timeBeginPeriod(TIMER_PERIOD); }
    LowLatency::~LowLatency() {
        /// Don't really care about the success of this call - nothing we can
        /// do.
        timeEndPeriod(TIMER_PERIOD);
    }
#endif

#ifndef OSVR_HAVE_LOWLATENCY_CODE
    // Fallback no-op implementations
    struct LowLatency::Impl {};
    LowLatency::LowLatency() {}
    LowLatency::~LowLatency() {}
#endif

} // namespace common
} // namespace osvr
