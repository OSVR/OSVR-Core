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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "VideoBasedTracker.h"
#include "cvToEigen.h"
#include "CameraDistortionModel.h"
#include "UndistortMeasurements.h"
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/CSV.h>

// Library/third-party includes
#include <opencv2/core/version.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <fstream>
#include <algorithm>
#include <iostream>

namespace osvr {
namespace vbtracker {

    VideoBasedTracker::VideoBasedTracker(ConfigParams const &params)
        : m_params(params), m_blobExtractor(params.blobParams) {}

    // This version requires YOU to add your beacons! You!
    void VideoBasedTracker::addSensor(
        LedIdentifierPtr &&identifier, CameraParameters const &camParams,
        std::function<void(BeaconBasedPoseEstimator &)> const &beaconAdder,
        size_t requiredInliers, size_t permittedOutliers) {
        m_camParams = camParams;
        m_identifiers.emplace_back(std::move(identifier));
        m_estimators.emplace_back(new BeaconBasedPoseEstimator(
            camParams.createUndistortedVariant(), requiredInliers,
            permittedOutliers, m_params));
        m_led_groups.emplace_back();
        beaconAdder(*m_estimators.back());
        m_assertInvariants();
    }

    void VideoBasedTracker::addSensor(
        LedIdentifierPtr &&identifier, CameraParameters const &camParams,
        Point3Vector const &locations, Vec3Vector const &emissionDirection,
        std::vector<double> const &variance,
        BeaconIDPredicate const &autocalibrationFixedPredicate,
        size_t requiredInliers, size_t permittedOutliers,
        double beaconAutocalibErrorScale) {
        addSensor(std::move(identifier), camParams,
                  [&](BeaconBasedPoseEstimator &estimator) {
                      estimator.SetBeacons(locations, emissionDirection,
                                           variance,
                                           autocalibrationFixedPredicate,
                                           beaconAutocalibErrorScale);
                  },
                  requiredInliers, permittedOutliers);
    }

    void VideoBasedTracker::dumpKeypointDebugData(
        std::vector<cv::KeyPoint> const &keypoints) {
        {
            std::cout << "Dumping blob detection debug data, capture frame "
                      << m_debugFrame << std::endl;
            cv::imwrite("debug_rawimage" + std::to_string(m_debugFrame) +
                            ".png",
                        m_frame);
            cv::imwrite("debug_blobframe" + std::to_string(m_debugFrame) +
                            ".png",
                        m_imageWithBlobs);
            cv::imwrite("debug_thresholded" + std::to_string(m_debugFrame) +
                            ".png",
                        m_thresholdImage);
        }

        {
            auto filename = std::string{"debug_data" +
                                        std::to_string(m_debugFrame) + ".txt"};
            std::ofstream datafile{filename.c_str()};
            datafile << "MinThreshold: " << m_sbdParams.minThreshold
                     << std::endl;
            datafile << "MaxThreshold: " << m_sbdParams.maxThreshold
                     << std::endl;
            datafile << "ThresholdStep: " << m_sbdParams.thresholdStep
                     << std::endl;
            datafile << "Thresholds:" << std::endl;
            for (double thresh = m_sbdParams.minThreshold;
                 thresh < m_sbdParams.maxThreshold;
                 thresh += m_sbdParams.thresholdStep) {
                datafile << thresh << std::endl;
            }
        }
        {
            using namespace osvr::util;
            CSV kpcsv;
            for (auto &keypoint : keypoints) {
                kpcsv.row() << cell("x", keypoint.pt.x)
                            << cell("y", keypoint.pt.y)
                            << cell("size", keypoint.size);
            }
            auto filename = std::string{"debug_blobdetect" +
                                        std::to_string(m_debugFrame) + ".csv"};
            std::ofstream csvfile{filename.c_str()};
            kpcsv.output(csvfile);
            csvfile.close();
        }
        std::cout << "Data dump complete." << std::endl;
        m_debugFrame++;
    }

