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

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenInterop.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    /// Add a util::time::TimeValue and a std::chrono::duration
    /// @todo put this in a TimeValue C++11-safe header
    template <typename Rep, typename Period>
    inline util::time::TimeValue
    operator+(util::time::TimeValue const &tv,
              std::chrono::duration<Rep, Period> additionalTime) {
        using namespace std::chrono;
        using SecondsDuration = duration<OSVR_TimeValue_Seconds>;
        using USecondsDuration =
            duration<OSVR_TimeValue_Microseconds, std::micro>;
        auto ret = tv;
        auto seconds = duration_cast<SecondsDuration>(additionalTime);
        ret.seconds += seconds.count();
        ret.microseconds +=
            duration_cast<USecondsDuration>(additionalTime - seconds).count();
        osvrTimeValueNormalize(&ret);
        return ret;
    }

    /// Add a util::time::TimeValue and a std::chrono::duration
    /// @todo put this in a TimeValue C++11-safe header
    template <typename Rep, typename Period>
    inline util::time::TimeValue
    operator+(std::chrono::duration<Rep, Period> additionalTime,
              util::time::TimeValue const &tv) {
        return tv + additionalTime;
    }

    /// Helper function to assign from a Kalman state to a body report, whether
    /// we predicted or not.
    inline void
    assignStateToBodyReport(BodyState const &state, BodyReport &report,
                            Eigen::Isometry3d const &trackerToRoom) {
        Eigen::Isometry3d output = trackerToRoom * state.getIsometry();
        util::eigen_interop::map(report.pose).rotation() =
            Eigen::Quaterniond(output.rotation());
        util::eigen_interop::map(report.pose).translation() =
            output.translation();
    }

    std::unique_ptr<BodyReporting> BodyReporting::make() {
        std::unique_ptr<BodyReporting> ret(new BodyReporting);
        return ret;
    }

    BodyReport BodyReporting::getReport(double additionalPrediction) {
        BodyState state;
        bool doingPrediction = false;
        BodyProcessModel process;
        util::time::TimeValue dataTime;
        Eigen::Isometry3d trackerToRoom;
        {
            std::unique_lock<std::mutex> lock{m_mutex, std::try_to_lock};
            if (!lock.owns_lock()) {
                // Didn't get the lock.
                return BodyReport::makeReportWithStatus(
                    ReportStatus::MutexLocked);
            }
            // OK, we got the lock.
            if (!m_shouldReport) {
                // Told we shouldn't report, OK.
                return BodyReport::makeReportWithStatus(
                    ReportStatus::NoReportAvailable);
            }
            /// If we got here, then we're reporting something.
            state = m_state;
            if (state.stateVector().tail<6>() !=
                kalman::types::Vector<6>::Zero()) {
                // If we have non-zero velocity, then we can do some prediction,
                // which we'll need the process for.
                doingPrediction = true;
                process = m_process;
            }
            dataTime = m_dataTime;
            trackerToRoom = m_trackerToRoom;
        } // unlock

        auto ret = BodyReport::makeReportWithStatus();
        if (doingPrediction) {
            // If we have non-zero velocity, then we can do some prediction.
            auto currentTime = util::time::getNow();
            /// Difference between measurement time and now.
            auto dt = osvrTimeValueDurationSeconds(&currentTime, &dataTime);
            /// and the additional time into the future we'd like to predict.
            dt += additionalPrediction;
            /// Using computeEstimate instead of the normal prediction saves us
            /// the unneeded prediction of the error covariance.
            state.setStateVector(process.computeEstimate(state, dt));
            /// Be sure to post-correct.
            state.postCorrect();

            /// OK, now set a proper timestamp for our prediction.
            ret.timestamp = currentTime +
                            std::chrono::duration<double>(additionalPrediction);
            assignStateToBodyReport(state, ret, trackerToRoom);
        } else {
            ret.timestamp = dataTime;
            assignStateToBodyReport(state, ret, trackerToRoom);
            /// @todo should we set the "don't report" flag here once we report
            /// a can't-predict state once?
        }

        return ret;
    }

    void BodyReporting::markShouldNotReport() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shouldReport = false;
    }
    /// Updates the state, implicitly setting the flag that the mainloop
    /// should report.
    void BodyReporting::updateState(util::time::TimeValue const &tv,
                                    BodyState const &state,
                                    BodyProcessModel const &process) {

        std::lock_guard<std::mutex> lock(m_mutex);
        m_shouldReport = true;
        m_dataTime = tv;
        m_state = state;
        m_process = process;
    }

    void BodyReporting::updateState(util::time::TimeValue const &tv,
                                    BodyState const &state) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_shouldReport = true;
        m_dataTime = tv;
        m_state = state;
    }

    void
    BodyReporting::setTrackerToRoomTransform(Eigen::Isometry3d const &xform) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_trackerToRoom = xform;
    }

    BodyReporting::BodyReporting()
        : m_trackerToRoom(Eigen::Isometry3d::Identity()) {}

} // namespace vbtracker
} // namespace osvr
