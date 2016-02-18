/** @file
    @brief Header file for class that tracks and identifies LEDs.

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

#ifndef INCLUDED_LED_h_GUID_C53E1134_AD6B_46B0_4808_19C7EAA7D0EC
#define INCLUDED_LED_h_GUID_C53E1134_AD6B_46B0_4808_19C7EAA7D0EC

// Internal Includes
#include "LedIdentifier.h"
#include "LedMeasurement.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace vbtracker {

    static const bool USING_INVERTED_LED_POSITION = true;

    /// @brief Helper class to keep track of the state of a blob over time. This
    /// is used to help determine the identity of each LED in the scene. The
    /// LEDs are identified by their blink codes.  A steady one is presumed to
    /// be a light source.
    class Led {
      public:
        static const int SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA =
            -1;
        static const int SENTINEL_INSUFFICIENT_EXTREMA_DIFFERENCE = -2;
        static const int
            SENTINEL_NO_PATTERN_RECOGNIZED_DESPITE_SUFFICIENT_DATA = -3;

        /// @name Constructors
        /// @brief Constructor takes initial values for the location and
        /// brightness, and a pointer to an object that will be used to identify
        /// the LEDs based on their brightness over time.
        /// @{
        Led(LedIdentifier *identifier, LedMeasurement const &meas);
        /// @}

        static const uint8_t MAX_NOVELTY = 4;
        /// @brief Add a new measurement for this LED, which must be for a frame
        /// that is just following the previous measurement, so that the
        /// encoding of brightness and darkness can be used to identify it.
        /// @param meas A number of measurement parameters, of which only
        /// `brightness` and location are used in identifying beacon patterns.
        /// brightness is an abstract quantity that is fed into the identity
        /// detector; it may be area or summed brightness or another
        /// useful estimate of the LED state. The other parameters are carried
        /// along in the hope that they may be useful to later code. (For
        /// instance, not all identified beacons may be good choices to use in
        /// determining tracking pose)
        void addMeasurement(LedMeasurement const &meas, bool blobsKeepId);

        LedMeasurement const &getMeasurement() const {
            return m_latestMeasurement;
        }

        /// @brief Tells which LED I am.
        ///
        /// - An index of -1 means not yet determined.
        /// - An index below -1 means known not to be an LED (different
        ///   identifiers use different codes to differentiate between cases).
        /// - An index of 0 or higher is determined based on the flash pattern.
        ZeroBasedBeaconId getID() const { return m_id; }

        /// @brief Gets either the raw negative sentinel ID or a 1-based ID (for
        /// display purposes)
        OneBasedBeaconId getOneBasedID() const { return makeOneBased(getID()); }

        /// @brief Do we have a positive identification as a known LED?
        bool identified() const { return beaconIdentified(getID()); }

        /// @brief Returns a value (decreasing per frame from some maximum down
        /// to a minimum of zero) indicating how new the identification of this
        /// blob with its current ID is. This can be used to compensate for
        /// accidental mis-identifications, identity switching, or the simple
        /// fact that new identifications might contain highly novel information
        /// that would otherwise "shock" the tracked state.
        uint8_t novelty() const { return m_novelty; }

        /// @brief Reports the most-recently-added position.
        cv::Point2f getLocation() const { return m_latestMeasurement.loc; }

        /// @brief Gets the most-recently-added position, in a
        /// xy-origin-at-bottom-left coordinate system
        cv::Point2f getInverseLocation() const {
            return cv::Point2f(m_latestMeasurement.imageSize.width -
                                   m_latestMeasurement.loc.x,
                               m_latestMeasurement.imageSize.height -
                                   m_latestMeasurement.loc.y);
        }

        /// @brief Gets the most-recently-added position in the coordinate
        /// system to be used for tracking.
        ///
        /// Provides a centralized place to switch between getLocation() and
        /// getInverseLocation() for tracking purposes.
        cv::Point2f getLocationForTracking() const {
            return USING_INVERTED_LED_POSITION ? getInverseLocation()
                                               : getLocation();
        }

        /// @brief Find the nearest KeyPoint from a container of points to me,
        /// if there is one within the specified threshold.  Runtime: O(n) where
        /// n is the number of elements in keypoints.
        /// @return end() if there is not a nearest within threshold (or an
        /// empty container).
        KeyPointIterator nearest(KeyPointList &keypoints,
                                 double threshold) const;

        /// @overload
        LedMeasurementVecIterator nearest(LedMeasurementVec &meas,
                                          double threshold) const;

        /// @brief Returns the most-recent boolean "bright" state according to
        /// the LED identifier. Note that the value is only meaningful if
        /// `identified()` is true.
        bool isBright() const { return m_lastBright; }

        /// Used for a status display in debug windows.
        bool wasUsedLastFrame() const { return m_wasUsedLastFrame; }

        /// Call from inside the tracking algorithm to mark that it was used.
        void markAsUsed() { m_wasUsedLastFrame = true; }

        void resetUsed() { m_wasUsedLastFrame = false; }

        /// Called from within pose estimation or elsewhere with model-based
        /// knowledge that can refute the identification of this blob.
        void markMisidentified();

      private:
        /// Most recent measurement
        LedMeasurement m_latestMeasurement;

        /// Starting from current frame going backwards
        BrightnessList m_brightnessHistory;

        /// @brief Which LED am I? Non-negative are indices, negative are
        /// sentinels
        ZeroBasedBeaconId m_id = ZeroBasedBeaconId(
            SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA);

        /// @brief Object used to determine the identity of an LED
        LedIdentifier *m_identifier = nullptr;

        /// @brief If identified, whether it is most recently in "bright" mode.
        bool m_lastBright = false;

        bool m_newlyRecognized = false;
        uint8_t m_novelty;

        bool m_wasUsedLastFrame = false;
    };

} // End namespace vbtracker
} // End namespace osvr

#endif // INCLUDED_LED_h_GUID_C53E1134_AD6B_46B0_4808_19C7EAA7D0EC
