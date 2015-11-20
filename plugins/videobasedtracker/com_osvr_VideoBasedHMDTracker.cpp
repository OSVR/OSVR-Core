/** @file
    @brief Main file for a video-based HMD tracker.

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
#include "VideoBasedTracker.h"
#include "HDKLedIdentifierFactory.h"
#include "CameraParameters.h"
#include "ImageSource.h"
#include "ImageSourceFactories.h"
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_VideoBasedHMDTracker_json.h"

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
#include <iomanip>
#include <sstream>
#include <memory>

// Define the constant below to print timing information (how many updates
// per second we are getting).
//#define VBHMD_TIMING

// Define the constant below to set a directory to save the video frames that
// are acquired with files in a format that can later be read by
// VBHMD_FAKE_IMAGES
//#define VBHMD_SAVE_IMAGES "./Frames"

// Anonymous namespace to avoid symbol collision
namespace {

class VideoBasedHMDTracker : boost::noncopyable {
  public:
    VideoBasedHMDTracker(OSVR_PluginRegContext ctx,
                         osvr::vbtracker::ImageSourcePtr &&source,
                         int devNumber = 0, bool showDebug = false)
        : m_source(std::move(source)), m_vbtracker(showDebug) {
        // Set the number of threads for OpenCV to use.
        cv::setNumThreads(1);

        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // Configure the tracker interface.
        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Come up with a device name
        std::ostringstream os;
        os << "TrackedCamera" << devNumber << "_" << 0;

        /// Create an asynchronous (threaded) device
        m_dev.initAsync(ctx, os.str(), opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_VideoBasedHMDTracker_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        if (!m_source->ok()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        //==================================================================
        // Trigger a camera grab.
        if (!m_source->grab()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        //==================================================================
        // Keep track of when we got the image, since that is our
        // best estimate for when the tracker was at the specified
        // pose.
        // TODO: Back-date the aquisition time by the expected image
        // transfer time and perhaps by half the exposure time to say
        // when the photons actually arrived.
        OSVR_TimeValue timestamp;
        osvrTimeValueGetNow(&timestamp);
        // Pull the image into an OpenCV matrix named m_frame.
        m_source->retrieve(m_frame, m_imageGray);

#ifdef VBHMD_SAVE_IMAGES
        // If we're supposed to save images, make file names that match the
        // format we need to read them back in again and save the images.
        std::ostringstream fileName;
        fileName << VBHMD_SAVE_IMAGES << "/";
        fileName << std::setfill('0') << std::setw(4) << m_imageNum++;
        fileName << ".tif";
        if (!cv::imwrite(fileName.str().c_str(), m_frame)) {
            std::cerr << "Could not write image to " << fileName.str()
                      << std::endl;
        }

#endif

#ifdef VBHMD_TIMING
        //==================================================================
        // Time our performance
        static struct timeval last = {0, 0};
        if (last.tv_sec == 0) {
            vrpn_gettimeofday(&last, NULL);
        }
        static unsigned count = 0;
        if (++count == 100) {
            struct timeval now;
            vrpn_gettimeofday(&now, NULL);
            double duration = vrpn_TimevalDurationSeconds(now, last);
            std::cout << "Video-based tracker: update rate " << count / duration
                      << " hz" << std::endl;
            count = 0;
            last = now;
        }
#endif

        m_vbtracker.processImage(
            m_frame, m_imageGray,
            [&](OSVR_ChannelCount sensor, OSVR_Pose3 const &pose) {

                //==================================================================
                // Report the new pose, time-stamped with the time we
                // received the image from the camera.
                osvrDeviceTrackerSendPoseTimestamped(m_dev, m_tracker, &pose,
                                                     sensor, &timestamp);
            });

        return OSVR_RETURN_SUCCESS;
    }

    /// Should be called immediately after construction for specifying the
    /// particulars of tracking.
    void addSensor(osvr::vbtracker::LedIdentifierPtr &&identifier,
                   osvr::vbtracker::CameraParameters const &params,
                   osvr::vbtracker::Point3Vector const &locations,
                   size_t requiredInliers = 4, size_t permittedOutliers = 2) {
        m_vbtracker.addSensor(std::move(identifier), params.cameraMatrix,
                              params.distortionParameters, locations,
                              requiredInliers, permittedOutliers);
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    osvr::vbtracker::ImageSourcePtr m_source;
#ifdef VBHMD_SAVE_IMAGES
    int m_imageNum = 1;
#endif
    cv::Mat m_frame;
    cv::Mat m_imageGray;

    osvr::vbtracker::VideoBasedTracker m_vbtracker;
};

class HardwareDetection {
  public:
    using CameraFactoryType = std::function<osvr::vbtracker::ImageSourcePtr()>;
    using SensorSetupType = std::function<void(VideoBasedHMDTracker &)>;
    HardwareDetection(CameraFactoryType camFactory, SensorSetupType setup,
                      int cameraID = 0, bool showDebug = false)
        : m_found(false), m_cameraFactory(camFactory), m_sensorSetup(setup),
          m_cameraID(cameraID), m_showDebug(showDebug) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }
        auto src = m_cameraFactory();
        if (!src || !src->ok()) {
            return OSVR_RETURN_FAILURE;
        }
        m_found = true;

        /// Create our device object, passing the context and moving the camera.
        std::cout << "Opening camera " << m_cameraID << std::endl;
        auto newTracker = osvr::pluginkit::registerObjectForDeletion(
            ctx, new VideoBasedHMDTracker(ctx, std::move(src), m_cameraID,
                                          m_showDebug));
        m_sensorSetup(*newTracker);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance, so that only one tracker will use this camera.)
    bool m_found = false;

    CameraFactoryType m_cameraFactory;
    SensorSetupType m_sensorSetup;

    int m_cameraID;   //< Which OpenCV camera should we open?
    bool m_showDebug; //< Show windows with video to help debug?
};

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

        // Using `get` here instead of `[]` lets us provide a default value.
        int cameraID = root.get("cameraID", 0).asInt();
        bool showDebug = root.get("showDebug", false).asBool();

        /// @todo get this (and the path) from the config file
        bool fakeImages = false;
        if (fakeImages) {
            /// Immediately create a "fake images" tracker.
            auto path = std::string{};
            // fake images
            auto src = osvr::vbtracker::openImageFileSequence(path);
            if (!src) {
                return OSVR_RETURN_FAILURE;
            }
            auto newTracker = osvr::pluginkit::registerObjectForDeletion(
                ctx, new VideoBasedHMDTracker(ctx, std::move(src), cameraID,
                                              showDebug));
            auto camParams = osvr::vbtracker::getSimulatedHDKCameraParameters();
            newTracker->addSensor(
                osvr::vbtracker::createHDKLedIdentifierSimulated(0), camParams,
                osvr::vbtracker::OsvrHdkLedLocations_SENSOR0, 4, 2);
            // There are sometimes only four beacons on the back unit (two of
            // the LEDs are disabled), so we let things work with just those.
            newTracker->addSensor(
                osvr::vbtracker::createHDKLedIdentifierSimulated(1), camParams,
                osvr::vbtracker::OsvrHdkLedLocations_SENSOR1, 4, 0);
            return OSVR_RETURN_SUCCESS;
        }
#if 0
        bool isOculusCamera = (width == 376) && (height == 480);
#endif

#ifdef _WIN32
        /// @todo speed of a function pointer here vs a lambda?
        auto cameraFactory = &osvr::vbtracker::openHDKCameraDirectShow;

#else // !_WIN32
        /// @todo This is rather crude, as we can't select the exact camera we
        /// want, nor set the "50Hz" high-gain mode (and only works with HDK
        /// camera firmware v7 and up). Presumably eventually use libuvc on
        /// other platforms instead, at least for the HDK IR camera.

        auto cameraFactory = [=] {
            return osvr::vbtracker::openOpenCVCamera(cameraID);
        };
#endif

        /// Function to execute after the device is created, to add the sensors.
        auto setupHDKParamsAndSensors = [](VideoBasedHMDTracker &newTracker) {
            auto camParams = osvr::vbtracker::getHDKCameraParameters();
            newTracker.addSensor(
                osvr::vbtracker::createHDKLedIdentifier(0), camParams,
                osvr::vbtracker::OsvrHdkLedLocations_SENSOR0, 6, 0);
            newTracker.addSensor(
                osvr::vbtracker::createHDKLedIdentifier(1), camParams,
                osvr::vbtracker::OsvrHdkLedLocations_SENSOR1, 4, 0);
        };

        // OK, now that we have our parameters, create the device.
        osvr::pluginkit::PluginContext context(ctx);
        context.registerHardwareDetectCallback(new HardwareDetection(
            cameraFactory, setupHDKParamsAndSensors, cameraID, showDebug));

        return OSVR_RETURN_SUCCESS;
    }
};

} // namespace

OSVR_PLUGIN(com_osvr_VideoBasedHMDTracker) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Tell the core we're available to create a device object.
    osvr::pluginkit::registerDriverInstantiationCallback(
        ctx, "VideoBasedHMDTracker", new ConfiguredDeviceConstructor);

    return OSVR_RETURN_SUCCESS;
}
