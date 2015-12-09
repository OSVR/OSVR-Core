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
#include <opencv2/core/version.hpp>

// Standard includes
// - none

#define OSVR_USE_SIMPLEBLOB
#undef OSVR_USE_CANNY_EDGEDETECT

namespace osvr {
namespace vbtracker {
    /// This is the absolute minimum pixel value that will be considered as a
    /// possible signal. Images that contain only values below this will be
    /// totally discarded as containing zero keypoints.
    static const double BASE_NOISE_THRESHOLD = 75.;

    VideoBasedTracker::VideoBasedTracker(bool showDebugWindows)
        : m_showDebugWindows(showDebugWindows) {

        /// Set up blob params
        m_params.minDistBetweenBlobs = 2.0f;

        m_params.minArea = 2.0f; // How small can the blobs be?

        // Look for bright blobs: there is a bug in this code
        m_params.filterByColor = false;
        // m_params.blobColor = static_cast<uchar>(255);

        m_params.filterByInertia = false; // Do we test for non-elongated blobs?
        // m_params.minInertiaRatio = 0.5;
        // m_params.maxInertiaRatio = 1.0;

        m_params.filterByConvexity = false; // Test for convexity?

        m_params.filterByCircularity = true; // Test for circularity?
        m_params.minCircularity = 0.5f; // default is 0.8, but the edge of the
        // case can make the blobs "weird-shaped"
    }
    void VideoBasedTracker::addOculusSensor() {
        /// @todo this clearly violates what I expected was the invariant - not
        /// sure if it's because of incomplete Oculus information, or due to a
        /// misunderstanding of how this all works.
        m_led_groups.emplace_back();
    }
    void VideoBasedTracker::addSensor(LedIdentifier *identifier,
                                      DoubleVecVec const &m,
                                      std::vector<double> const &d,
                                      Point3Vector const &locations,
                                      size_t requiredInliers,
                                      size_t permittedOutliers) {
        addSensor(LedIdentifierPtr(identifier), m, d, locations,
                  requiredInliers, permittedOutliers);
    }

    void VideoBasedTracker::addSensor(LedIdentifierPtr &&identifier,
                                      DoubleVecVec const &m,
                                      std::vector<double> const &d,
                                      Point3Vector const &locations,
                                      size_t requiredInliers,
                                      size_t permittedOutliers) {
        m_identifiers.emplace_back(std::move(identifier));
        m_estimators.emplace_back(new BeaconBasedPoseEstimator(
            m, d, locations, requiredInliers, permittedOutliers));
        m_led_groups.emplace_back();
        m_assertInvariants();
    }

    void VideoBasedTracker::addSensor(LedIdentifierPtr &&identifier,
                                      DoubleVecVec const &m,
                                      std::vector<double> const &d,
                                      Point3Vector const &locations,
                                      double variance, size_t requiredInliers,
                                      size_t permittedOutliers) {
        addSensor(std::move(identifier), m, d, locations, requiredInliers,
                  permittedOutliers);
        m_estimators.back()->SetBeacons(locations, variance);
    }

    void VideoBasedTracker::addSensor(LedIdentifierPtr &&identifier,
                                      DoubleVecVec const &m,
                                      std::vector<double> const &d,
                                      Point3Vector const &locations,
                                      std::vector<double> const &variance,
                                      size_t requiredInliers,
                                      size_t permittedOutliers) {
        addSensor(std::move(identifier), m, d, locations, requiredInliers,
                  permittedOutliers);
        m_estimators.back()->SetBeacons(locations, variance);
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

    bool VideoBasedTracker::processImage(cv::Mat frame, cv::Mat grayImage,
                                         OSVR_TimeValue const &tv,
                                         PoseHandler handler) {
        m_assertInvariants();
        bool done = false;
        m_frame = frame;
        m_imageGray = grayImage;
        auto foundKeyPoints = extractKeypoints(grayImage);
#if 0
        /// @todo maybe hoist to avoid allocations?
        KeypointEnhancer enh;
        auto foundKeyPoints =
            enh.enhanceKeypoints(grayImage, initialFoundKeyPoints);
        enh.getDebugImage().copyTo(m_keypointEnhancement);
#endif

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
            // @todo: Include motion estimate based on Kalman filter along with
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
                    ++led;
                }
            }
            // If we have any blobs that have not been associated with an
            // LED, then we add a new LED for each of them.
            // std::cout << "Had " << Leds.size() << " LEDs, " <<
            // keyPoints.size() << " new ones available" << std::endl;
            for (auto &keypoint : keyPoints) {
                m_led_groups[sensor].emplace_back(m_identifiers[sensor].get(),
                                                  keypoint.pt, keypoint.size);
            }

            //==================================================================
            // Compute the pose of the HMD w.r.t. the camera frame of reference.
            bool gotPose = false;
            if (m_estimators[sensor]) {

                // Get an estimated pose, if we have enough data.
                OSVR_PoseState pose;
                if (m_estimators[sensor]->EstimatePoseFromLeds(
                        m_led_groups[sensor], tv, pose)) {
#if 0
                    // Project the expected locations of the beacons
                    // into the image and then compute the error between the
                    // expected locations and the visible locations for all of
                    // the visible beacons.  If they are too far off, cancel the
                    // pose.
                    std::vector<cv::Point2f> imagePoints;
                    m_estimators[sensor]->ProjectBeaconsToImage(imagePoints);
                    for (auto &led : m_led_groups[sensor]) {
                        auto label = std::to_string(led.getOneBasedID());
                        cv::Point where = led.getLocation();
                    }
                    // XXX
#endif
                    m_pose = pose;
                    handler(static_cast<unsigned>(sensor), pose);
                    gotPose = true;
                }
            }

