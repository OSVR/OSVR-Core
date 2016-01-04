/** @file
    @brief Header

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

#ifndef INCLUDED_VideoBasedTracker_h_GUID_831CC0DD_16A5_43AB_12D3_AE86E1998ED4
#define INCLUDED_VideoBasedTracker_h_GUID_831CC0DD_16A5_43AB_12D3_AE86E1998ED4

// Internal Includes
#include "Types.h"
#include "LED.h"
#include "LedIdentifier.h"
#include "BeaconBasedPoseEstimator.h"
#include "CameraParameters.h"
#include "SBDBlobExtractor.h"
#include <osvr/Util/ChannelCountC.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <boost/assert.hpp>

// Standard includes
#include <vector>
#include <list>
#include <functional>
#include <algorithm>

// Define the constant below to provide debugging (window showing video and
// behavior, printing tracked positions)
//#define VBHMD_DEBUG

namespace osvr {
namespace vbtracker {
    class VideoBasedTracker {
      public:
        VideoBasedTracker(ConfigParams const &params = ConfigParams{});

        static BeaconIDPredicate getDefaultBeaconFixedPredicate() {
            return [](int id) { return id <= 4; };
        }

        /// @name Sensor addition methods
        /// @{
        /// @brief Adds a sensor, given an LedIdentifier and parameters to
        /// create a pose estimator.
        /// @param identifier Takes unique ownership of the passed LedIdentifier
        /// object
        /// @param camParams An object with the camera matrix and distortion
        /// parameters.
        /// @param locations A list of the 3d locations (in mm) of each marker
        /// @param emissionDirection Normalized vectors for each beacon in body
        /// space giving their emission direction.
        /// @param variance A single default base measurement variance used as a
        /// starting point for all beacons.
        /// @param autocalibrationFixedPredicate A function that, when given a
        /// 1-based ID of a beacon, returns "true" if the autocalibration
        /// routines should consider that beacon "fixed" and not subject to
        /// autocalibration.
        /// @param requiredInliers How many "good" points must be available
        /// @param permittedOutliers How many additional "bad" points we can
        /// have
        void addSensor(LedIdentifierPtr &&identifier,
                       CameraParameters const &camParams,
                       Point3Vector const &locations,
                       Vec3Vector const &emissionDirection, double variance,
                       BeaconIDPredicate const &autocalibrationFixedPredicate =
                           getDefaultBeaconFixedPredicate(),
                       size_t requiredInliers = 4,
                       size_t permittedOutliers = 2) {
            addSensor(std::move(identifier), camParams, locations,
                      emissionDirection, std::vector<double>{variance},
                      autocalibrationFixedPredicate, requiredInliers,
                      permittedOutliers);
        }

        /// @overload
        ///
        /// For those who want the default variance but want to provide an
        /// autocalibration fixed predicate or more.
        void addSensor(LedIdentifierPtr &&identifier,
                       CameraParameters const &camParams,
                       Point3Vector const &locations,
                       Vec3Vector const &emissionDirection,
                       BeaconIDPredicate const &autocalibrationFixedPredicate,
                       size_t requiredInliers = 4,
                       size_t permittedOutliers = 2) {
            addSensor(std::move(identifier), camParams, locations,
                      emissionDirection, std::vector<double>{},
                      autocalibrationFixedPredicate, requiredInliers,
                      permittedOutliers);
        }
        /// @overload
        /// Takes a vector of default measurement variances, one per beacon. By
        /// default (if empty) a default overall base measurement variance is
        /// used. If only a single entry is in the vector, it is used for every
        /// beacon.
        ///
        /// (This is actually the one that does the work.)
        void addSensor(
            LedIdentifierPtr &&identifier, CameraParameters const &camParams,
            Point3Vector const &locations, Vec3Vector const &emissionDirection,
            std::vector<double> const &variance = std::vector<double>{},
            BeaconIDPredicate const &autocalibrationFixedPredicate =
                getDefaultBeaconFixedPredicate(),
            size_t requiredInliers = 4, size_t permittedOutliers = 2,
            double beaconAutocalibErrorScale = 1);
        /// @}

        typedef std::function<void(OSVR_ChannelCount, OSVR_Pose3 const &)>
            PoseHandler;

        /// @brief The main method that processes an image into tracked poses.
        /// @return true if user hit q to quit in a debug window, if such a
        /// thing exists.
        bool processImage(cv::Mat frame, cv::Mat grayImage,
                          OSVR_TimeValue const &tv, PoseHandler handler);

        /// For debug purposes
        BeaconBasedPoseEstimator const &getFirstEstimator() const {
            return *(m_estimators.front());
        }

        /// For debug purposes
        BeaconBasedPoseEstimator &getFirstEstimator() {
            return *(m_estimators.front());
        }

      private:
        /// @overload
        /// For advanced usage - this one requires YOU to add your beacons by
        /// passing a functor (probably a lambda) to do so.
        void addSensor(
            LedIdentifierPtr &&identifier, CameraParameters const &camParams,
            std::function<void(BeaconBasedPoseEstimator &)> const &beaconAdder,
            size_t requiredInliers = 4, size_t permittedOutliers = 2);

        void dumpKeypointDebugData(std::vector<cv::KeyPoint> const &keypoints);

        void drawLedCircleOnStatusImage(Led const &led, bool filled,
                                        cv::Vec3b color);
        void drawRecognizedLedIdOnStatusImage(Led const &led);

        bool m_debugHelpDisplayed = false;
        /// @name Images
        /// @{
        cv::Mat m_frame;
        cv::Mat m_imageGray;
        cv::Mat m_thresholdImage;
        cv::Mat m_imageWithBlobs;
        cv::Mat m_statusImage;
        cv::Mat *m_shownImage = &m_statusImage;
        int m_debugFrame = 0;
        /// @}

        ConfigParams m_params;
        SBDBlobExtractor m_blobExtractor;
        cv::SimpleBlobDetector::Params m_sbdParams;

        /// @brief Test (with asserts) what Ryan thinks are the invariants. Will
        /// inline right out of existence in non-debug builds.
        void m_assertInvariants() const {
            BOOST_ASSERT_MSG(
                m_identifiers.size() == m_led_groups.size(),
                "Expected to have as many identifier objects as LED groups");
            BOOST_ASSERT_MSG(m_identifiers.size() == m_estimators.size(),
                             "Expected to have as many identifier objects as "
                             "estimator objects");

            for (auto &e : m_estimators) {
                BOOST_ASSERT_MSG(
                    e->getNumBeacons() > 4,
                    "Expected each estimator to have at least 4 beacons");
            }
        }
        /// @name Structures needed to do the tracking.
        /// @{
        LedIdentifierList m_identifiers;
        LedGroupList m_led_groups;
        EstimatorList m_estimators;
        /// @}

        /// @brief The pose that we report
        OSVR_PoseState m_pose;

        /// A captured copy of the camera parameters;
        CameraParameters m_camParams;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_VideoBasedTracker_h_GUID_831CC0DD_16A5_43AB_12D3_AE86E1998ED4
