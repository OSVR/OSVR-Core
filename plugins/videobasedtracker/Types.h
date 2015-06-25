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
// - none

// Library/third-party includes
#include <opencv2/features2d/features2d.hpp>

// Standard includes
#include <vector>
#include <list>
#include <string>
#include <memory>

namespace osvr {
namespace vbtracker {
    class Led;
    class LedIdentifier;
    class BeaconBasedPoseEstimator;

    /// @todo Replace usages of this with Eigen or cv matrices.
    typedef std::vector<std::vector<double> > DoubleVecVec;

    typedef std::vector<std::string> PatternStringList;

    /// @todo std::list used here for ease of rotate, but has terrible data
    /// locality - consider changing when a more efficient rotation-invariant
    /// string match algorithm is used.
    typedef std::list<bool> LedPattern;
    typedef std::vector<LedPattern> PatternList;

    typedef std::vector<cv::KeyPoint> KeyPointList;
    typedef KeyPointList::iterator KeyPointIterator;

    typedef float Brightness;
    typedef std::list<Brightness> BrightnessList;
    typedef std::pair<Brightness, Brightness> BrightnessMinMax;

    typedef std::unique_ptr<BeaconBasedPoseEstimator> EstimatorPtr;
    typedef std::unique_ptr<LedIdentifier> LedIdentifierPtr;

    typedef std::list<Led> LedGroup;

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
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
