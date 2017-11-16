/** @file
    @brief Implementation

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

// Internal Includes
#include "ThreadsafeBodyReporting.h"
#include "AngVelTools.h"

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/EigenQuatExponentialMap.h>
#include <osvr/Util/TimeValueChrono.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    static const double VELOCITY_DT = 0.02;
    static const std::size_t REPORT_QUEUE_SIZE = 16;

    /// Turn a body-space angular velocity vector into a room-space incremental
    /// rotation quaternion.
    ///
    /// Orientation is assumed to be normalized. dt must be chosen such that the
    /// velocity will not alias.
    template <typename Derived>
    inline Eigen::Quaterniond
    angVelVecToQuat(Eigen::Quaterniond const &orientation, double dt,
                    Eigen::MatrixBase<Derived> const &vec) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);

        Eigen::Quaterniond result =
            orientation * angVelVecToIncRot(vec, dt) * orientation.conjugate();
        return result;
    }

    /// Helper function to assign from a Kalman state to a body report, whether
    /// we predicted or not.
    inline void
    assignStateToBodyReport(BodyState const &state, BodyReport &report,
                            Eigen::Isometry3d const &trackerToRoom) {
        Eigen::Isometry3d output = trackerToRoom * state.getIsometry();
        Eigen::Quaterniond orientation =
            Eigen::Quaterniond(output.rotation()).normalized();
        util::eigen_interop::map(report.pose).rotation() = orientation;
        util::eigen_interop::map(report.pose).translation() =
            output.translation();

        util::eigen_interop::map(
            report.vel.angularVelocity.incrementalRotation) =
            angVelVecToQuat(orientation, VELOCITY_DT, state.angularVelocity());
        report.vel.angularVelocity.dt = VELOCITY_DT;
        report.vel.angularVelocityValid = OSVR_TRUE;

        util::eigen_interop::map(report.vel.linearVelocity) =
            trackerToRoom * state.velocity();
        report.vel.linearVelocityValid = OSVR_TRUE;
    }

    std::unique_ptr<BodyReporting> BodyReporting::make() {
        std::unique_ptr<BodyReporting> ret(new BodyReporting);
        return ret;
    }

    bool BodyReporting::getReport(double additionalPrediction,
                                  BodyReport &report) {

        QueueValueType queueVal;
        bool gotOne = false;
        // Read all the queued-up states, but only keep the most recent one.
        while (m_queue.read(queueVal)) {
            gotOne = true;
        }
        if (!gotOne) {
            return false;
        }

        /// Thaw out the frozen state.
        m_dataTime = queueVal.timestamp;
        Eigen::Map<QueueValueVec> valMap(queueVal.stateData.data());
        m_state.incrementalOrientation() = Eigen::Vector3d::Zero();
        m_state.position() = valMap.head<3>();
        m_state.setQuaternion(Eigen::Quaterniond(valMap.segment<4>(3)));
        m_state.velocity() = valMap.segment<3>(7);
        m_state.angularVelocity() = valMap.tail<3>();

        // If we have a process model, and have non-zero velocity, then we can
        // do some prediction.
        bool doingPrediction =
            m_hasProcessModel && (m_state.stateVector().tail<6>() !=
                                  kalman::types::Vector<6>::Zero());

        if (doingPrediction) {
            // If we have non-zero velocity, then we can do some prediction.
            auto currentTime = util::time::getNow();
            /// Difference between measurement time and now.
            auto dt = osvrTimeValueDurationSeconds(&currentTime, &m_dataTime);
            /// and the additional time into the future we'd like to predict.
            dt += additionalPrediction;

            /// Using computeEstimate instead of the normal prediction saves us
            /// the unneeded prediction of the error covariance.
            m_state.setStateVector(m_process.computeEstimate(m_state, dt));
            /// Be sure to post-correct.
            m_state.postCorrect();

            /// OK, now set a proper timestamp for our prediction.
            report.timestamp = currentTime + std::chrono::duration<double>(
                                                 additionalPrediction);
        } else {
            report.timestamp = m_dataTime;
        }
        assignStateToBodyReport(m_state, report, m_trackerToRoom);
        report.status = ReportStatus::Valid;
        return true;
    }

    bool BodyReporting::updateState(util::time::TimeValue const &tv,
                                    BodyState const &state) {
        QueueValueType val;
        /// Initialize the array inside the queue value
        QueueValueVec::Map(val.stateData.data()) << state.position(),
            state.getQuaternion().coeffs(), state.velocity(),
            state.angularVelocity();

        val.timestamp = tv;
        return m_queue.write(std::move(val));
    }

    void BodyReporting::initProcessModel(BodyProcessModel const &process) {
        m_process = process;
        m_hasProcessModel = true;
    }

    void
    BodyReporting::setTrackerToRoomTransform(Eigen::Isometry3d const &xform) {
        m_trackerToRoom = xform;
    }

    BodyReporting::BodyReporting()
        : m_trackerToRoom(Eigen::Isometry3d::Identity()),
          m_queue(REPORT_QUEUE_SIZE) {}

} // namespace vbtracker
} // namespace osvr
