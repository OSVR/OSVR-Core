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
#include "BasicTypes.h"
#include "BlobParams.h"

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

    /// @todo Replace usages of this with Eigen or cv matrices.
    typedef std::vector<std::vector<double> > DoubleVecVec;

    typedef std::vector<std::string> PatternStringList;

    typedef std::vector<LedPatternWrapped>
        PatternList; //< Ordered set of patterns to search

    typedef std::vector<cv::KeyPoint> KeyPointList;
    typedef KeyPointList::iterator KeyPointIterator;

    struct LedMeasurement;
    typedef std::vector<LedMeasurement> LedMeasurementList;
    typedef LedMeasurementList::iterator LedMeasurementIterator;

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

    /// General configuration parameters
    struct ConfigParams {
        /// Parameters specific to the blob-detection step of the algorithm
        BlobParams blobParams;

        /// Seconds beyond the current time to predict, using the Kalman state.
        double additionalPrediction = 24. / 1000.;

        /// Max residual (pixel units) for a beacon before applying a variance
        /// penalty.
        double maxResidual = 75;
        /// Initial beacon error for autocalibration (units: mm^2).
        /// 0 effectively turns off beacon auto-calib.
        /// This is a variance number, so std deviation squared, but it's
        /// pretty likely to be between 0 and 1, so the variance will be smaller
        /// than the standard deviation.
        double initialBeaconError = 0.001;

        /// Maximum distance a blob can move, in multiples of its previous
        /// "keypoint diameter", and still be considered the same blob.
        double blobMoveThreshold = 4.;

        /// Whether to show the debug windows and debug messages.
        bool debug = false;

        /// How many threads to let OpenCV use. Set to 0 or less to let OpenCV
        /// decide (that is, not set an explicit preference)
        int numThreads = 1;

        /// This is the autocorrelation kernel of the process noise. The first
        /// three elements correspond to position, the second three to
        /// incremental rotation.
        double processNoiseAutocorrelation[6];

        /// The value used in exponential decay of linear velocity: it's the
        /// proportion of that velocity remaining at the end of 1 second. Thus,
        /// smaller = faster decay/higher damping. In range [0, 1]
        double linearVelocityDecayCoefficient = 0.9;

        /// The value used in exponential decay of angular velocity: it's the
        /// proportion of that velocity remaining at the end of 1 second. Thus,
        /// smaller = faster decay/higher damping. In range [0, 1]
        double angularVelocityDecayCoefficient = 0.9;

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
        /// This is a rough estimate - the origin of the coordinate system is
        /// roughly the flat part of the hard plastic.
        double headToFrontBeaconOriginDistance = 0;

        /// This used to be different than the other beacons, but now it's
        /// mostly the same.
        double backPanelMeasurementError = 3.0;

        /// This is the process-model noise in the beacon-auto-calibration, in
        /// mm^2/s. Not fully accurate, since it only gets applied when a beacon
        /// gets used for a measurement, but it should be enough to keep beacons
        /// from converging in a bad local minimum.
        double beaconProcessNoise = 1.e-15;

        /// This is the multiplicative penalty applied to the variance of
        /// measurements with a "bad" residual
        double highResidualVariancePenalty = 10.;

        /// When true, will stream debug info (variance, pixel measurement,
        /// pixel residual) on up to the first 34 beacons of your first sensor
        /// as analogs.
        bool streamBeaconDebugInfo = false;

        /// This should be the ratio of lengths of sides that you'll permit to
        /// be filtered in. Larger side first, please.
        ///
        /// Not currently being used.
        float boundingBoxFilterRatio = 5.f / 4.f;

        /// This should be a negative number - it's the largest the z component
        /// of the camera-space LED emission vector is permitted to be and still
        /// be used in estimation. acos(this number) is the maximum angle away
        /// from pointing at the camera that we'll accept an LED pointing.
        double maxZComponent = -0.3;

        /// Should we attempt to skip bright-mode LEDs? The alternative is to
        /// just give them slightly higher variance.
        bool shouldSkipBrightLeds = false;

        /// If this option is set to true, then while some of the pattern
        /// identifier is run each frame, an "early-out" will be taken if the
        /// blob/LED already has a valid (non-negative) ID assigned to it. This
        /// can help keep IDs on hard to identify blobs, but it can also persist
        /// errors longer. That's why it's an option.
        ///
        /// Defaulting to off because it adds some jitter for some reason.
        bool blobsKeepIdentity = false;

        /// Extra verbose developer debugging messages (right now just "hey, you
        /// can't possibly be that beacon, I shouldn't be able to see you"
        /// messages)
        bool extraVerbose = false;

        /// If non-empty, the file to load (or save to) for calibration data.
        /// Only make sense for a single target.
        std::string calibrationFile = "";

        ConfigParams() {
            // Apparently I can't non-static-data-initializer initialize an
            // array member. Sad. GCC almost let me. MSVC said no way.
            processNoiseAutocorrelation[0] = 3e+2;
            processNoiseAutocorrelation[1] = 3e+2;
            processNoiseAutocorrelation[2] = 3e+2;
            processNoiseAutocorrelation[3] = 1e0;
            processNoiseAutocorrelation[4] = 1e0;
            processNoiseAutocorrelation[5] = 1e0;

            /// If you use manual beacon offset (aka turn off offsetToCentroid),
            /// this is a good default since it's the best beacon offset for the
            /// HDK we've found so far - centroid of front beacons, with only z
            /// component retained.
            manualBeaconOffset[0] = 0;
            manualBeaconOffset[1] = 0;
            manualBeaconOffset[2] = 38.8676;
        }
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_Types_h_GUID_819757A3_DE89_4BAD_3BF5_6FE152F1EA08
