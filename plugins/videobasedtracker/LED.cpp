/** @file
@brief Implementation for class that tracks and identifies LEDs.

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

#include "LED.h"

namespace osvr {
namespace vbtracker {

    Led::Led(LedIdentifier *identifier, float x, float y,
             Brightness brightness) {
        m_identifier = identifier;
        m_location.x = x;
        m_location.y = y;
        m_id = -1;
        m_brightnessHistory.push_back(brightness);
    }

    Led::Led(LedIdentifier *identifier, cv::Point2f loc,
             Brightness brightness) {
        m_identifier = identifier;
        m_location = loc;
        m_id = -1;
        m_brightnessHistory.push_back(brightness);
    }

    void Led::addMeasurement(cv::Point2f loc, Brightness brightness) {
        m_location = loc;
        m_brightnessHistory.push_back(brightness);

        // If we don't have an identifier, then our ID is unknown.
        // Otherwise, try and find it.
        if (!m_identifier) {
            m_id = -1;
        } else {
            m_id = m_identifier->getId(m_brightnessHistory);
        }
    }

    KeyPointIterator Led::nearest(KeyPointList &keypoints,
                                  double threshold) const {
        // If we have no elements in the vector, return the end().
        if (keypoints.empty()) {
            return end(keypoints);
        }

        // Find the distance to the first point and record it as the
        // current minimum distance;
        auto ret = begin(keypoints);
        double minDist = sqrt(norm(m_location - ret->pt));

        // Search the rest of the elements to see if we can find a
        // better one.
        auto it = keypoints.begin();
        while (++it != keypoints.end()) {
            double dist = sqrt(norm(m_location - it->pt));
            if (dist < minDist) {
                minDist = dist;
                ret = it;
            }
        }

        // If the closest is within the threshold, return it.  Otherwise,
        // return the end.
        if (minDist <= threshold) {
            return ret;
        } else {
            return end(keypoints);
        }
    }

} // End namespace vbtracker
} // End namespace osvr
