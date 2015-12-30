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

#define OSVR_USE_SIMPLEBLOB
#undef OSVR_USE_CANNY_EDGEDETECT

namespace osvr {
namespace vbtracker {

    VideoBasedTracker::VideoBasedTracker(ConfigParams const &params)
        : m_params(params), m_blobExtractor(params) {}

    // This version adds the beacons as a part of the constructor.
    void VideoBasedTracker::addSensor(
        LedIdentifierPtr &&identifier, CameraParameters const &camParams,
        Point3Vector const &locations,
        BeaconIDPredicate const &autocalibrationFixedPredicate,
        size_t requiredInliers, size_t permittedOutliers) {
        m_camParams = camParams;

        m_identifiers.emplace_back(std::move(identifier));
        m_estimators.emplace_back(new BeaconBasedPoseEstimator(
            camParams.createUndistortedVariant(), locations, requiredInliers,
            permittedOutliers, autocalibrationFixedPredicate, m_params));
        m_led_groups.emplace_back();
        m_assertInvariants();
    }

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
        Point3Vector const &locations, double variance,
        BeaconIDPredicate const &autocalibrationFixedPredicate,
        size_t requiredInliers, size_t permittedOutliers) {
        addSensor(std::move(identifier), camParams,
                  [&](BeaconBasedPoseEstimator &estimator) {
                      estimator.SetBeacons(locations, variance,
                                           autocalibrationFixedPredicate);
                  },
                  requiredInliers, permittedOutliers);
    }

    void VideoBasedTracker::addSensor(
        LedIdentifierPtr &&identifier, CameraParameters const &camParams,
        Point3Vector const &locations, std::vector<double> const &variance,
        BeaconIDPredicate const &autocalibrationFixedPredicate,
        size_t requiredInliers, size_t permittedOutliers) {
        addSensor(std::move(identifier), camParams,
                  [&](BeaconBasedPoseEstimator &estimator) {
                      estimator.SetBeacons(locations, variance,
                                           autocalibrationFixedPredicate);
                  },
                  requiredInliers, permittedOutliers);
    }
#if 0
    /// This class is not currently used because it needs some more tuning.
    class KeypointEnhancer {
      public:
        std::vector<cv::KeyPoint>
        enhanceKeypoints(cv::Mat const &grayImage,
                         std::vector<cv::KeyPoint> const &foundKeyPoints) {
            std::vector<cv::KeyPoint> ret;
            cv::Mat greyCopy = grayImage.clone();
            /// Reset the flood fill mask to just have a one-pixel border on the
            /// edges.
            m_floodFillMask =
                cv::Mat::zeros(grayImage.rows + 2, grayImage.cols + 2, CV_8UC1);
            m_floodFillMask.row(0) = 1;
            m_floodFillMask.row(m_floodFillMask.rows - 1) = 1;
            m_floodFillMask.col(0) = 1;
            m_floodFillMask.col(m_floodFillMask.cols - 1) = 1;

            for (auto const &keypoint : foundKeyPoints) {
                ret.push_back(enhanceKeypoint(greyCopy, keypoint));
            }

            return ret;
        }

        cv::Mat getDebugImage() {
            return m_floodFillMask(cv::Rect(1, 1, m_floodFillMask.cols - 2,
                                            m_floodFillMask.rows - 2));
        }

      private:
        cv::KeyPoint enhanceKeypoint(cv::Mat grayImage,
                                     cv::KeyPoint origKeypoint) {
            cv::Rect bounds;
            int loDiff = 5;
            int upDiff = 5;
            // Saving this now before we monkey with m_floodFillMask
            cv::bitwise_not(m_floodFillMask, m_scratchNotMask);

            cv::floodFill(grayImage, m_floodFillMask, origKeypoint.pt, 255,
                          &bounds, loDiff, upDiff,
                          CV_FLOODFILL_MASK_ONLY |
                              (/* connectivity 4 or 8 */ 4) |
                              (/* value to write in to mask */ 255 << 8));
            // Now m_floodFillMask contains the mask with both our point
            // and all other points so far. We need to split them by ANDing with
            // the NOT of the old flood-fill mask we saved earlier.
            cv::bitwise_and(m_scratchNotMask, m_floodFillMask,
                            m_perPointResults);
            // OK, now we have the results for just this point in per-point
            // results

            cv::Mat binarySubImage = m_perPointResults(bounds);
            cv::Moments moms = cv::moments(binarySubImage, true);

            auto area = moms.m00;
            auto x = moms.m10 / moms.m00;
            auto y = moms.m01 / moms.m00;
            auto diameter = 2 * std::sqrt(area / M_PI);
            auto ret = origKeypoint;
            ret.pt = cv::Point2f(x, y) + cv::Point2f(bounds.tl());
            ret.size = diameter;
            return ret;
        }
        cv::Mat m_scratchNotMask;
        cv::Mat m_floodFillMask;
        cv::Mat m_perPointResults;
    };
#endif
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

