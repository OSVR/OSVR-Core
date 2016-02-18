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

#ifndef INCLUDED_ThreadsafeBodyReporting_h_GUID_EB81AB60_6C5C_4A92_CD3D_ADFA62489F70
#define INCLUDED_ThreadsafeBodyReporting_h_GUID_EB81AB60_6C5C_4A92_CD3D_ADFA62489F70

// Internal Includes
#include "ModelTypes.h"

// Library/third-party includes
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <boost/optional.hpp>

// Standard includes
#include <mutex>
#include <memory>

namespace osvr {
namespace vbtracker {
    enum class ReportStatus { Valid, MutexLocked, NoReportAvailable };
    struct BodyReport {

        static BodyReport
        makeReportWithStatus(ReportStatus status = ReportStatus::Valid) {
            BodyReport ret;
            ret.status = status;
            return ret;
        }

        /// Explicit boolean operator, so you can `if (report)` to see if it's
        /// valid.
        explicit operator bool() const { return status == ReportStatus::Valid; }

        ReportStatus status;
        util::time::TimeValue timestamp;
        OSVR_PoseState pose;
        // OSVR_VelocityState vel;
        // OSVR_AngularVelocityState angVel;
    };

    /// A per-body class intended to marshall data coming from the
    /// tracking/processing thread back to the mainloop thread.
    class BodyReporting {
      public:
        /// Factory function
        static std::unique_ptr<BodyReporting> make();

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        /// @name mainloop-thread methods
        /// @{

        /// Before using the contents of the report, check the status field -
        /// only if it is ReportStatus::Valid does it contain anything useful,
        /// for the timestamp it says on the object.
        ///
        /// Otherwise either the mutex was locked (you're in a
        /// fast-spinning loop, just get it next time), or there's nothing worth
        /// reporting - in both cases, the other fields are not initialized!
        BodyReport getReport(double additionalPrediction);
        /// @}

        /// @name processing-thread methods
        /// @brief These all block on the mutex, because the processing thread
        /// needs to get its message through.
        /// @{
        /// Sets the flag that the mainloop should not report. Doesn't touch the
        /// other members since they're unusuable by definition if you should
        /// not report.
        void markShouldNotReport();
        /// Updates the state, implicitly setting the flag that the mainloop
        /// should report.
        void updateState(util::time::TimeValue const &tv,
                         BodyState const &state,
                         BodyProcessModel const &process);
        /// @overload
        void updateState(util::time::TimeValue const &tv,
                         BodyState const &state);

        /// This only needs to be called once - it sets a transform that will be
        /// applied to each report just before it is used by the receiving
        /// thread.
        void setTrackerToRoomTransform(Eigen::Isometry3d const &xform);
        /// @}
      private:
        BodyReporting();
        std::mutex m_mutex;
        /// @name Protected by mutex
        /// @{
        bool m_shouldReport = false;
        util::time::TimeValue m_dataTime;
        BodyState m_state;
        BodyProcessModel m_process;
        Eigen::Isometry3d m_trackerToRoom;
        /// @}
    };
    using BodyReportingPtr = std::unique_ptr<BodyReporting>;

    using BodyReportingVector = std::vector<BodyReportingPtr>;
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_ThreadsafeBodyReporting_h_GUID_EB81AB60_6C5C_4A92_CD3D_ADFA62489F70
