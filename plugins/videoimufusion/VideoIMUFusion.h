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
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
// - none

// Standard includes
#include <memory>

/// The core of the fusion code - doesn't deal with getting data in or reporting
/// it out, for easier use in testing.
class VideoIMUFusion {
  public:
    VideoIMUFusion();
    ~VideoIMUFusion();
    OSVR_ReturnCode update();
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    void handleIMUData(const OSVR_TimeValue &timestamp,
                       const OSVR_OrientationReport &report);
    void handleVideoTrackerDataWhileRunning(const OSVR_TimeValue &timestamp,
                                            const OSVR_PoseReport &report);
    void handleVideoTrackerDataDuringStartup(
        const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report,
        const OSVR_OrientationState &orientation);

    bool running() const { return m_state == State::Running; }

    OSVR_PoseState const &getLatestFusedPose() const { return m_lastFusion; }
    osvr::util::time::TimeValue const &getLatestFusedTime() const {
        return m_lastFusionTime;
    }

    OSVR_PoseState const &getLatestReorientedVideoPose() const {
        return m_reorientedVideo;
    }

    OSVR_PoseState const &getLatestCameraPose() const { return m_camera; }

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
    OSVR_PoseState m_lastFusion;
    osvr::util::time::TimeValue m_lastFusionTime;
};

#endif // INCLUDED_VideoIMUFusion_h_GUID_85338EA5_58E6_4787_16D2_EC53201EFE9F
