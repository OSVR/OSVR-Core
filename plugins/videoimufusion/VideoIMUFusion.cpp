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
#include "VideoIMUFusion.h"
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>

// Generated JSON header file
#include "org_osvr_filter_videoimufusion_json.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace detail {
template <typename ReportType>
void callbackCaller(void *userdata, const OSVR_TimeValue *timestamp,
                    const ReportType *report) {
    auto &f = *static_cast<WrappedCallbackFunction<ReportType> *>(userdata);
    f(timestamp, report);
}
template <typename ReportType> struct CallbackType_impl {
    typedef void (*type)(void *, const OSVR_TimeValue *, const ReportType *);
};
template <typename ReportType>
using CallbackType = typename CallbackType_impl<ReportType>::type;

template <typename ReportType> inline CallbackType<ReportType> getCaller() {
    return &callbackCaller<ReportType>;
}

template <typename ReportType, typename F>
inline std::pair<CallbackType<ReportType>, WrappedCallbackPtr<ReportType>>
wrapCallback(F &&f) {
    auto functor = WrappedCallbackPtr<ReportType>{
        new WrappedCallbackFunction<ReportType>{std::forward<F>(f)}};
    return std::make_pair(getCaller<ReportType>(), std::move(functor));
}
} // namespace detail
using detail::wrapCallback;

VideoIMUFusion::VideoIMUFusion(OSVR_PluginRegContext ctx,
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

    enterCameraPoseAcquisitionState();
}
VideoIMUFusion::~VideoIMUFusion() {
    /// free the interfaces before the pointed-to function objects disappear.
    if (m_imu) {
        osvrClientFreeInterface(m_clientCtx, m_imu);
        m_imu = nullptr;
    }
    if (m_videoTracker) {
        osvrClientFreeInterface(m_clientCtx, m_videoTracker);
        m_imu = nullptr;
    }
}

OSVR_ReturnCode VideoIMUFusion::update() { return OSVR_RETURN_SUCCESS; }

void VideoIMUFusion::enterCameraPoseAcquisitionState() {}

void VideoIMUFusion::enterRunningState() {}

void VideoIMUFusion::handleIMUData(const OSVR_TimeValue &timestamp,
                                   const OSVR_OrientationReport &report) {}
void VideoIMUFusion::handleVideoTrackerData(const OSVR_TimeValue &timestamp,
                                            const OSVR_PoseReport &report) {}
