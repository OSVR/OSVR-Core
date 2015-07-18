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

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    void VideoBasedTracker::addOculusSensor() {
        /// @todo this clearly violates what I expected was the invariant - not
        /// sure if it's because of incomplete Oculus information, or due to a
        /// misunderstanding of how this all works.
        m_led_groups.emplace_back();
    }
    void VideoBasedTracker::addSensor(LedIdentifier *identifier,
                                      DoubleVecVec const &m,
                                      std::vector<double> const &d,
                                      DoubleVecVec const &locations) {
        addSensor(LedIdentifierPtr(identifier), m, d, locations);
    }

    void VideoBasedTracker::addSensor(LedIdentifierPtr &&identifier,
                                      DoubleVecVec const &m,
                                      std::vector<double> const &d,
                                      DoubleVecVec const &locations) {
        m_identifiers.emplace_back(std::move(identifier));
        m_estimators.emplace_back(
            new BeaconBasedPoseEstimator(m, d, locations));
        m_led_groups.emplace_back();
        m_assertInvariants();
    }
    bool VideoBasedTracker::processImage(cv::Mat frame, cv::Mat grayImage,
                                         PoseHandler handler) {
        m_assertInvariants();
        bool done = false;
        m_frame = frame;
        m_imageGray = grayImage;

        //================================================================
        // Tracking the points

        // Threshold the image based on the brightness value that is between
        // the darkest and brightest pixel in the image.
        double minVal, maxVal;
        cv::minMaxLoc(m_imageGray, &minVal, &maxVal);
        double thresholdValue = 35;
        cv::threshold(m_imageGray, m_thresholdImage, thresholdValue, 255,
                      CV_THRESH_BINARY);

        // Construct a blob detector and find the blobs in the image.
        // @todo Make it so we don't have to have a blown-out image to track.
        // If the light is dimmer in the simulated image, so the brightest
        // pixel is not 255 saturated across the blobs, we don't find any
        // blobs.
        // @todo: Determine the maximum size of a trackable blob by seeing
        // when we're so close that we can't view at least four in the
        // camera.
        cv::SimpleBlobDetector::Params params;
        params.filterByColor = true; // Look for bright blobs
        params.blobColor = static_cast<uchar>(255);
        params.filterByInertia = true; // Look for non-elongated blobs
        params.minInertiaRatio = 0.5;
        params.maxInertiaRatio = 1.0;
        params.minThreshold = static_cast<float>(thresholdValue);
        params.maxThreshold = static_cast<float>(thresholdValue + (maxVal - thresholdValue) * 0.3);
        params.thresholdStep = (params.maxThreshold - params.minThreshold) / 10;
        cv::SimpleBlobDetector detector(params);
        /// @todo this variable is a candidate for hoisting to member
        std::vector<cv::KeyPoint> foundKeyPoints;
        detector.detect(m_imageGray, foundKeyPoints);

        // TODO: Consider computing the center of mass of a dilated bounding
        // rectangle around each keypoint to produce a more precise subpixel
        // localization of each LED.  The moments() function may be helpful
        // with this.

        // TODO: Estimate the summed brightness of each blob so that we can
        // detect when they are getting brighter and dimmer.  Pass this as
        // the brightness parameter to the Led class when adding a new one
        // or augmenting with a new frame.

        // We allow multiple sets of LEDs, each corresponding to a different
        // sensor, to be located in the same image.  We construct a new set
        // of LEDs for each and try to find them.  It is assumed that they all
        // have unique ID patterns across all sensors.
        for (size_t sensor = 0; sensor < m_identifiers.size(); sensor++) {
            osvrPose3SetIdentity(&m_pose);
            std::vector<cv::KeyPoint> keyPoints = foundKeyPoints;

            // Locate the closest blob from this frame to each LED found
            // in the previous frame.  If it is close enough to the nearest
            // neighbor from last time, we assume that it is the same LED and
            // update it.  If not, we delete the LED from the list.  Once we
            // have matched a blob to an LED, we remove it from the list.  If
            // there are any blobs leftover, we create new LEDs from them.
            // TODO: Include motion estimate based on Kalman filter along with
            // model of the projection once we have one built.  Note that this
            // will require handling the lens distortion appropriately.
            auto led = begin(m_led_groups[sensor]);
            auto e = end(m_led_groups[sensor]);
            while (led != e) {
                double TODO_BLOB_MOVE_THRESHOLD = 10;
                auto nearest =
                    led->nearest(keyPoints, TODO_BLOB_MOVE_THRESHOLD);
                if (nearest == keyPoints.end()) {
                    // We have no blob corresponding to this LED, so we need
                    // to delete this LED.
                    led = m_led_groups[sensor].erase(led);
                } else {
                    // Update the values in this LED and then go on to the
                    // next one.  Remove this blob from the list of potential
                    // matches.
                    led->addMeasurement(nearest->pt, nearest->size);
                    keyPoints.erase(nearest);
                    led++;
                }
            }
            // If we have any blobs that have not been associated with an
            // LED, then we add a new LED for each of them.
            // std::cout << "Had " << Leds.size() << " LEDs, " <<
            // keyPoints.size() << " new ones available" << std::endl;
            while (keyPoints.size() > 0) {
                osvr::vbtracker::Led newLed(m_identifiers[sensor].get(),
                                            keyPoints.begin()->pt,
                                            keyPoints.begin()->size);
                m_led_groups[sensor].push_back(newLed);
                keyPoints.erase(keyPoints.begin());
            }

            //==================================================================
            // Compute the pose of the HMD w.r.t. the camera frame of reference.
            // TODO: Keep track of whether we already have a good pose and, if
            // so, have the algorithm initialize using it so we do less work on
            // average.
            bool gotPose = false;
            if (m_estimators[sensor]) {
                OSVR_PoseState pose;
                if (m_estimators[sensor]->EstimatePoseFromLeds(
                        m_led_groups[sensor], pose)) {
                    m_pose = pose;
                    handler(static_cast<unsigned>(sensor), pose);
                    gotPose = true;
                }
            }

#ifdef VBHMD_DEBUG
            // Don't display the debugging info every frame, or we can't go fast enough.
            static int count = 0;
            if (++count == 11) {
                // Draw detected blobs as red circles.
                cv::drawKeypoints(m_frame, keyPoints, m_imageWithBlobs,
                    cv::Scalar(0, 0, 255),
                    cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                // Label the keypoints with their IDs.
                for (led = m_led_groups[sensor].begin();
                    led != m_led_groups[sensor].end(); led++) {
                    // Print 1-based LED ID for actual LEDs
                    auto label = std::to_string(led->getOneBasedID());
                    cv::Point where = led->getLocation();
                    where.x += 1;
                    where.y += 1;
                    cv::putText(m_imageWithBlobs, label, where,
                        cv::FONT_HERSHEY_SIMPLEX, 0.5,
                        cv::Scalar(0, 0, 255));
                }

                // If we have a transform, reproject all of the points from the
                // model space (the LED locations) back into the image and
                // display them on the blob image in green.
                if (gotPose) {
                    std::vector<cv::Point2f> imagePoints;
                    m_estimators[sensor]->ProjectBeaconsToImage(imagePoints);
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
                    cv::imshow(windowName.str().c_str(), *m_shownImage);
                    int key = cv::waitKey(1);
                    switch (key) {
                    case 'i':
                        // Show the input image.
                        m_shownImage = &m_frame;
                        break;

                    case 't':
                        // Show the thresholded image.
                        m_shownImage = &m_thresholdImage;
                        break;

                    case 'b':
                        // Show the blob image.
                        m_shownImage = &m_imageWithBlobs;
                        break;
                    case 'q':
                        // Indicate we want to quit.
                        done = true;
                        break;
                    }
                }

                // Report the pose, if we got one
                if (gotPose) {
                    std::cout << "Pos (sensor " << sensor
                        << "): " << m_pose.translation.data[0] << ", "
                        << m_pose.translation.data[1] << ", "
                        << m_pose.translation.data[2] << std::endl;
                }
                count = 0;
            }
#endif
        }

        m_assertInvariants();
        return done;
    }
} // namespace vbtracker
} // namespace osvr
