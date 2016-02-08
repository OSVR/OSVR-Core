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

#ifndef INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
#define INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08

// Internal Includes
#include "ConfigParams.h"

// Library/third-party includes
#include <opencv2/features2d/features2d.hpp>

// Standard includes
#include <vector>
#include <list>
#include <string>
#include <memory>
#include <functional>

namespace osvr {
namespace vbtracker {
/// @todo Remove when we no longer assume a single camera or monotonic camera
/// timestamps, and the build will break in a few places
/// where known "gotchas" exist
    #define OSVR_UVBI_ASSUME_SINGLE_CAMERA
    /// @todo Remove when we no longer assume that IMU reports arrive before video reports with same timestamps.
    #define OSVR_UVBI_ASSUME_CAMERA_ALWAYS_SLOWER


    class Led;
    class LedIdentifier;
    class BeaconBasedPoseEstimator;

    typedef std::vector<cv::Point3f> Point3Vector;

    typedef std::vector<cv::Vec3d> Vec3Vector;

    /// @todo Replace usages of this with Eigen or cv matrices.
    typedef std::vector<std::vector<double> > DoubleVecVec;

    typedef std::vector<std::string> PatternStringList;

    typedef std::string LedPatternWrapped; //< Pattern repeated almost twice
    typedef std::vector<LedPatternWrapped>
        PatternList; //< Ordered set of patterns to search

    typedef std::vector<cv::KeyPoint> KeyPointList;
    typedef KeyPointList::iterator KeyPointIterator;

    typedef float Brightness;
    typedef std::list<Brightness> BrightnessList;
    typedef std::pair<Brightness, Brightness> BrightnessMinMax;

    typedef std::unique_ptr<BeaconBasedPoseEstimator> EstimatorPtr;
    typedef std::unique_ptr<LedIdentifier> LedIdentifierPtr;

    typedef std::list<Led> LedGroup;
    using LedPtrList = std::vector<Led *>;

    struct LedMeasurement {

        LedMeasurement() = default;
        explicit LedMeasurement(cv::KeyPoint const &kp)
            : loc(kp.pt), brightness(kp.size), diameter(kp.size),
              area(static_cast<float>((diameter / 2) * (diameter / 2) *
                                      CV_PI)) {}

        /// Location in image space - should be undistorted when passed to the
        /// Led class.
        cv::Point2f loc;
        /// "Brightness" - currently actually diameter or radius.
        Brightness brightness;

        /// Blob diameter in pixels.
        float diameter = 0.f;

        /// Area in pixels
        float area = 1.f;

        /// Blob circularity (as defined by OpenCV) - in [0,1]
        float circularity = 0.f;

        /// Do we know an upright bounding box? (that is, is the next member
        /// valid?)
        bool knowBoundingBox = false;

        /// Dimensions of the upright bounding box.
        cv::Size2f boundingBox;
    };
    typedef std::vector<LedMeasurement> LedMeasurementVec;
    typedef LedMeasurementVec::iterator LedMeasurementVecIterator;

    /// @name Containers of "per-sensor" objects
    /// @brief It seems like in a "well-formed" video-based tracker, there is
    /// one element in each of these containers for each "sensor" (known rigid
    /// organization of markers), and these are parallel (indices between them
    /// correlate)
    /// @{
    typedef std::vector<LedIdentifierPtr> LedIdentifierList;
    typedef std::vector<LedGroup> LedGroupList;
    typedef std::vector<EstimatorPtr> EstimatorList;
    /// @}

    /// Takes in a 1-based index, returns true or false (true if the beacon
    /// should be considered fixed - not subject to autocalibration)
    using BeaconIDPredicate = std::function<bool(int)>;

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