    bool VideoBasedTracker::processImage(cv::Mat frame, cv::Mat grayImage,
                                         OSVR_TimeValue const &tv,
                                         PoseHandler handler) {
        m_assertInvariants();
        bool done = false;
        m_frame = frame;
        m_imageGray = grayImage;
        auto foundLeds = m_blobExtractor.extractBlobs(grayImage);

        /// Perform the undistortion of keypoints
        auto undistortedLeds = undistortLeds(foundLeds, m_camParams);

        // We allow multiple sets of LEDs, each corresponding to a different
        // sensor, to be located in the same image.  We construct a new set
        // of LEDs for each and try to find them.  It is assumed that they all
        // have unique ID patterns across all sensors.
        for (size_t sensor = 0; sensor < m_identifiers.size(); sensor++) {

            osvrPose3SetIdentity(&m_pose);
            auto ledsMeasurements = undistortedLeds;

            // Locate the closest blob from this frame to each LED found
            // in the previous frame.  If it is close enough to the nearest
            // neighbor from last time, we assume that it is the same LED and
            // update it.  If not, we delete the LED from the list.  Once we
            // have matched a blob to an LED, we remove it from the list.  If
            // there are any blobs leftover, we create new LEDs from them.
            // @todo: Include motion estimate based on Kalman filter along with
            // model of the projection once we have one built.  Note that this
            // will require handling the lens distortion appropriately.
            {
                auto &myLeds = m_led_groups[sensor];
                auto led = begin(myLeds);
                auto e = end(myLeds);
                while (led != end(myLeds)) {
                    led->resetUsed();
                    auto threshold = m_params.blobMoveThreshold *
                                     led->getMeasurement().diameter;
                    auto nearest = led->nearest(ledsMeasurements, threshold);
                    if (nearest == end(ledsMeasurements)) {
                        // We have no blob corresponding to this LED, so we need
                        // to delete this LED.
                        led = myLeds.erase(led);
                    } else {
                        // Update the values in this LED and then go on to the
                        // next one. Remove this blob from the list of
                        // potential matches.
                        led->addMeasurement(*nearest,
                                            m_params.blobsKeepIdentity);
                        ledsMeasurements.erase(nearest);
                        ++led;
                    }
                }
                // If we have any blobs that have not been associated with an
                // LED, then we add a new LED for each of them.
                // std::cout << "Had " << Leds.size() << " LEDs, " <<
                // keyPoints.size() << " new ones available" << std::endl;
                for (auto &remainingLed : ledsMeasurements) {
                    myLeds.emplace_back(m_identifiers[sensor].get(),
                                        remainingLed);
                }
            }
            //==================================================================
            // Compute the pose of the HMD w.r.t. the camera frame of
            // reference.
            bool gotPose = false;
            if (m_estimators[sensor]) {

                // Get an estimated pose, if we have enough data.
                OSVR_PoseState pose;
                if (m_estimators[sensor]->EstimatePoseFromLeds(
                        m_led_groups[sensor], tv, pose)) {
                    m_pose = pose;
                    handler(static_cast<unsigned>(sensor), pose);
                    gotPose = true;
                }
            }
            if (m_params.debug) {
                // Don't display the debugging info every frame, or we can't go
                // fast enough.
                static const auto RED = cv::Vec3b(0, 0, 255);
                static const auto YELLOW = cv::Vec3b(0, 255, 255);
                static const auto GREEN = cv::Vec3b(0, 255, 0);
                static int count = 0;
                if (++count == 11) {
                    // Fake the thresholded image to give an idea of what the
                    // blob detector is doing.
                    m_thresholdImage = m_blobExtractor.getDebugThresholdImage();

                    // Draw detected blobs as blue circles.
                    m_imageWithBlobs = m_blobExtractor.getDebugBlobImage();

                    // Draw the unidentified (flying?) blobs (UFBs?) on the
                    // status image
                    m_frame.copyTo(m_statusImage);
                    for (auto const &led : m_led_groups[sensor]) {
                        auto loc = led.getLocation();
                        if (!led.identified()) {
                            drawLedCircleOnStatusImage(led, false, RED);
                        } else if (!gotPose) {
                            // If identified, but we don't have a pose, draw
                            // them as yellow outlines.
                            drawLedCircleOnStatusImage(led, false, YELLOW);

                            drawRecognizedLedIdOnStatusImage(led);
                        }
                    }

                    // Label the keypoints with their IDs in the "blob" image
                    for (auto &led : m_led_groups[sensor]) {
                        // Print 1-based LED ID for actual LEDs
                        auto label = std::to_string(led.getOneBasedID());
                        cv::Point where = led.getLocation();
                        where.x += 1;
                        where.y += 1;
                        cv::putText(m_imageWithBlobs, label, where,
                                    cv::FONT_HERSHEY_SIMPLEX, 0.5,
                                    cv::Scalar(0, 0, 255));
                    }

                    // If we have a transform, reproject all of the points from
                    // the model space (the LED locations) back into the image
                    // and display them on the blob image in green.
                    if (gotPose) {
                        std::vector<cv::Point2f> imagePoints;
                        m_estimators[sensor]->ProjectBeaconsToImage(
                            imagePoints);
                        const size_t n = imagePoints.size();
                        for (size_t i = 0; i < n; ++i) {
                            // Print 1-based LED IDs
                            auto label = std::to_string(i + 1);
                            auto where = imagePoints[i];
                            where.x += 1;
                            where.y += 1;
                            cv::putText(m_imageWithBlobs, label, where,
                                        cv::FONT_HERSHEY_SIMPLEX, 0.5,
                                        cv::Scalar(GREEN));
                        }

                        // Now, also set up the status image, as long as we have
                        // a reprojection.
                        for (auto const &led : m_led_groups[sensor]) {
                            if (led.identified()) {
                                // Color-code identified beacons based on
                                // whether or not we used their data.
                                auto color =
                                    led.wasUsedLastFrame() ? GREEN : YELLOW;

                                // Draw a filled circle at the keypoint.
                                drawLedCircleOnStatusImage(led, true, color);
                                if (led.getID() < n) {
                                    auto reprojection =
                                        imagePoints[led.getID()];

                                    drawRecognizedLedIdOnStatusImage(led);
                                    cv::putText(
                                        m_statusImage,
                                        std::to_string(led.getOneBasedID()),
                                        reprojection, cv::FONT_HERSHEY_SIMPLEX,
                                        0.25, cv::Scalar(0, 0, 0));
                                }
                            }
                        }
                        // end of status image setup
                    }

                    if (!m_debugHelpDisplayed) {
                        std::cout
                            << "\nVideo-based tracking debug windows help:\n";
                        std::cout
                            << "  - press 's' to show the detected blobs and "
                               "the status of recognized beacons (default)\n"
                            << "  - press 'b' to show the labeled blobs and "
                               "the reprojected beacons\n"
                            << "  - press 'i' to show the raw input image\n"
                            << "  - press 't' to show the blob-detecting "
                               "threshold image\n"
                            << "  - press 'p' to dump the current "
                               "auto-calibrated beacon positions to a CSV "
                               "file\n"
                            << "  - press 'q' to quit the debug windows "
                               "(tracker will continue operation)\n"
                            << std::endl;
                        m_debugHelpDisplayed = true;
                    }
                    // Pick which image to show and show it.
                    if (m_frame.data) {
                        std::ostringstream windowName;
                        windowName << "Sensor" << sensor;
                        cv::imshow(windowName.str(), *m_shownImage);
                        int key = cv::waitKey(1);
                        switch (key) {
                        case 's':
                            // Show the concise "status" image (default)
                            m_shownImage = &m_statusImage;
                            break;
                        case 'b':
                            // Show the blob/keypoints image
                            m_shownImage = &m_imageWithBlobs;
                            break;
                        case 'i':
                            // Show the input image.
                            m_shownImage = &m_frame;
                            break;

                        case 't':
                            // Show the thresholded image
                            m_shownImage = &m_thresholdImage;
                            break;

#if 0
                        case 'd':
                            dumpKeypointDebugData(foundKeyPoints);
                            break;
#endif
                        case 'p':
                            // Dump the beacon positions to file.
                            {
                                std::ofstream beaconfile("beacons.csv");
                                for (auto const &estimator : m_estimators) {
                                    beaconfile << "----" << std::endl;
                                    estimator->dumpBeaconLocationsToStream(
                                        beaconfile);
                                }
                                beaconfile.close();
                            }
                            break;

                        case 'q':
                            // Indicate we want to quit.
                            done = true;
                            // Also, if we can't "quit", at least hide the debug
                            // window.
                            m_params.debug = false;
                            cv::destroyAllWindows();
                            break;
                        }
                    }
                    count = 0;
                }
            }
        }

        m_assertInvariants();
        return done;
    }

    void VideoBasedTracker::drawLedCircleOnStatusImage(Led const &led,
                                                       bool filled,
                                                       cv::Vec3b color) {
        cv::circle(m_statusImage, led.getLocation(),
                   led.getMeasurement().diameter / 2., cv::Scalar(color),
                   filled ? -1 : 1);
    }

    void VideoBasedTracker::drawRecognizedLedIdOnStatusImage(Led const &led) {

        auto label = std::to_string(led.getOneBasedID());
        cv::putText(m_statusImage, label, led.getLocation(),
                    cv::FONT_HERSHEY_SIMPLEX, 0.25, cv::Scalar(127, 127, 127));
    }

} // namespace vbtracker
} // namespace osvr
