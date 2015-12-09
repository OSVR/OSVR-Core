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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_VideoIMUFusion_h_GUID_85338EA5_58E6_4787_16D2_EC53201EFE9F
#define INCLUDED_VideoIMUFusion_h_GUID_85338EA5_58E6_4787_16D2_EC53201EFE9F

// Internal Includes
#include "FusionParams.h"
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory>
#include <cassert>

/// The core of the fusion code - doesn't deal with getting data in or reporting
/// it out, for easier use in testing.
class VideoIMUFusion {
  public:
    /// Constructor
    VideoIMUFusion(VideoIMUFusionParams const &params = VideoIMUFusionParams());
    /// Out-of-line destructor required for unique_ptr pimpl idiom
    ~VideoIMUFusion();

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    /// Call with each new IMU report, whether or not fusion is in running
    /// state.
    void handleIMUData(const OSVR_TimeValue &timestamp,
                       const OSVR_OrientationReport &report);

    void handleIMUVelocity(const OSVR_TimeValue &timestamp,
                           const Eigen::Vector3d &angVel);
    /// Call with each new video tracker report once we've entered running
    /// state.
    void handleVideoTrackerDataWhileRunning(const OSVR_TimeValue &timestamp,
                                            const OSVR_PoseReport &report);
    /// Call with each new video tracker report, as well as the most recent IMU
    /// orientation state, while the filter has not yet entered running state.
    ///
    /// The IMU state is required to compute a filtered estimate of the
    /// position/orientation of the camera relative to the room and IMU
    void handleVideoTrackerDataDuringStartup(
        const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report,
        const OSVR_OrientationState &orientation);

    bool running() const { return m_state == State::Running; }

    /// Returns the latest (fusion result, if available) pose.
    OSVR_PoseState const &getLatestPose() const {
        return m_lastPose;
    }

    /// Returns the timestamp associated with the latest (fusion result, if available) pose.
    osvr::util::time::TimeValue const &getLatestTime() const {
        return m_lastTime;
    }

    /// Returns the latest video-tracker pose, re-oriented to be in room space.
    /// Only valid once running state is entered!
    OSVR_PoseState const &getLatestReorientedVideoPose() const {
        assert(running());
        return m_reorientedVideo;
    }

    /// Returns the latest pose of the camera in the room.
    /// Only valid once running state is entered!
    ///
    /// Currently constant once running state is entered.
    OSVR_PoseState const &getLatestCameraPose() const {
        assert(running());
        return m_camera;
    }

    /// Returns the current state error covariance matrix
    /// Only valid once running state is entered!
    Eigen::Matrix<double, 12, 12> const &getErrorCovariance() const;

  private:
    void enterCameraPoseAcquisitionState();
    void enterRunningState(Eigen::Isometry3d const &cTr,
                           const OSVR_TimeValue &timestamp,
                           const OSVR_PoseReport &report,
                           const OSVR_OrientationState &orientation);
    void updateFusedOutput(const OSVR_TimeValue &timestamp);
    enum class State {
        /// We do not yet know the relative pose of the camera
        AcquiringCameraPose,
        Running
    };

    State m_state = State::AcquiringCameraPose;

    class StartupData;
    std::unique_ptr<StartupData> m_startupData;
    class RunningData;
    std::unique_ptr<RunningData> m_runningData;
    Eigen::Isometry3d m_cTr;

    OSVR_PoseState m_camera;
    OSVR_PoseState m_reorientedVideo;
    OSVR_PoseState m_lastPose;
    osvr::util::time::TimeValue m_lastTime;
    VideoIMUFusionParams m_params;
};

#endif // INCLUDED_VideoIMUFusion_h_GUID_85338EA5_58E6_4787_16D2_EC53201EFE9F
