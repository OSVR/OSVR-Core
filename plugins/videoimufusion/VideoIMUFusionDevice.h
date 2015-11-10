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

#ifndef INCLUDED_VideoIMUFusionDevice_h_GUID_477141AB_39F3_489A_8C15_BF558BECB7E0
#define INCLUDED_VideoIMUFusionDevice_h_GUID_477141AB_39F3_489A_8C15_BF558BECB7E0

// Internal Includes
#include "VideoIMUFusion.h"
#include "FusionParams.h"
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/ClientKit/InterfaceC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

/// Wrapper for VideoIMUFusion that handles the specifics of being an analysis
/// plugin. Forwards events to the nested VideoIMUFusion instance, and reports
/// its state as appropriate.
class VideoIMUFusionDevice {
  public:
    VideoIMUFusionDevice(
        OSVR_PluginRegContext ctx, std::string const &name,
        std::string const &imuPath, std::string const &videoPath,
        VideoIMUFusionParams const &params = VideoIMUFusionParams());
    ~VideoIMUFusionDevice();
    OSVR_ReturnCode update() { return OSVR_RETURN_SUCCESS; }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
  private:
    static void s_handleIMUData(void *userdata, const OSVR_TimeValue *timestamp,
                                const OSVR_OrientationReport *report);
    static void s_handleIMUVelocity(void *userdata,
                                    const OSVR_TimeValue *timestamp,
                                    const OSVR_AngularVelocityReport *report);
    static void s_handleVideoTrackerData(void *userdata,
                                         const OSVR_TimeValue *timestamp,
                                         const OSVR_PoseReport *report);
    void handleIMUData(const OSVR_TimeValue &timestamp,
                       const OSVR_OrientationReport &report);

    void handleIMUVelocity(const OSVR_TimeValue &timestamp,
                           const OSVR_AngularVelocityReport &report);
    void handleVideoTrackerData(const OSVR_TimeValue &timestamp,
                                const OSVR_PoseReport &report);

    void sendMainPoseReport();

    OSVR_TrackerDeviceInterface m_trackerOut;
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_ClientContext m_clientCtx;

    OSVR_ClientInterface m_imu = nullptr;

    OSVR_ClientInterface m_videoTracker = nullptr;

    VideoIMUFusion m_fusion;
};

#endif // INCLUDED_VideoIMUFusionDevice_h_GUID_477141AB_39F3_489A_8C15_BF558BECB7E0
