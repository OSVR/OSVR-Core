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

#ifndef INCLUDED_IMUMessage_h_GUID_BF9DF56D_F7CB_4E92_C9E6_B44C56C8138D
#define INCLUDED_IMUMessage_h_GUID_BF9DF56D_F7CB_4E92_C9E6_B44C56C8138D

// Internal Includes
// - none

// Library/third-party includes
#include <boost/none_t.hpp>
#include <boost/variant.hpp>
#include <osvr/TypePack/List.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/TimeValue.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    // Forward declaration
    class TrackedBodyIMU;

    enum class ImuMessageCategory { Empty, Orientation, AngularVelocity };

    /// An IMU report data structure, along with the report time
    /// and the internal tracking system's pointer to IMU object.
    template <typename ReportType> class TimestampedImuReport {
      public:
        TimestampedImuReport(TrackedBodyIMU &myImu,
                             util::time::TimeValue const &tv,
                             ReportType const &d)
            : imuPtr(&myImu), timestamp(tv), data(d) {}

      private:
        TrackedBodyIMU *imuPtr;

      public:
        TrackedBodyIMU &imu() const { return *imuPtr; }

        util::time::TimeValue timestamp;
        ReportType data;
    };

    /// Generic constructor/factory function
    template <typename ReportType>
    inline TimestampedImuReport<ReportType>
    makeImuReport(TrackedBodyIMU &myImu, util::time::TimeValue const &tv,
                  ReportType const &d) {
        return TimestampedImuReport<ReportType>{myImu, tv, d};
    }

    /// An orientation report data structure, along with the report time
    /// and the internal tracking system's pointer to IMU object.
    using TimestampedOrientation = TimestampedImuReport<OSVR_OrientationReport>;

    /// An angular velocity report data structure, along with the report time
    /// and the internal tracking system's pointer to IMU object.
    using TimestampedAngVel = TimestampedImuReport<OSVR_AngularVelocityReport>;

    /// Typelist containing only the member types of the MessageEntry variant
    /// type that actually constitute valid report types.
    using TimestampedReports =
        typepack::list<TimestampedOrientation, TimestampedAngVel>;

    /// A "typesafe tagged-union" variant that can hold an IMU report data along
    /// with the timestamp and the pointer to the tracking system's IMU object
    /// that it applies to.
    using IMUMessage = boost::variant<boost::none_t, TimestampedOrientation,
                                      TimestampedAngVel>;

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_IMUMessage_h_GUID_BF9DF56D_F7CB_4E92_C9E6_B44C56C8138D