            if (m_showDebugWindows) {
                // Don't display the debugging info every frame, or we can't go
                // fast enough.
                static int count = 0;
                if (++count == 11) {
#ifdef OSVR_USE_SIMPLEBLOB
                    // Fake the thresholded image to give an idea of what the
                    // blob detector is doing.
                    auto getCurrentThresh = [&](int i) {
                        return i * m_params.thresholdStep +
                               m_params.minThreshold;
                    };
                    cv::Mat temp;
                    cv::threshold(m_imageGray, m_thresholdImage,
                                  m_params.minThreshold, m_params.minThreshold,
                                  CV_THRESH_BINARY);
                    cv::Mat tempOut;
                    for (int i = 1; getCurrentThresh(i) < m_params.maxThreshold;
                         ++i) {
                        auto currentThresh = getCurrentThresh(i);
                        cv::threshold(m_imageGray, temp, currentThresh,
                                      currentThresh, CV_THRESH_BINARY);
                        cv::addWeighted(m_thresholdImage, 0.5, temp, 0.5, 0,
                                        tempOut);
                        m_thresholdImage = tempOut;
                    }
#if 0
                    // Draw detected blobs as blue circles.
                    cv::drawKeypoints(
                        tempOut, foundKeyPoints, m_thresholdImage,
                        cv::Scalar(255, 0, 0),
                        cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
#endif
#endif
                    // Draw detected blobs as blue circles.
                    cv::drawKeypoints(
                        m_frame, foundKeyPoints, m_imageWithBlobs,
                        cv::Scalar(255, 0, 0),
                        cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

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
                        cv::imshow(windowName.str().c_str(), *m_shownImage);
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

                        case 'q':
                            // Indicate we want to quit.
                            done = true;
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

#ifdef OSVR_USE_SIMPLEBLOB
    std::vector<cv::KeyPoint>
    VideoBasedTracker::extractKeypoints(cv::Mat const &grayImage) {

        /// @todo this variable is a candidate for hoisting to member
        std::vector<cv::KeyPoint> foundKeyPoints;
        //================================================================
        // Tracking the points

        // Construct a blob detector and find the blobs in the image.
        double minVal, maxVal;
        cv::minMaxIdx(grayImage, &minVal, &maxVal);
        static int frames = 0;
        static int earlyOuts = 0;
        frames++;
        if (maxVal < BASE_NOISE_THRESHOLD) {
            earlyOuts++;
            /// empty image, early out!
            return foundKeyPoints;
        }

        if (frames % 2000 == 0) {
            std::cout << "Skipped " << earlyOuts << " of " << frames
                      << " frames due to max value falling below threshold."
                      << std::endl;
        }

        auto imageRangeLerp = [=](double alpha) {
            return minVal + (maxVal - minVal) * alpha;
        };
        // 0.3 LERP between min and max as the min threshold, but
        // don't let really dim frames confuse us.
        m_params.minThreshold =
            std::max(imageRangeLerp(0.3), BASE_NOISE_THRESHOLD);
        m_params.maxThreshold = imageRangeLerp(0.8);
        static const auto steps = 3;
        m_params.thresholdStep =
            (m_params.maxThreshold - m_params.minThreshold) / steps;
/// @todo: Make a different set of parameters optimized for the
/// Oculus Dk2.
/// @todo: Determine the maximum size of a trackable blob by seeing
/// when we're so close that we can't view at least four in the
/// camera.
#if CV_MAJOR_VERSION == 2
        cv::Ptr<cv::SimpleBlobDetector> detector =
            new cv::SimpleBlobDetector(m_params);
#elif CV_MAJOR_VERSION == 3
        auto detector = cv::SimpleBlobDetector::create(m_params);
#else
#error "Unrecognized OpenCV version!"
#endif
        detector->detect(grayImage, foundKeyPoints);

        // @todo: Consider computing the center of mass of a dilated bounding
        // rectangle around each keypoint to produce a more precise subpixel
        // localization of each LED.  The moments() function may be helpful
        // with this.

        // @todo: Estimate the summed brightness of each blob so that we can
        // detect when they are getting brighter and dimmer.  Pass this as
        // the brightness parameter to the Led class when adding a new one
        // or augmenting with a new frame.
        return foundKeyPoints;
    }
#endif

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

        // We don't need the exact m_params struct, but we use some similar
        // parameters, so why not re-use it.

        for (auto &contour : contours) {
            cv::Mat points(contour);
            cv::Moments moms = cv::moments(points);

            /// @todo any advantage to contourArea over using the moments m00?
            auto area = cv::contourArea(points) /*moms.m00*/;

            /// Filter by area
            if (area < m_params.minArea || area > m_params.maxArea) {
                continue;
            }
            /// Filter by circularity
            auto perim = cv::arcLength(points, true);
            auto circularity = 4 * M_PI * area / (perim * perim);
            if (circularity < m_params.minCircularity ||
                circularity > m_params.maxCircularity) {
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
