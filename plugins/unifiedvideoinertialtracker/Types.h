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
#include "Assumptions.h"

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

    class Led;
    class LedIdentifier;

    typedef std::vector<cv::Point3f> Point3Vector;

    typedef std::vector<cv::Vec3d> Vec3Vector;

    typedef std::vector<std::string> PatternStringList;

    typedef std::string LedPatternWrapped; //< Pattern repeated almost twice
    typedef std::vector<LedPatternWrapped>
        PatternList; //< Ordered set of patterns to search

    typedef std::vector<cv::KeyPoint> KeyPointList;
    typedef KeyPointList::iterator KeyPointIterator;

    typedef float Brightness;
    typedef std::list<Brightness> BrightnessList;
    typedef std::pair<Brightness, Brightness> BrightnessMinMax;

    typedef std::unique_ptr<LedIdentifier> LedIdentifierPtr;

    typedef std::list<Led> LedGroup;
    using LedPtrList = std::vector<Led *>;

    struct LedMeasurement {

        LedMeasurement() = default;
        LedMeasurement(cv::KeyPoint const &kp, cv::Size imgSize)
            : loc(kp.pt), imageSize(imgSize), brightness(kp.size),
              diameter(kp.size), area(static_cast<float>(
                                     (diameter / 2) * (diameter / 2) * CV_PI)) {
        }

        /// Location in image space - should be undistorted when passed to the
        /// Led class.
        cv::Point2f loc;

        /// Size of the image the measurement came from.
        cv::Size imageSize;

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

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
