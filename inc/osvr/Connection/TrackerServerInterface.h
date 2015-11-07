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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_TrackerServerInterface_h_GUID_2FECCA11_65B7_41AA_9326_8CD10202FA64
#define INCLUDED_TrackerServerInterface_h_GUID_2FECCA11_65B7_41AA_9326_8CD10202FA64

// Internal Includes
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {

    /// @brief Interface for external access to generating tracker reports.
    ///
    /// Does not inherit from BaseServerInterface because it wraps an existing
    /// VRPN class.
    class TrackerServerInterface {
      public:
        virtual void sendReport(OSVR_PositionState const &val,
                                OSVR_ChannelCount sensor,
                                util::time::TimeValue const &timestamp) = 0;

        virtual void sendReport(OSVR_OrientationState const &val,
                                OSVR_ChannelCount sensor,
                                util::time::TimeValue const &timestamp) = 0;

        virtual void sendReport(OSVR_PoseState const &val,
                                OSVR_ChannelCount sensor,
                                util::time::TimeValue const &timestamp) = 0;

        virtual void sendVelReport(OSVR_VelocityState const &val,
                                   OSVR_ChannelCount sensor,
                                   util::time::TimeValue const &timestamp) = 0;
        virtual void sendVelReport(OSVR_LinearVelocityState const &val,
                                   OSVR_ChannelCount sensor,
                                   util::time::TimeValue const &timestamp) = 0;
        virtual void sendVelReport(OSVR_AngularVelocityState const &val,
                                   OSVR_ChannelCount sensor,
                                   util::time::TimeValue const &timestamp) = 0;

        virtual void
        sendAccelReport(OSVR_AccelerationState const &val,
                        OSVR_ChannelCount sensor,
                        util::time::TimeValue const &timestamp) = 0;
        virtual void
        sendAccelReport(OSVR_LinearAccelerationState const &val,
                        OSVR_ChannelCount sensor,
                        util::time::TimeValue const &timestamp) = 0;
        virtual void
        sendAccelReport(OSVR_AngularAccelerationState const &val,
                        OSVR_ChannelCount sensor,
                        util::time::TimeValue const &timestamp) = 0;
    };

} // namespace connection
} // namespace osvr

#endif // INCLUDED_TrackerServerInterface_h_GUID_2FECCA11_65B7_41AA_9326_8CD10202FA64