    /// Perform the undistortion of LED measurements.
    inline std::vector<LedMeasurement>
    undistortLeds(std::vector<LedMeasurement> const &distortedMeasurements,
                  CameraParameters const &camParams) {
        std::vector<LedMeasurement> ret;
        ret.resize(distortedMeasurements.size());
        auto distortionModel = CameraDistortionModel{
            Eigen::Vector2d{camParams.focalLengthX(), camParams.focalLengthY()},
            cvToVector(camParams.principalPoint()),
            Eigen::Vector3d{camParams.k1(), camParams.k2(), camParams.k3()}};
        auto ledUndistort = [&distortionModel](LedMeasurement const &meas) {
            LedMeasurement ret{meas};
            Eigen::Vector2d undistorted = distortionModel.undistortPoint(
                cvToVector(meas.loc).cast<double>());
            ret.loc = vecToPoint(undistorted.cast<float>());
            return ret;
        };
        std::transform(begin(distortedMeasurements), end(distortedMeasurements),
                       begin(ret), ledUndistort);
        return ret;
    }

    bool VideoBasedTracker::processImage(cv::Mat frame, cv::Mat grayImage,
                                         OSVR_TimeValue const &tv,
                                         PoseHandler handler) {
        m_assertInvariants();
        bool done = false;
        m_frame = frame;
        m_imageGray = grayImage;
        auto foundLeds = m_blobExtractor.extractBlobs(grayImage);
#if 0
        /// @todo maybe hoist to avoid allocations?
        KeypointEnhancer enh;
        auto foundKeyPoints =
            enh.enhanceKeypoints(grayImage, initialFoundKeyPoints);
        enh.getDebugImage().copyTo(m_keypointEnhancement);
#endif

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
                    auto nearest = led->nearest(ledsMeasurements,
                                                m_params.blobMoveThreshold);
                    if (nearest == end(ledsMeasurements)) {
                        // We have no blob corresponding to this LED, so we need
                        // to delete this LED.
                        led = myLeds.erase(led);
                    } else {
                        // Update the values in this LED and then go on to the
                        // next one.  Remove this blob from the list of
                        // potential
                        // matches.
                        led->addMeasurement(*nearest);
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
                static int count = 0;
                if (++count == 11) {
                    // Fake the thresholded image to give an idea of what the
                    // blob detector is doing.
                    m_thresholdImage = m_blobExtractor.getDebugThresholdImage();

                    // Draw detected blobs as blue circles.
                    m_imageWithBlobs = m_blobExtractor.getDebugBlobImage();

                    // Label the keypoints with their IDs.
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
                        size_t n = imagePoints.size();
                        for (size_t i = 0; i < n; ++i) {
                            // Print 1-based LED IDs
                            auto label = std::to_string(i + 1);
                            auto where = imagePoints[i];
                            where.x += 1;
                            where.y += 1;
                            cv::putText(m_imageWithBlobs, label, where,
                                        cv::FONT_HERSHEY_SIMPLEX, 0.5,
                                        cv::Scalar(0, 255, 0));
                        }
                    }

                    // Pick which image to show and show it.
                    if (m_frame.data) {
                        std::ostringstream windowName;
                        windowName << "Sensor" << sensor;
                        cv::imshow(windowName.str(), *m_shownImage);
                        int key = cv::waitKey(1);
                        switch (key) {
                        case 'i':
                            // Show the input image.
                            m_shownImage = &m_frame;
                            break;

                        case 't':
                            // Show the thresholded image
                            m_shownImage = &m_thresholdImage;
                            break;

                        case 'b':
                            // Show the blob/keypoints image (default)
                            m_shownImage = &m_imageWithBlobs;
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

#ifdef OSVR_USE_CANNY_EDGEDETECT
    std::vector<cv::KeyPoint>
    VideoBasedTracker::extractKeypoints(cv::Mat const &grayImage) {

        //================================================================
        // Tracking the points

        // Do edge detection: the LEDs are pretty clear edges.

        cv::Mat noiseRemoved;
        cv::threshold(grayImage, noiseRemoved, BASE_NOISE_THRESHOLD, 0,
                      CV_THRESH_TOZERO);
        cv::Canny(noiseRemoved, m_thresholdImage, 180, 220, 5);

        std::vector<cv::KeyPoint> foundKeyPoints;

        // Extract the contours.
        std::vector<std::vector<cv::Point>> contours;
        cv::Mat binaryImage = m_thresholdImage.clone();
        cv::findContours(binaryImage, contours, CV_RETR_EXTERNAL,
                         CV_CHAIN_APPROX_NONE);

        // We don't need the exact m_sbdParams struct, but we use some similar
        // parameters, so why not re-use it.

        for (auto &contour : contours) {
            cv::Mat points(contour);
            cv::Moments moms = cv::moments(points);

            /// @todo any advantage to contourArea over using the moments m00?
            auto area = cv::contourArea(points) /*moms.m00*/;

            /// Filter by area
            if (area < m_sbdParams.minArea || area > m_sbdParams.maxArea) {
                continue;
            }
            /// Filter by circularity
            auto perim = cv::arcLength(points, true);
            auto circularity = 4 * M_PI * area / (perim * perim);
            if (circularity < m_sbdParams.minCircularity ||
                circularity > m_sbdParams.maxCircularity) {
                continue;
            }

            // OK, we think this one is valid. Make a keypoint for it.
            auto x = moms.m10 / moms.m00;
            auto y = moms.m01 / moms.m00;
            foundKeyPoints.push_back(cv::KeyPoint(x, y, area / 4));
        }

        // @todo: Estimate the summed brightness of each blob so that we can
        // detect when they are getting brighter and dimmer.  Pass this as
        // the brightness parameter to the Led class when adding a new one
        // or augmenting with a new frame.
        return foundKeyPoints;
    }
#endif

} // namespace vbtracker
} // namespace osvr
