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

#ifndef INCLUDED_ApplyIMUToState_h_GUID_B896B3E1_97C6_44BE_50CE_EDA075299FED
#define INCLUDED_ApplyIMUToState_h_GUID_B896B3E1_97C6_44BE_50CE_EDA075299FED

// Internal Includes
#include "ModelTypes.h"
#include "CannedIMUMeasurement.h"

// Library/third-party includes
#include <osvr/Util/TimeValue.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    class TrackingSystem;
    /// @return updated state in place.
    void applyIMUToState(TrackingSystem const &sys,
                         util::time::TimeValue const &initialTime,
                         BodyState &state, BodyProcessModel &processModel,
                         util::time::TimeValue const &newTime,
                         CannedIMUMeasurement const &meas);
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ApplyIMUToState_h_GUID_B896B3E1_97C6_44BE_50CE_EDA075299FED
