/** @file
    @brief Implementation

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

// Internal Includes
#include "VideoIMUFusionDevice.h"
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/ClientKit/InterfaceStateC.h>

// Generated JSON header file
#include "org_osvr_filter_videoimufusion_json.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

static const OSVR_ChannelCount FUSED_SENSOR_ID = 0;
static const OSVR_ChannelCount TRANSFORMED_VIDEO_SENSOR_ID = 1;

using osvr::pluginkit::wrapCallback;

VideoIMUFusionDevice::VideoIMUFusionDevice(OSVR_PluginRegContext ctx,
                                           std::string const &name,
                                           std::string const &imuPath,
                                           std::string const &videoPath) {
    /// Create the initialization options
    OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

    osvrDeviceTrackerConfigure(opts, &m_trackerOut);

    /// Create the device token with the options
    OSVR_DeviceToken dev;
    if (OSVR_RETURN_FAILURE ==
        osvrAnalysisSyncInit(ctx, name.c_str(), opts, &dev, &m_clientCtx)) {
        throw std::runtime_error("Could not initialize analysis plugin!");
    }
    m_dev = osvr::pluginkit::DeviceToken(dev);

    /// Send JSON descriptor
    m_dev.sendJsonDescriptor(org_osvr_filter_videoimufusion_json);

    /// Register update callback
    m_dev.registerUpdateCallback(this);

    /// Set up to receive our input.
    osvrClientGetInterface(m_clientCtx, imuPath.c_str(), &m_imu);
    auto imuCb = wrapCallback<OSVR_OrientationReport>([&](
        const OSVR_TimeValue *timestamp, const OSVR_OrientationReport *report) {
        handleIMUData(*timestamp, *report);
    });
    osvrRegisterOrientationCallback(m_imu, imuCb.first, imuCb.second.get());
    m_imuCb = std::move(imuCb.second);

    osvrClientGetInterface(m_clientCtx, videoPath.c_str(), &m_videoTracker);
    auto videoTrackerCb = wrapCallback<OSVR_PoseReport>(
        [&](const OSVR_TimeValue *timestamp, const OSVR_PoseReport *report) {
            handleVideoTrackerData(*timestamp, *report);
        });
    osvrRegisterPoseCallback(m_videoTracker, videoTrackerCb.first,
                             videoTrackerCb.second.get());
    m_videoTrackerCb = std::move(videoTrackerCb.second);
}

VideoIMUFusionDevice::~VideoIMUFusionDevice() {
    /// free the interfaces before the pointed-to function objects
    /// disappear.
    if (m_imu) {
        osvrClientFreeInterface(m_clientCtx, m_imu);
        m_imu = nullptr;
    }
    if (m_videoTracker) {
        osvrClientFreeInterface(m_clientCtx, m_videoTracker);
        m_imu = nullptr;
    }
}

void VideoIMUFusionDevice::handleIMUData(const OSVR_TimeValue &timestamp,
                                         const OSVR_OrientationReport &report) {
    m_fusion.handleIMUData(timestamp, report);
    if (m_fusion.running()) {
        sendMainPoseReport();
    }
}
void VideoIMUFusionDevice::handleVideoTrackerData(
    const OSVR_TimeValue &timestamp, const OSVR_PoseReport &report) {
    if (!m_fusion.running()) {
        auto ts = OSVR_TimeValue{};
        auto oriState = OSVR_OrientationState{};
        auto ret = osvrGetOrientationState(m_imu, &ts, &oriState);
        if (ret != OSVR_RETURN_SUCCESS) {
            std::cout << "Got a video report before an IMU report, ignoring it"
                      << std::endl;
            return;
        }
        m_fusion.handleVideoTrackerDataDuringStartup(timestamp, report,
                                                     oriState);
        return;
    }
    m_fusion.handleVideoTrackerDataWhileRunning(timestamp, report);
    sendMainPoseReport();
    osvrDeviceTrackerSendPoseTimestamped(
        m_dev, m_trackerOut, &m_fusion.getLatestReorientedVideoPose(),
        TRANSFORMED_VIDEO_SENSOR_ID, &timestamp);
}

void VideoIMUFusionDevice::sendMainPoseReport() {
    osvrDeviceTrackerSendPoseTimestamped(
        m_dev, m_trackerOut, &m_fusion.getLatestFusedPose(),
        TRANSFORMED_VIDEO_SENSOR_ID, &m_fusion.getLatestFusedTime());
}
