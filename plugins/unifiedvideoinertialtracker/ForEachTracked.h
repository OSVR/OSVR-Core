/** @file
    @brief Header

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

#ifndef INCLUDED_ForEachTracked_h_GUID_8DA1845D_2D7F_4C8C_9424_C18D06339380
#define INCLUDED_ForEachTracked_h_GUID_8DA1845D_2D7F_4C8C_9424_C18D06339380

// Internal Includes
#include "TrackingSystem.h"
#include "TrackedBody.h"
#include "TrackedBodyTarget.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    /// For each body in a tracking system
    template <typename F> inline void forEachBody(TrackingSystem &sys, F &&f) {
        auto n = static_cast<BodyId::wrapped_type>(sys.getNumBodies());
        for (BodyId::wrapped_type i = 0; i < n; ++i) {
            f(sys.getBody(BodyId(i)));
        }
    }

    /// For each body in a tracking system - const overload.
    template <typename F>
    inline void forEachBody(TrackingSystem const &sys, F &&f) {
        auto n = static_cast<BodyId::wrapped_type>(sys.getNumBodies());
        for (BodyId::wrapped_type i = 0; i < n; ++i) {
            f(sys.getBody(BodyId(i)));
        }
    }

    /// For each target belonging to a tracked body
    ///
    /// Delegates to the member function of the tracked body.
    template <typename F> inline void forEachTarget(TrackedBody &body, F &&f) {
        body.forEachTarget(std::forward<F>(f));
    }

    /// For each target belonging to a tracked body - const overload.
    ///
    /// Delegates to the member function of the tracked body.
    template <typename F>
    inline void forEachTarget(TrackedBody const &body, F &&f) {
        body.forEachTarget(std::forward<F>(f));
    }

    /// For each target in a tracking system
    template <typename F>
    inline void forEachTarget(TrackingSystem &sys, F &&f) {
        forEachBody(sys, [&](TrackedBody &body) {
            forEachTarget(body, std::forward<F>(f));
        });
    }

    /// For each target in a tracking system - const overload.
    template <typename F>
    inline void forEachTarget(TrackingSystem const &sys, F &&f) {
        forEachBody(sys, [&](TrackedBody const &body) {
            forEachTarget(body, std::forward<F>(f));
        });
    }

    /// For each IMU in a tracking system
    template <typename F> inline void forEachIMU(TrackingSystem &sys, F &&f) {
        forEachBody(sys, [&](TrackedBody &body) {
            if (body.hasIMU()) {
                f(body.getIMU());
            }
        });
    }

    /// For each IMU in a tracking system - const overload.
    template <typename F>
    inline void forEachIMU(TrackingSystem const &sys, F &&f) {
        forEachBody(sys, [&](TrackedBody const &body) {
            if (body.hasIMU()) {
                f(body.getIMU());
            }
        });
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ForEachTracked_h_GUID_8DA1845D_2D7F_4C8C_9424_C18D06339380
