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
#include "ConfigurationParser.h"
#include "ImageSourceFactories.h"
#include "VideoBasedTracker.h"
#include <osvr/Server/ConfigureServerFromFile.h>
#include "HDKData.h"
#include "HDKLedIdentifierFactory.h"
#include "CameraParameters.h"

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>
#include <json/reader.h>
#include <json/value.h>

// Standard includes
#include <memory>
#include <iostream>

using std::endl;
using namespace osvr::vbtracker;

static osvr::server::detail::StreamPrefixer
    out("[OSVR Video Tracker Calibration] ", std::cout);
static osvr::server::detail::StreamPrefixer
    err("[OSVR Video Tracker Calibration] ", std::cerr);

/// @brief OpenCV's simple highgui module refers to windows by their name, so we
/// make this global for a simpler demo.
static const std::string windowNameAndInstructions(
    "OSVR tracking camera preview | q or esc to save calibration and quit");

static int withAnError() {
    err << "\n";
    err << "Press enter to exit." << endl;
    std::cin.ignore();
    return -1;
}
static Json::Value findVideoTrackerParams(Json::Value const &drivers) {
    if (!drivers.isArray()) {
        return Json::Value(Json::nullValue);
    }
    for (auto const &entry : drivers) {
        if (entry["plugin"] == "com_osvr_VideoBasedHMDTracker" &&
            entry["driver"] == "VideoBasedHMDTracker") {
            return entry["params"];
        }
    }
    return Json::Value(Json::nullValue);
}

class TrackerCalibrationApp {
  public:
    TrackerCalibrationApp(ImageSourcePtr &&src,
                          osvr::vbtracker::ConfigParams params)
        : m_src(std::move(src)), m_params(params), m_vbtracker(params) {

        cv::namedWindow(windowNameAndInstructions);
    }

    osvr::vbtracker::VideoBasedTracker &vbtracker() { return m_vbtracker; }

    void run() {
        /// Turn off the Kalman filter until the user has brought the HMD close.
        vbtracker().getFirstEstimator().permitKalmanMode(false);
        {
            double zDistance = 1000.;
            while (zDistance > 0.25) {
                tryGrabAndProcessFrame([&zDistance](OSVR_ChannelCount sensor,
                                                    OSVR_Pose3 const &pose) {
                    if (sensor == 0) {
                        zDistance = osvrVec3GetZ(&pose.translation);
                    }
                });
            }
        }
    }

    template <typename F> bool tryGrabAndProcessFrame(F &&functor) {
        auto grabResult = grabAndRetrieve();
        if (!grabResult) {
            return false;
        }

        m_vbtracker.processImage(m_frame, m_imageGray, m_timestamp,
                                 std::forward<F>(functor));
        m_frameStride = (m_frameStride + 1) % 15;
        return true;
    }

    /// returns true if we have a frame.
    bool grabAndRetrieve() {
        if (!m_src->grab()) {
            err << "Failed to grab!" << endl;
            return false;
        }
        m_timestamp = osvr::util::time::getNow();
        m_src->retrieve(m_frame, m_imageGray);
        return true;
    }

    bool timeToUpdateDisplay() const { return m_frameStride == 0; }

  private:
    ImageSourcePtr m_src;
    osvr::vbtracker::ConfigParams m_params;
    osvr::vbtracker::VideoBasedTracker m_vbtracker;
    osvr::util::time::TimeValue m_timestamp;
    cv::Mat m_frame;
    cv::Mat m_imageGray;
    std::size_t m_frameStride = 0;
};

int main(int argc, char *argv[]) {
    ConfigParams params;

    /// First step: get the config.
    {
        std::string configName(osvr::server::getDefaultConfigFilename());
        if (argc > 1) {
            configName = argv[1];
        } else {
            out << "Using default config file - pass a filename on the command "
                   "line to use a different one."
                << endl;
        }

        Json::Value root;
        {
            out << "Using config file '" << configName << "'" << endl;
            std::ifstream configFile(configName);
            if (!configFile.good()) {
                err << "Could not open config file!" << endl;
                err << "Searched in the current directory; file may be "
                       "misspelled, missing, or in a different directory."
                    << endl;
                return withAnError();
            }
            Json::Reader reader;
            if (!reader.parse(configFile, root)) {
                err << "Could not parse config file as JSON!" << endl;
                return withAnError();
            }
        }
        auto trackerParams = findVideoTrackerParams(root["drivers"]);
        if (trackerParams.isNull()) {
            out << "Warning: No video tracker params found?" << endl;
        }

        // Actually parse those params from JSON to the struct, just like the
        // plugin would.
        params = parseConfigParams(trackerParams);
    }

    /// Second step: Adjust the config slightly.
    params.debug = true;
    params.extraVerbose = false;         // don't need those messages
    params.streamBeaconDebugInfo = true; // want the data being recorded there.

    /// Third step: Open cam and construct a tracker.
    auto src = openHDKCameraDirectShow();
    if (!src || !src->ok()) {
        err << "Couldn't find or access the IR camera!" << endl;
        return withAnError();
    }

    if (params.numThreads > 0) {
        // Set the number of threads for OpenCV to use.
        cv::setNumThreads(params.numThreads);
    }

    TrackerCalibrationApp trackerApp{std::move(src), params};

    /// Fourth step: Add the sensors to the tracker.
    {
        auto backPanelFixedBeacon = [](int) { return true; };
        auto frontPanelFixedBeacon = [](int id) {
            return (id == 16) || (id == 17) || (id == 19) || (id == 20);
        };

        auto camParams = getHDKCameraParameters();
        if (params.includeRearPanel) {
            // distance between front and back panel target origins, in mm.
            auto distanceBetweenPanels = params.headCircumference / M_PI * 10. +
                                         params.headToFrontBeaconOriginDistance;
            Point3Vector locations = OsvrHdkLedLocations_SENSOR0;
            Vec3Vector directions = OsvrHdkLedDirections_SENSOR0;
            std::vector<double> variances = OsvrHdkLedVariances_SENSOR0;

            // For the back panel beacons: have to rotate 180 degrees
            // about Y, which is the same as flipping sign on X and Z
            // then we must translate along Z by head diameter +
            // distance from head to front beacon origins
            for (auto &pt : OsvrHdkLedLocations_SENSOR1) {
                locations.emplace_back(-pt.x, pt.y,
                                       -pt.z - distanceBetweenPanels);
                variances.push_back(params.backPanelMeasurementError);
            }
            // Similarly, rotate the directions.
            for (auto &vec : OsvrHdkLedDirections_SENSOR1) {
                directions.emplace_back(-vec[0], vec[1], -vec[2]);
            }
            trackerApp.vbtracker().addSensor(
                createHDKUnifiedLedIdentifier(), camParams, locations,
                directions, variances, frontPanelFixedBeacon, 4, 0);
        } else {
            err << "WARNING: only calibrating the first sensor is currently "
                   "supported!"
                << endl;
            // OK, so if we don't have to include the rear panel as part of the
            // single sensor, that's easy.
            trackerApp.vbtracker().addSensor(
                createHDKLedIdentifier(0), camParams,
                OsvrHdkLedLocations_SENSOR0, OsvrHdkLedDirections_SENSOR0,
                OsvrHdkLedVariances_SENSOR0, frontPanelFixedBeacon, 6, 0);
            trackerApp.vbtracker().addSensor(
                createHDKLedIdentifier(1), camParams,
                OsvrHdkLedLocations_SENSOR1, OsvrHdkLedDirections_SENSOR1,
                backPanelFixedBeacon, 4, 0);
        }
    }

    return 0;
}