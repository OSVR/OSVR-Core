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
#include "ThreadsafeBodyReporting.h"

#include "HDKLedIdentifierFactory.h"
#include "CameraParameters.h"
#include "HDKData.h"
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>

#include <osvr/Util/EigenInterop.h>

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

#include <util/Stride.h>

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
using TrackingSystemPtr = std::unique_ptr<osvr::vbtracker::TrackingSystem>;
using BodyReportingVector = std::vector<osvr::vbtracker::BodyReportingPtr>;
class TrackerThread : boost::noncopyable {
  public:
    TrackerThread(osvr::vbtracker::TrackingSystem &trackingSystem,
                  osvr::vbtracker::ImageSource &imageSource,
                  BodyReportingVector &reportingVec,
                  osvr::vbtracker::CameraParameters const &camParams)
        : m_trackingSystem(trackingSystem), m_cam(imageSource),
          m_reportingVec(reportingVec), m_camParams(camParams) {
        msg() << "Tracker thread object created." << std::endl;
    }

    void operator()() {
        msg() << "Tracker thread object invoked." << std::endl;
        bool keepGoing = true;
        while (keepGoing) {
            doFrame();

            {
                /// Copy the run flag.
                std::lock_guard<std::mutex> lock(m_runMutex);
                keepGoing = m_run;
            }
            if (!keepGoing) {
                msg() << "Tracker thread object: Just checked our run flag and "
                         "noticed it turned false..."
                      << std::endl;
            }
        }
        msg() << "Tracker thread object: functor exiting." << std::endl;
    }

    /// Call from the main thread!
    void triggerStop() {
        msg() << "Tracker thread object: triggerStop() called" << std::endl;
        std::lock_guard<std::mutex> lock(m_runMutex);
        m_run = false;
    }

  private:
    std::ostream &msg() const { return std::cout << "[UnifiedTracker] "; }
    std::ostream &warn() const { return msg() << "Warning: "; }
    void doFrame() {
        // Check camera status.
        if (!m_cam.ok()) {
            // Hmm, camera seems bad. Might regain it? Skip for now...
            warn() << "Camera is reporting it is not OK." << std::endl;
            return;
        }
        // Trigger a grab.
        if (!m_cam.grab()) {
            // Again failing without quitting, in hopes we get better luck next
            // time...
            warn() << "Camera grab failed." << std::endl;
            return;
        }
        // When we triggered the grab is our current best guess of the time for
        // the image
        /// @todo backdate to account for image transfer image, exposure time,
        /// etc.
        auto tv = osvr::util::time::getNow();

        // Pull the image into an OpenCV matrix named m_frame.
        m_cam.retrieve(m_frame, m_frameGray);
        if (!m_frame.data || !m_frameGray.data) {
            warn()
                << "Camera retrieve appeared to fail: frames had null pointers!"
                << std::endl;
            return;
        }

        // Submit to the tracking system.
        auto bodyIds = m_trackingSystem.processFrame(tv, m_frame, m_frameGray,
                                                     m_camParams);

        for (auto const &bodyId : bodyIds) {
            auto &body = m_trackingSystem.getBody(bodyId);
            m_reportingVec[bodyId.value()]->updateState(
                body.getStateTime(), body.getState(), body.getProcessModel());
        }
    }
    osvr::vbtracker::TrackingSystem &m_trackingSystem;
    osvr::vbtracker::ImageSource &m_cam;
    BodyReportingVector &m_reportingVec;
    osvr::vbtracker::CameraParameters m_camParams;
    cv::Mat m_frame;
    cv::Mat m_frameGray;
    std::mutex m_runMutex;
    volatile bool m_run = true;
};

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

        /// Set up thread communication.
        setupBodyReporting();

        /// Set up the object that will run in the other thread, and spawn the
        /// thread.
        startTrackerThread();

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    ~UnifiedVideoInertialTracker() { stopTrackerThread(); }

    /// Create a "BodyReporting" interchange structure for each body we track.
    void setupBodyReporting() {
        m_bodyReportingVector.clear();
        auto n = m_trackingSystem->getNumBodies();
        for (size_type i = 0; i < n; ++i) {
            m_bodyReportingVector.emplace_back(
                osvr::vbtracker::BodyReporting::make());
        }
    }

    OSVR_ReturnCode update();

    void startTrackerThread() {
        if (m_trackerThreadFunctor) {
            throw std::logic_error("Trying to start the tracker thread when "
                                   "it's already started!");
        }
        std::cout << "Starting the tracker thread..." << std::endl;
        m_trackerThreadFunctor.reset(new TrackerThread(
            *m_trackingSystem, *m_source, m_bodyReportingVector,
            osvr::vbtracker::getHDKCameraParameters()));
        m_trackerThread = std::thread([&] { (*m_trackerThreadFunctor)(); });
    }
    void stopTrackerThread() {
        if (m_trackerThreadFunctor) {
            std::cout << "Shutting down the tracker thread..." << std::endl;
            m_trackerThreadFunctor->triggerStop();
            m_trackerThread.join();
            m_trackerThreadFunctor.reset();
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
    const double m_additionalPrediction;
    std::vector<osvr::vbtracker::BodyReportingPtr> m_bodyReportingVector;
    std::unique_ptr<TrackerThread> m_trackerThreadFunctor;
    std::thread m_trackerThread;
};

inline OSVR_ReturnCode UnifiedVideoInertialTracker::update() {
    namespace ei = osvr::util::eigen_interop;
    std::size_t numSensors = m_bodyReportingVector.size();
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
