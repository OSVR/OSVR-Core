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
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/ClientKit/InterfaceC.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>
#include <memory>

namespace detail {
template <typename ReportType>
using WrappedCallbackFunction =
    std::function<void(const OSVR_TimeValue *, const ReportType *)>;

template <typename ReportType>
using WrappedCallbackPtr = std::unique_ptr<WrappedCallbackFunction<ReportType>>;
} // namespace detail

class VideoIMUFusion {
  public:
    VideoIMUFusion(OSVR_PluginRegContext ctx, std::string const &name,
                   std::string const &imuPath, std::string const &videoPath);
    ~VideoIMUFusion();
    OSVR_ReturnCode update();
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  private:
    void handleIMUData(const OSVR_TimeValue &timestamp,
                       const OSVR_OrientationReport &report);
    void handleVideoTrackerData(const OSVR_TimeValue &timestamp,
                                const OSVR_PoseReport &report);
    void handleVideoTrackerDataDuringStartup(const OSVR_TimeValue &timestamp,
                                             const OSVR_PoseReport &report);

    void enterCameraPoseAcquisitionState();
    void enterRunningState(Eigen::Isometry3d const &cTr);

    OSVR_TrackerDeviceInterface m_trackerOut;
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_ClientContext m_clientCtx;

    OSVR_ClientInterface m_imu = nullptr;
    detail::WrappedCallbackPtr<OSVR_OrientationReport> m_imuCb;

    OSVR_ClientInterface m_videoTracker = nullptr;
    detail::WrappedCallbackPtr<OSVR_PoseReport> m_videoTrackerCb;

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
};

#endif // INCLUDED_VideoIMUFusion_h_GUID_85338EA5_58E6_4787_16D2_EC53201EFE9F
