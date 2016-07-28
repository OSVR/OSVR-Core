/** @file
    @brief Header for an RAII object defining platform-specific actions to take
   to enter/exit a "low-latency" system mode.

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

#ifndef INCLUDED_LowLatency_h_GUID_A7B15740_3824_499E_22C3_EE2B08AFC7AC
#define INCLUDED_LowLatency_h_GUID_A7B15740_3824_499E_22C3_EE2B08AFC7AC

// Internal Includes
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory>

namespace osvr {
namespace common {
    /// An object that sets a system for low-latency, but potentially
    /// high-CPU-usage, high power, and even lower-performance due to scheduling
    /// (see the cautions about timeBeginPeriod).
    ///
    /// Typical usage would be for a class to hold a unique_ptr of this,
    /// instantiate a new one only when you need to enter the mode, then reset
    /// the pointer when you don't need it any more.
    ///
    /// Explanation, by way of implementation detail: On Windows, this requests
    /// a change to the Windows global timer frequency with timeBeginPeriod and
    /// matching timeEndPeriod. Not polite to leave it on long-term, but "this
    /// is VR" so when milliseconds count, it might be OK. Bruce Dawson even
    /// says so :)
    /// https://randomascii.wordpress.com/2016/03/08/power-wastage-on-an-idle-laptop/#comment-20184
    class LowLatency {
      public:
        OSVR_COMMON_EXPORT LowLatency();
        OSVR_COMMON_EXPORT ~LowLatency();
        LowLatency(LowLatency const &) = delete;
        LowLatency &operator=(LowLatency const &) = delete;

      private:
        // private implementation, if any is needed.
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_LowLatency_h_GUID_A7B15740_3824_499E_22C3_EE2B08AFC7AC
