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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "LedIdentifier.h"

#include <opencv2/opencv.hpp>
#include <vector>

namespace osvr {
namespace vbtracker {

//----------------------------------------------------------------------
// Helper class to keep track of the state of a blob over time.  This
// is used to help determine the identity of each LED in the scene.
// The LEDs are identified by their blink codes.  A steady one is
// presumed to be a light source.
class Led {
public:
    // Constructor takes initial values for the location and brightness,
    // and a pointer to an object that will be used to identify the LEDs
    // based on their brightness over time.
    Led(LedIdentifier *identifier, float x = 0, float y = 0, float brightness = 0);
    Led(LedIdentifier *identifier, cv::Point2f loc, float brightness = 0);

    // Add a new measurement for this LED, which must be for a frame that
    // is just following the previous measurement, so that the encoding
    // of brightness and darkness can be used to identify it.
    // "brightness" is an abstract quantity that is fed into the identity
    // detector; it may be area or summed brightness or another useful estimate
    // of the LED state.
    void addMeasurement(cv::Point2f loc, float brightness);

    // Tells which LED I am.  An index of -1 means not yet determined.
    // An index below -1 means known not to be an LED (different identifiers
    // use different codes to differentiate between cases).
    // An index of 0 or higher is determined based on the flash pattern.
    int getID() const { return m_id; }

    // Reports the most-recently-added position.
    cv::Point2f getLocation() const { return m_location; }

    // Find the nearest KeyPoint from a vector of points to me, if there is
    // one within the specified threshold.
    // Returns end() if there is not a nearest within threshold (or an empty vector).
    std::vector<cv::KeyPoint>::iterator nearest(
        std::vector<cv::KeyPoint> &vec
        , double threshold) const;

private:
    std::list<float> m_brightnessHistory;  //< Starting from current frame going backwards
    int            m_id;           //< Which LED am I?
    cv::Point2f    m_location;     //< Most recent recorded location
    LedIdentifier *m_identifier;   //< Class used to determine the identity of an LED
};

} // End namespace vbtracker
} // End namespace osvr
