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
#include <boost/optional.hpp>
#include <folly/ProducerConsumerQueue.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/TimeValue.h>

// Standard includes
#include <array>
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
        OSVR_VelocityState vel;
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

        /// Attempts to receive a report: "consumes" as many as there are
        /// available, using the last one produced for further computation. If a
        /// process model and non-zero velocity are available, the state is
        /// predicted from the reported timestamp to "now" (or now +
        /// additionalPrediction if nonzero). If false is returned, no reports
        /// were available to consume.
        bool getReport(double additionalPrediction, BodyReport &report);
        /// @}

        /// @name processing-thread methods
        /// @{

        /// "Produces" an updated state.
        /// @return false if there was no room in the queue.
        bool updateState(util::time::TimeValue const &tv,
                         BodyState const &state);

        /// One-time call: sets up the process model, allowing the consumer end
        /// of this class to predict to "now".
        void initProcessModel(BodyProcessModel const &process);

        /// This only needs to be called once - it sets a transform that will be
        /// applied to each report just before it is used by the receiving
        /// thread.
        void setTrackerToRoomTransform(Eigen::Isometry3d const &xform);
        /// @}
      private:
        BodyReporting();
        /// @name One-time initialization
        /// @{
        bool m_hasProcessModel = false;
        BodyProcessModel m_process;
        Eigen::Isometry3d m_trackerToRoom;
        /// @}

        template <std::size_t ArraySize> struct QueueValue {
            util::time::TimeValue timestamp;
            std::array<double, ArraySize> stateData;
        };
        /// 13 elements, instead of 12, because we're shipping a quaternion
        /// instead of an incremental rotation.
        using QueueValueType = QueueValue<13>;
        using QueueValueVec = Eigen::Matrix<double, 13, 1>;
        /// The communication channel between threads.
        folly::ProducerConsumerQueue<QueueValueType> m_queue;

        /// @name Convenience members used by the consumer side, so they don't
        /// have to create them each time.
        /// @{
        BodyState m_state;
        util::time::TimeValue m_dataTime;
        /// @}
    };
    using BodyReportingPtr = std::unique_ptr<BodyReporting>;

    using BodyReportingVector = std::vector<BodyReportingPtr>;
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_ThreadsafeBodyReporting_h_GUID_EB81AB60_6C5C_4A92_CD3D_ADFA62489F70
