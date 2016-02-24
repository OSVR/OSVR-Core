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
#include "SetupSensors.h"

#include "CVTwoStepProgressBar.h"

#include <osvr/Common/JSONHelpers.h>
#include <osvr/Common/JSONEigen.h>

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <json/reader.h>
#include <json/value.h>

// Standard includes
#include <memory>
#include <iostream>
#include <unordered_set>

using std::endl;
using namespace osvr::vbtracker;

static osvr::server::detail::StreamPrefixer
    out("[OSVR Video Tracker Calibration] ", std::cout);
static osvr::server::detail::StreamPrefixer
    err("[OSVR Video Tracker Calibration] ", std::cerr);

/// @brief OpenCV's simple highgui module refers to windows by their name, so we
/// make this global for a simpler demo.
static const std::string windowNameAndInstructions(
    "OSVR Video Tracker Pre-Calibration | q or esc to quit without saving");

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
        : m_src(std::move(src)), m_params(params), m_vbtracker(params),
          m_distanceBetweenPanels(computeDistanceBetweenPanels(params)) {
        m_firstNotch = m_params.initialBeaconError * 0.9;
        m_secondNotch = m_params.initialBeaconError * 0.8;
        cv::namedWindow(windowNameAndInstructions);
    }

    osvr::vbtracker::VideoBasedTracker &vbtracker() { return m_vbtracker; }

    void run() {
        /// Turn off the Kalman filter until the user has brought the HMD close.
        vbtracker().getFirstEstimator().permitKalmanMode(false);

        /// Get the HMD close.
        {
            out << "Bring the HMD into view and fairly close to the camera."
                << endl;
            double zDistance = 1000.;
            while (zDistance > 0.3 && !m_quit) {
                tryGrabAndProcessFrame([&zDistance](OSVR_ChannelCount sensor,
                                                    OSVR_Pose3 const &pose) {
                    if (sensor == 0) {
                        zDistance = osvrVec3GetZ(&pose.translation);
                    }
                });

                if (timeToUpdateDisplay()) {
                    m_frame.copyTo(m_display);
                    cv::putText(
                        m_display,
                        "Bring the HMD into view and close to the camera.",
                        cv::Point(30, 30), cv::FONT_HERSHEY_SIMPLEX, 0.4,
                        cv::Scalar(255, 0, 0));
                    updateDisplay();
                }
            }
        }
        if (m_quit) {
            /// early out.
            return;
        }

        /// Now, we can turn on the Kalman filter.
        vbtracker().getFirstEstimator().permitKalmanMode(true);

        out << "OK - you'll now want to move the HMD slowly around the view of "
               "the camera, and rotate it so that all sides can be seen."
            << endl;
        out << "Press 's' to save your calibration when you're done." << endl;
        m_frame.copyTo(m_display);
        updateDisplay();

        // Only show the front-panel beacons - we do not want to encourage
        // attempts to calibrate the back panel, since it's not rigidly attached
        // to the front panel.
        static const int MAX_BEACONS_TO_SHOW = 34;

        while (!m_quit) {
            bool gotDebugData = false;
            tryGrabAndProcessFrame(
                [&](OSVR_ChannelCount sensor, OSVR_Pose3 const &pose) {
                    if (sensor == 0) {
                        gotDebugData = true;
                    }
                });

            if (timeToUpdateDisplay() && gotDebugData) {
                m_frame.copyTo(m_display);

                {
                    /// Draw circles for each active beacon showing the size of
                    /// the residuals
                    auto &debugData =
                        vbtracker().getFirstEstimator().getBeaconDebugData();
                    auto nBeacons = debugData.size();
                    auto nBeaconsActive = size_t{0};
                    auto nBeaconsIdentified = size_t{0};
                    for (decltype(nBeacons) i = 0; i < nBeacons; ++i) {
                        if (debugData[i].measurement.x != 0) {
                            nBeaconsIdentified++;
                        }
                        if (debugData[i].variance == 0) {
                            continue;
                        }
                        nBeaconsActive++;
#if 0
                        cv::line(m_display, debugData[i].measurement, debugData[i].measurement + debugData[i].residual, cv::Scalar(0, 255, 0), 2);
#else
                        cv::circle(m_display, debugData[i].measurement,
                                   cv::norm(debugData[i].residual),
                                   cv::Scalar(20, 20, 20), 2);
#endif
                    }

                    cv::putText(m_display,
                                std::to_string(nBeaconsActive) +
                                    " beacons active of " +
                                    std::to_string(nBeaconsIdentified) +
                                    " identified this frame",
                                cv::Point(30, 30), cv::FONT_HERSHEY_SIMPLEX,
                                0.45, cv::Scalar(255, 100, 100));

                    cv::putText(m_display, "Green labels indicate beacons with "
                                           "enough calibration data. S to save "
                                           "and quit.",
                                cv::Point(30, 50), cv::FONT_HERSHEY_SIMPLEX,
                                0.45, cv::Scalar(255, 100, 100));
                }
                auto numGreen = std::size_t{0};
                auto numYellow = std::size_t{0};
                auto numUnimproved = std::size_t{0};

                {
                    /// Reproject (almost) all beacons
                    std::vector<cv::Point2f> reprojections;
                    vbtracker().getFirstEstimator().ProjectBeaconsToImage(
                        reprojections);
                    const auto nBeacons = reprojections.size();
                    m_nBeacons = nBeacons;
                    const auto beaconsToDisplay = std::min(
                        MAX_BEACONS_TO_SHOW, static_cast<int>(nBeacons));
                    for (std::size_t i = 0; i < beaconsToDisplay; ++i) {
                        Eigen::Vector3d autocalibVariance =
                            vbtracker()
                                .getFirstEstimator()
                                .getBeaconAutocalibVariance(i);
                        cv::Scalar color{0, 0, 255};
                        if ((autocalibVariance.array() <
                             Eigen::Array3d::Constant(m_secondNotch))
                                .all()) {
                            /// Green - good!
                            color = cv::Scalar{0, 255, 0};
                            numGreen++;
                        } else if ((autocalibVariance.array() <
                                    Eigen::Array3d::Constant(m_firstNotch))
                                       .all()) {
                            /// Yellow - better than where you started
                            color = cv::Scalar{0, 255, 255};
                            numYellow++;
                        } else {
                            numUnimproved++;
                        }

                        cv::putText(m_display, std::to_string(i + 1),
                                    reprojections[i] + cv::Point2f(1, 1),
                                    cv::FONT_HERSHEY_SIMPLEX, 0.45, color);
                    }
                }

                {
                    /// Draw progress bar along bottom of window.
                    static const auto PROGRESS_HEIGHT = 5;
                    drawTwoStepProgressBar(
                        m_display,
                        cv::Point(0, m_display.rows - PROGRESS_HEIGHT),
                        cv::Size(m_display.cols, PROGRESS_HEIGHT), numGreen,
                        numYellow, numUnimproved);
                }

                auto key = updateDisplay();
                if ('s' == key || 'S' == key) {
                    m_save = true;
                    m_quit = true;
                }
            }
        }

        int beaconsToOutput =
            std::min(MAX_BEACONS_TO_SHOW, static_cast<int>(m_nBeacons));
        for (int i = 0; i < beaconsToOutput; ++i) {
            std::cout << "Beacon " << i + 1 << " autocalib variance ratio: "
                      << vbtracker()
                                 .getFirstEstimator()
                                 .getBeaconAutocalibVariance(i)
                                 .transpose() /
                             m_params.initialBeaconError
                      << "\n";
        }

        std::cout << endl;
        if (m_save) {
            Json::Value calib(Json::arrayValue);
            out << "Saving your calibration data..." << endl;
            auto &estimator = vbtracker().getFirstEstimator();
            auto numBeaconsFromAutocalib =
                std::min(m_nBeacons, getNumHDKFrontPanelBeacons());

            for (std::size_t i = 0; i < numBeaconsFromAutocalib; ++i) {
                calib.append(osvr::common::toJson(
                    estimator.getBeaconAutocalibPosition(i)));
            }
            if (m_nBeacons > numBeaconsFromAutocalib) {
                // This means they wanted rear panel beacons too. We will write
                // un-calibrated beacon locations for those, so they don't get
                // "less-neutral" starting positions.
                Point3Vector locations;
                addRearPanelBeaconLocations(m_distanceBetweenPanels, locations);
                for (auto const &beacon : locations) {
                    Json::Value val(Json::arrayValue);
                    val.append(beacon.x);
                    val.append(beacon.y);
                    val.append(beacon.z);
                    calib.append(val);
                }
            }
            std::cout << "\n" << osvr::common::jsonToCompactString(calib)
                      << "\n" << endl;

            {
                std::ofstream outfile(m_params.calibrationFile);
                outfile << osvr::common::jsonToStyledString(calib);
                outfile.close();
            }
            closeWindow();
            out << "Done! Press enter to exit." << endl;
            std::cin.ignore();
        }
    }

    /// returns true if we processed a frame.
    template <typename F> bool tryGrabAndProcessFrame(F &&functor) {
        if (!m_src->grab()) {
            err << "Failed to grab!" << endl;
            return false;
        }
        m_timestamp = osvr::util::time::getNow();
        m_src->retrieve(m_frame, m_imageGray);

        m_vbtracker.processImage(m_frame, m_imageGray, m_timestamp,
                                 std::forward<F>(functor));
        m_frameStride = (m_frameStride + 1) % 11;
        return true;
    }

    /// Is it time to update the display window?
    bool timeToUpdateDisplay() const { return m_frameStride == 0; }

    /// Set the contents of m_display before calling this. Returns the character
    /// pressed, if any.
    char updateDisplay() {
        cv::imshow(windowNameAndInstructions, m_display);
        auto key = static_cast<char>(cv::waitKey(1));
        if ('q' == key || 'Q' == key || 27 /* esc */ == key) {
            m_quit = true;
        }
        return key;
    }

    void closeWindow() { cv::destroyWindow(windowNameAndInstructions); }

  private:
    ImageSourcePtr m_src;
    osvr::vbtracker::ConfigParams m_params;
    const double m_distanceBetweenPanels;
    double m_firstNotch;
    double m_secondNotch;
    osvr::vbtracker::VideoBasedTracker m_vbtracker;
    osvr::util::time::TimeValue m_timestamp;
    cv::Mat m_frame;
    cv::Mat m_imageGray;
    // This is the one the steps of the app should mess with.
    cv::Mat m_display;
    std::size_t m_frameStride = 0;
    std::size_t m_nBeacons = 0;
    bool m_quit = false;
    bool m_save = false;
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

    if (params.calibrationFile.empty()) {
        err << "calibrationFile not specified in configuration file! no clue "
               "where to write to!"
            << endl;
        return withAnError();
    }

    /// Second step: Adjust the config slightly.
    params.debug = false;
    params.extraVerbose = false;         // don't need those messages
    params.streamBeaconDebugInfo = true; // want the data being recorded there.

    /// Third step: Open cam and construct a tracker.
    auto src = openHDKCameraDirectShow();
    if (!src || !src->ok()) {
        err << "Couldn't find or access the IR camera!" << endl;
        return withAnError();
    }

    // Set the number of threads for OpenCV to use.
    cv::setNumThreads(1);

    TrackerCalibrationApp trackerApp{std::move(src), params};

    /// Fourth step: Add the sensors to the tracker.
    {
        auto camParams = getHDKCameraParameters();
        if (params.includeRearPanel) {

            /// Setup sensor just as in tracker app, but don't try to load
            /// calibration.
            setupSensorsIncludeRearPanel(trackerApp.vbtracker(), params, false);
        } else {

            err << "WARNING: only calibrating the first sensor is currently "
                   "supported!"
                << endl;
            setupSensorsWithoutRearPanel(trackerApp.vbtracker(), params, false);
        }
    }

    trackerApp.run();

    return 0;
}
