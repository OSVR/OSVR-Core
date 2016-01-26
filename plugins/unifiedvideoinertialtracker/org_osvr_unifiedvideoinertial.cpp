/** @file
    @brief Main file for a unified video-based and inertial tracking system.

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "ImageSources/ImageSource.h"
#include "ImageSources/ImageSourceFactories.h"

#include "MakeHDKTrackingSystem.h"
#include "TrackingSystem.h"

#include "HDKLedIdentifierFactory.h"
#include "CameraParameters.h"
#include "HDKData.h"
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>

#include "ConfigurationParser.h"

// Generated JSON header file
#include "org_osvr_unifiedvideoinertial_json.h"

// Library/third-party includes
#include <opencv2/core/core.hpp> // for basic OpenCV types
#include <opencv2/core/operations.hpp>
#include <opencv2/highgui/highgui.hpp> // for image capture
#include <opencv2/imgproc/imgproc.hpp> // for image scaling
#include <json/value.h>
#include <json/reader.h>

#include <boost/noncopyable.hpp>

// Standard includes
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <thread>
#include <mutex>
#include <stdexcept>

// Anonymous namespace to avoid symbol collision
namespace {
static const auto DRIVER_NAME = "UnifiedTrackingSystem";

static const auto DEBUGGABLE_BEACONS = 34;
static const auto DATAPOINTS_PER_BEACON = 5;

class UnifiedVideoInertialTracker : boost::noncopyable {
  public:
    UnifiedVideoInertialTracker(OSVR_PluginRegContext ctx,
                                osvr::vbtracker::ImageSourcePtr &&source,
                                osvr::vbtracker::ConfigParams params)
        : m_source(std::move(source)) {

        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // Configure the tracker interface.
        osvrDeviceTrackerConfigure(opts, &m_tracker);
#if 0
        osvrDeviceAnalogConfigure(opts, &m_analog,
                                  DEBUGGABLE_BEACONS * DATAPOINTS_PER_BEACON);
#endif

        /// Create the analysis device token with the options
        OSVR_DeviceToken dev;
        if (OSVR_RETURN_FAILURE ==
            osvrAnalysisSyncInit(ctx, DRIVER_NAME, opts, &dev, &m_clientCtx)) {
            throw std::runtime_error("Could not initialize analysis plugin!");
        }
        m_dev = osvr::pluginkit::DeviceToken(dev);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(org_osvr_unifiedvideoinertial_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update();

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_ClientContext m_clientCtx;
    OSVR_ClientInterface m_clientInterface;
    OSVR_TrackerDeviceInterface m_tracker;
#if 0
    OSVR_AnalogDeviceInterface m_analog;
#endif
    osvr::vbtracker::ImageSourcePtr m_source;
    cv::Mat m_frame;
    cv::Mat m_imageGray;
    std::unique_ptr<osvr::vbtracker::TrackingSystem> m_trackingSystem;
};

inline OSVR_ReturnCode UnifiedVideoInertialTracker::update() {
    if (!m_source->ok()) {
        // Couldn't open the camera.  Failing silently for now. Maybe the
        // camera will be plugged back in later.
        return OSVR_RETURN_SUCCESS;
    }

    return OSVR_RETURN_SUCCESS;
}

class ConfiguredDeviceConstructor {
  public:
    /// @brief This is the required signature for a device instantiation
    /// callback.
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        // Read the JSON data from parameters.
        Json::Value root;
        if (params) {
            Json::Reader r;
            if (!r.parse(params, root)) {
                std::cerr << "Could not parse parameters!" << std::endl;
            }
        }

        // Read these parameters from a "params" field in the device Json
        // configuration file.

        // This is in a separate function/header for sharing and for clarity.
        auto config = osvr::vbtracker::parseConfigParams(root);

#ifdef _WIN32
        auto cam = osvr::vbtracker::openHDKCameraDirectShow();
#else // !_WIN32
        /// @todo This is rather crude, as we can't select the exact camera we
        /// want, nor set the "50Hz" high-gain mode (and only works with HDK
        /// camera firmware v7 and up). Presumably eventually use libuvc on
        /// other platforms instead, at least for the HDK IR camera.

        auto cam = osvr::vbtracker::openOpenCVCamera(0);
#endif

        if (!cam || !cam->ok()) {
            std::cerr << "Could not access the tracking camera, skipping "
                         "video-based tracking!"
                      << std::endl;
            return OSVR_RETURN_FAILURE;
        }

        auto trackingSystem = osvr::vbtracker::makeHDKTrackingSystem(config);
        // OK, now that we have our parameters, create the device.
        osvr::pluginkit::PluginContext context(ctx);
        auto newTracker = osvr::pluginkit::registerObjectForDeletion(
            ctx, new UnifiedVideoInertialTracker(ctx, std::move(cam), config));

        return OSVR_RETURN_SUCCESS;
    }
};

} // namespace

OSVR_PLUGIN(org_osvr_unifiedvideoinertial) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Tell the core we're available to create a device object.
    osvr::pluginkit::registerDriverInstantiationCallback(
        ctx, DRIVER_NAME, new ConfiguredDeviceConstructor);

    return OSVR_RETURN_SUCCESS;
}
