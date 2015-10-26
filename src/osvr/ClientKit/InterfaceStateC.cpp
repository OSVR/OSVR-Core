/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/ClientKit/InterfaceStateC.h>
#include <osvr/Common/ClientInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

#define OSVR_CALLBACK_METHODS(TYPE)                                            \
    OSVR_ReturnCode osvrGet##TYPE##State(OSVR_ClientInterface iface,           \
                                         struct OSVR_TimeValue *timestamp,     \
                                         OSVR_##TYPE##State *state) {          \
        bool hasState =                                                        \
            iface->getState<OSVR_##TYPE##Report>(*timestamp, *state);          \
        return hasState ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;           \
    }

OSVR_CALLBACK_METHODS(Pose)
OSVR_CALLBACK_METHODS(Position)
OSVR_CALLBACK_METHODS(Orientation)
OSVR_CALLBACK_METHODS(Velocity)
OSVR_CALLBACK_METHODS(LinearVelocity)
OSVR_CALLBACK_METHODS(AngularVelocity)
OSVR_CALLBACK_METHODS(Acceleration)
OSVR_CALLBACK_METHODS(LinearAcceleration)
OSVR_CALLBACK_METHODS(AngularAcceleration)
OSVR_CALLBACK_METHODS(Button)
OSVR_CALLBACK_METHODS(Analog)
OSVR_CALLBACK_METHODS(Location2D)
OSVR_CALLBACK_METHODS(Direction)
OSVR_CALLBACK_METHODS(EyeTracker2D)
OSVR_CALLBACK_METHODS(EyeTracker3D)
OSVR_CALLBACK_METHODS(EyeTrackerBlink)
OSVR_CALLBACK_METHODS(NaviVelocity)
OSVR_CALLBACK_METHODS(NaviPosition)
OSVR_CALLBACK_METHODS(SkeletonJoint)
OSVR_CALLBACK_METHODS(SkeletonWhole)
OSVR_CALLBACK_METHODS(SkeletonTrimmed)
OSVR_CALLBACK_METHODS(SkeletonHand)
OSVR_CALLBACK_METHODS(SkeletonArm)
OSVR_CALLBACK_METHODS(SkeletonFoot)
OSVR_CALLBACK_METHODS(SkeletonLeg)

#undef OSVR_CALLBACK_METHODS
