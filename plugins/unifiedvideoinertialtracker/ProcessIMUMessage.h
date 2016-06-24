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

#ifndef INCLUDED_ProcessIMUMessage_h_GUID_9F00FD7F_C068_4DB1_61C8_42317D1CF786
#define INCLUDED_ProcessIMUMessage_h_GUID_9F00FD7F_C068_4DB1_61C8_42317D1CF786

// Internal Includes
#include "IMUMessage.h"
#include "TrackedBodyIMU.h"
#include "TrackedBody.h"

// Library/third-party includes
#include <osvr/TypePack/Contains.h>
#include <osvr/Util/EigenInterop.h>

// Standard includes
#include <type_traits>

namespace osvr {
namespace vbtracker {
    namespace detail {

        /// Alias to determine if a class is in fact a "Timestamped Report"
        /// type.
        template <typename Report>
        using is_timestamped_report =
            typepack::contains<TimestampedReports, Report>;

        /// Implementation detail of unpacking and handling the IMU messages.
        class IMUMessageProcessor : public boost::static_visitor<> {
          public:
            BodyId body;
            void operator()(boost::none_t const &) const {
                /// dummy overload to handle empty messages
            }

            template <typename Report>
            typename std::enable_if<is_timestamped_report<Report>::value>::type
            operator()(Report const &report) {
                /// templated overload to handle real messages since they're
                /// identical except for the final element of the tuple.
                auto &imu = *std::get<0>(report);
                auto timestamp = std::get<1>(report);
                /// Go off to individual methods for the last argument.
                updatePose(imu, timestamp, std::get<2>(report));
            }

            void updatePose(TrackedBodyIMU &imu,
                            util::time::TimeValue const &timestamp,
                            OSVR_OrientationReport const &ori) {
                body = imu.getBody().getId();
                imu.updatePoseFromOrientation(
                    timestamp, util::eigen_interop::map(ori.rotation).quat());
            }

            void updatePose(TrackedBodyIMU &imu,
                            util::time::TimeValue const &timestamp,
                            OSVR_AngularVelocityReport const &angVel) {
                body = imu.getBody().getId();
                imu.updatePoseFromAngularVelocity(
                    timestamp,
                    util::eigen_interop::map(angVel.state.incrementalRotation)
                        .quat(),
                    angVel.state.dt);
            }
        };
    } // namespace detail
    inline BodyId processImuMessage(IMUMessage const &m) {

        detail::IMUMessageProcessor processor;
        boost::apply_visitor(processor, m);
        return processor.body;
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ProcessIMUMessage_h_GUID_9F00FD7F_C068_4DB1_61C8_42317D1CF786
