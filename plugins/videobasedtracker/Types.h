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
#include <functional>

namespace osvr {
namespace vbtracker {
    class Led;
    class LedIdentifier;
    class BeaconBasedPoseEstimator;

    typedef std::vector<cv::Point3f> Point3Vector;

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

    /// Takes in a 1-based index, returns true or false (true if the beacon
    /// should be considered fixed - not subject to autocalibration)
    using BeaconIDPredicate = std::function<bool(int)>;

    /// Blob detection configuration parameters
    struct BlobParams {
        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector - in
        /// pixel units
        float minDistBetweenBlobs = 2.0f;
        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector - in
        /// pixel units
        float minArea = 2.0f;
        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector
        float minCircularity = 0.5;
        /// This is the absolute minimum pixel value that will be considered as
        /// a possible signal. Images that contain only values below this will
        /// be totally discarded as containing zero keypoints.
        double absoluteMinThreshold = 75.;
        /// This value, in the range (0, 1), is the linear interpolation factor
        /// between the minimum and maximum value pixel in a frame that will be
        /// the *minimum* threshold value used by the simple blob detector (if
        /// it does not drop below absoluteMinThreshold)
        double minThresholdAlpha = 0.3;
        /// This value, in the range (0, 1), is the linear interpolation factor
        /// between the minimum and maximum value pixel in a frame that will be
        /// the *maximum* threshold value used by the simple blob detector (if
        /// it does not drop below absoluteMinThreshold)
        double maxThresholdAlpha = 0.7;
        /// This is the number of thresholding and contour extraction steps that
        /// the blob extractor will take between the two threshold extrema, and
        /// thus greatly impacts performance. Adjust with care.
        int thresholdSteps = 3;
    };
    /// General configuration parameters
    struct ConfigParams {
        /// Parameters specific to the blob-detection step of the algorithm
        BlobParams blobParams;
        /// Seconds beyond the current time to predict, using the Kalman state.
        double additionalPrediction = 24. / 1000.;
        /// Max residual (pixel units) for a beacon before throwing that
        /// measurement out.
        double maxResidual = 100;
        /// Initial beacon error for autocalibration (units: mm^2).
        /// 0 effectively turns off beacon auto-calib.
        /// This is a variance number, so std deviation squared, but it's
        /// pretty likely to be between 0 and 1, so the variance will be smaller
        /// than the standard deviation.
        double initialBeaconError = 0.005;
        /// Maximum distance a blob can move, in pixel units, and still be
        /// considered the same blob.
        double blobMoveThreshold = 10.;
        bool debug = false;
        /// How many threads to let OpenCV use.
        int numThreads = 1;
        /// This is the autocorrelation kernel of the process noise. The first
        /// three elements correspond to position, the second three to
        /// incremental rotation.
        double processNoiseAutocorrelation[6];
        /// The value used in exponential decay of linear velocity: it's the
        /// proportion of that velocity remaining at the end of 1 second. Thus,
        /// smaller = faster decay/higher damping.
        double linearVelocityDecayCoefficient = 0.3;
        /// The value used in exponential decay of angular velocity: it's the
        /// proportion of that velocity remaining at the end of 1 second. Thus,
        /// smaller = faster decay/higher damping.
        double angularVelocityDecayCoefficient = 0.01;
        /// The measurement variance (units: mm^2) is included in the plugin
        /// along with the coordinates of the beacons. Some beacons are observed
        /// with higher variance than others, due to known difficulties in
        /// tracking them, etc. However, for testing you may fine-tine the
        /// measurement variances globally by scaling them here.
        double measurementVarianceScaleFactor = 1.;

        /// Whether the tracking algorithm internally adjusts beacon positions
        /// based on the centroid of the input beacon positions.
        bool offsetToCentroid = true;

        /// Manual beacon offset (in mm) - only really sensible if you only have
        /// one target, only used if offsetToCentroid is false.
        double manualBeaconOffset[3];

        /// If true, this will replace the two sensors with just a single one,
        /// including the beacons at the back of the head "rigidly" as a part of
        /// it. If true, recommend offsetToCentroid = false, and
        /// manualBeaconOffset to be 0, 0, -75.
        bool includeRearPanel = false;

        /// Head circumference at the head strap, in cm - 55.75 is our estimate
        /// for an average based on some hat sizing guidelines. Only matters if
        /// includeRearPanel is true.
        double headCircumference = 55.75;

        /// This is the distance fron the front of the head to the origin of the
        /// front sensor coordinate system in the Z axis, in mm.
        /// @todo this is an estimate!
        double headToFrontBeaconOriginDistance = 75;

        double backPanelMeasurementError = 1.5;

        ConfigParams() {
            // Apparently I can't non-static-data-initializer initialize an
            // array member. Sad. GCC almost let me. MSVC said no way.
            processNoiseAutocorrelation[0] = 1e+2;
            processNoiseAutocorrelation[1] = 5e+1;
            processNoiseAutocorrelation[2] = 1e+2;
            processNoiseAutocorrelation[3] = 5e-1;
            processNoiseAutocorrelation[4] = 5e-1;
            processNoiseAutocorrelation[5] = 5e-1;
            manualBeaconOffset[0] = 0;
            manualBeaconOffset[1] = 0;
            manualBeaconOffset[2] = 0;
        }
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
