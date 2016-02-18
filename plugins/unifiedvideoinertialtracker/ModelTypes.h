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

#ifndef INCLUDED_ModelTypes_h_GUID_F873E106_09BF_41A7_B7C7_CCE59466A590
#define INCLUDED_ModelTypes_h_GUID_F873E106_09BF_41A7_B7C7_CCE59466A590

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Kalman/PoseState.h>
#include <osvr/Kalman/PoseSeparatelyDampedConstantVelocity.h>
#include <osvr/Kalman/PureVectorState.h>

// Standard includes
#include <memory>
#include <vector>

namespace osvr {
namespace vbtracker {

    using BodyState = kalman::pose_externalized_rotation::State;
    using BodyProcessModel =
        osvr::kalman::PoseSeparatelyDampedConstantVelocityProcessModel;

    using BeaconState = kalman::PureVectorState<3>;
    using BeaconStatePtr = std::unique_ptr<BeaconState>;
    using BeaconStateVec = std::vector<BeaconStatePtr>;
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_ModelTypes_h_GUID_F873E106_09BF_41A7_B7C7_CCE59466A590
