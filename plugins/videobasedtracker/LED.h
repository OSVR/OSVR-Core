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

// Library/third-party includes
#include <opencv2/opencv.hpp>

// Standard includes
#include <vector>

namespace osvr {
namespace vbtracker {

    /// @brief Helper class to keep track of the state of a blob over time. This
    /// is used to help determine the identity of each LED in the scene. The
    /// LEDs are identified by their blink codes.  A steady one is presumed to
    /// be a light source.
    class Led {
      public:
        /// @name Constructors
        /// @brief Constructor takes initial values for the location and
        /// brightness, and a pointer to an object that will be used to identify
        /// the LEDs based on their brightness over time.
        /// @todo Would it ever be valid to pass in nullptr? if not, let's take
        /// in a reference instead to better convey the semantics.
        /// @{
        Led(LedIdentifier *identifier, float x = 0, float y = 0,
            Brightness brightness = 0);
        Led(LedIdentifier *identifier, cv::Point2f loc,
            Brightness brightness = 0);
        /// @}

        /// @brief Add a new measurement for this LED, which must be for a frame
        /// that is just following the previous measurement, so that the
        /// encoding of brightness and darkness can be used to identify it.
        /// @param brightness is an abstract quantity that is fed into the
        /// identity detector; it may be area or summed brightness or another
        /// useful estimate of the LED state.
        void addMeasurement(cv::Point2f loc, Brightness brightness);

        /// @brief Tells which LED I am.
        ///
        /// - An index of -1 means not yet determined.
        /// - An index below -1 means known not to be an LED (different
        ///   identifiers use different codes to differentiate between cases).
        /// - An index of 0 or higher is determined based on the flash pattern.
        int getID() const { return m_id; }

        /// @brief Gets either the raw negative sentinel ID or a 1-based ID (for
        /// display purposes)
        int getOneBasedID() const { return (m_id < 0) ? m_id : m_id + 1; }

        /// @brief Do we have a positive identification as a known LED?
        bool identified() const { return !(m_id < 0); }

        /// @brief Reports the most-recently-added position.
        cv::Point2f getLocation() const { return m_location; }

        /// @brief Find the nearest KeyPoint from a container of points to me,
        /// if there is one within the specified threshold.
        /// @return end() if there is not a nearest within threshold (or an
        /// empty container).
        KeyPointIterator nearest(KeyPointList &keypoints,
                                 double threshold) const;

        /// @brief Returns the most-recent boolean "bright" state according to
        /// the LED identifier. Note that the value is only meaningful if
        /// `identified()` is true.
        bool isBright() const { return m_lastBright; }

      private:
        /// Starting from current frame going backwards
        BrightnessList m_brightnessHistory;

        /// @brief Which LED am I? Non-negative are indices, negative are
        /// sentinels
        int m_id;

        /// @brief Most recent recorded location
        cv::Point2f m_location;

        /// @brief Object used to determine the identity of an LED
        LedIdentifier *m_identifier;

        /// @brief If identified, whether it is most recently in "bright" mode.
        bool m_lastBright = false;
    };

} // End namespace vbtracker
} // End namespace osvr

#endif // INCLUDED_LED_h_GUID_C53E1134_AD6B_46B0_4808_19C7EAA7D0EC
