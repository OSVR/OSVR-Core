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
#include "MakeHDKTrackingSystem.h"
#include "HDKData.h"
#include "ConfigurationParser.h"
#include "TrackerThread.h"

// ImageSources mini-library
#include "ImageSources/ImageSource.h"
#include "ImageSources/ImageSourceFactories.h"

// uvbi-core mini-library
#include "TrackingSystem.h"
#include "ThreadsafeBodyReporting.h"
#include "CameraParameters.h"

#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>

#include <osvr/Util/EigenInterop.h>

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
#include <boost/variant.hpp>

#include <util/Stride.h>

// Standard includes
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <stdexcept>

// Anonymous namespace to avoid symbol collision
namespace {
static const auto DRIVER_NAME = "UnifiedTrackingSystem";

static const auto DEBUGGABLE_BEACONS = 34;
static const auto DATAPOINTS_PER_BEACON = 5;
using TrackingSystemPtr = std::unique_ptr<osvr::vbtracker::TrackingSystem>;
using osvr::vbtracker::TrackerThread;
using osvr::vbtracker::BodyReportingVector;
using osvr::vbtracker::TrackedBody;
using osvr::vbtracker::TrackedBodyIMU;
using osvr::vbtracker::BodyId;

class UnifiedVideoInertialTracker : boost::noncopyable {
  public:
    using size_type = std::size_t;
    UnifiedVideoInertialTracker(OSVR_PluginRegContext ctx,
                                osvr::vbtracker::ImageSourcePtr &&source,
                                osvr::vbtracker::ConfigParams params,
                                TrackingSystemPtr &&trackingSystem)
        : m_source(std::move(source)),
          m_trackingSystem(std::move(trackingSystem)),
          m_additionalPrediction(params.additionalPrediction) {
        if (params.numThreads > 0) {
            // Set the number of threads for OpenCV to use.
            cv::setNumThreads(params.numThreads);
        }

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

        m_mainBody = &(m_trackingSystem->getBody(BodyId(0)));
        if (m_mainBody->hasIMU()) {
            m_imu = &(m_mainBody->getIMU());
            /// Create our client interface and register a callback.
            if (OSVR_RETURN_FAILURE ==
                osvrClientGetInterface(m_clientCtx, params.imu.path.c_str(),
                                       &m_clientInterface)) {
                throw std::runtime_error(
                    "Could not get client interface for analysis plugin!");
            }
            osvrRegisterOrientationCallback(
                m_clientInterface, &UnifiedVideoInertialTracker::oriCallback,
                this);
            osvrRegisterAngularVelocityCallback(
                m_clientInterface, &UnifiedVideoInertialTracker::angVelCallback,
                this);
        }

        /// Set up thread communication.
        setupBodyReporting();

        /// Set up the object that will run in the other thread, and spawn the
        /// thread.
        startTrackerThread();

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    /// Processes a tracker report.
    template <typename ReportType>
    void handleData(OSVR_TimeValue const &timestamp, ReportType const &report) {
        m_trackerThreadManager->submitIMUReport(*m_imu, timestamp, report);
    }

    static void oriCallback(void *userdata, const OSVR_TimeValue *timestamp,
                            const OSVR_OrientationReport *report) {
        auto &self = *static_cast<UnifiedVideoInertialTracker *>(userdata);
        self.handleData(*timestamp, *report);
    }
    static void angVelCallback(void *userdata, const OSVR_TimeValue *timestamp,
                               const OSVR_AngularVelocityReport *report) {
        auto &self = *static_cast<UnifiedVideoInertialTracker *>(userdata);
        self.handleData(*timestamp, *report);
    }

    ~UnifiedVideoInertialTracker() { stopTrackerThread(); }

    /// Create a "BodyReporting" interchange structure for each body we track.
    void setupBodyReporting() {
        m_bodyReportingVector.clear();
        static const auto EXTRA_TRACKING_SENSORS = 3;
        // Add three extra sensors: one for camera, one for corrected imu, and
        // one for imu in camera space.
        auto n = m_trackingSystem->getNumBodies() + EXTRA_TRACKING_SENSORS;
        for (size_type i = 0; i < n; ++i) {
            m_bodyReportingVector.emplace_back(
                osvr::vbtracker::BodyReporting::make());
        }
    }

    OSVR_ReturnCode update();

    void startTrackerThread() {
        if (m_trackerThreadManager) {
            throw std::logic_error("Trying to start the tracker thread when "
                                   "it's already started!");
        }
        std::cout << "Starting the tracker thread..." << std::endl;
        m_trackerThreadManager.reset(new TrackerThread(
            *m_trackingSystem, *m_source, m_bodyReportingVector,
            osvr::vbtracker::getHDKCameraParameters()));

        /// This will start the thread, but it won't enter its full main loop
        /// until we call permitStart()
        m_trackerThread =
            std::thread([&] { m_trackerThreadManager->threadAction(); });
    }
    void stopTrackerThread() {
        if (m_trackerThreadManager) {
            std::cout << "Shutting down the tracker thread..." << std::endl;
            m_trackerThreadManager->triggerStop();
            if (m_trackerThread.joinable()) {
                m_trackerThread.join();
            }
            m_trackerThreadManager.reset();
            m_trackerThread = std::thread();
        }
    }

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
    TrackingSystemPtr m_trackingSystem;
    /// @todo kind-of assumes there's only one body.
    TrackedBody *m_mainBody = nullptr;
    /// @todo kind-of assumes there's only one body.
    TrackedBodyIMU *m_imu = nullptr;
    const double m_additionalPrediction;
    BodyReportingVector m_bodyReportingVector;
    std::unique_ptr<TrackerThread> m_trackerThreadManager;
    bool m_threadLoopStarted = false;
    std::thread m_trackerThread;
};

inline OSVR_ReturnCode UnifiedVideoInertialTracker::update() {
    if (!m_threadLoopStarted) {
        /// First call to update, we let the tracker go flying.
        m_threadLoopStarted = true;
        m_trackerThreadManager->permitStart();
        return OSVR_RETURN_SUCCESS;
    }
    namespace ei = osvr::util::eigen_interop;
    std::size_t numSensors = m_bodyReportingVector.size();
    /// On each update pass, we go through and attempt to report for every body,
    /// at the current time + additional prediction as requested.
    for (std::size_t i = 0; i < numSensors; ++i) {
        auto report =
            m_bodyReportingVector[i]->getReport(m_additionalPrediction);
        if (!report) {
            /// couldn't get a report for this sensor for one reason or another.
            // std::cout << "Couldn't get report for " << i << std::endl;
            continue;
        }
        osvrDeviceTrackerSendPoseTimestamped(m_dev, m_tracker, &report.pose, i,
                                             &report.timestamp);
    }
    return OSVR_RETURN_SUCCESS;
}

class ConfiguredDeviceConstructor {
  public:
    /// @brief This is the required signature for a device instantiation
    /// callback.
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {

        std::cerr << "\n\nWARNING: The 'unifiedvideoinertial' tracking plugin "
                     "is highly experimental and\n"
                  << "NOT currently recommended for use except by those "
                     "working on its development!\n"
                  << "Users should instead use the combination of the "
                     "'VideoBasedHMDTracker' and\n"
                  << "the 'VideoIMUFusion' plugins, as found in the HDK13 "
                     "config files.\n\n"
                  << std::endl;

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
            ctx, new UnifiedVideoInertialTracker(ctx, std::move(cam), config,
                                                 std::move(trackingSystem)));

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
