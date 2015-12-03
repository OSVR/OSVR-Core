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

    Led::Led(LedIdentifier *identifier, float x, float y, Brightness brightness)
        : m_id(-1), m_identifier(identifier) {
        addMeasurement(cv::Point2f(x, y), brightness);
    }

    Led::Led(LedIdentifier *identifier, cv::Point2f loc, Brightness brightness)
        : m_id(-1), m_identifier(identifier) {
        addMeasurement(loc, brightness);
    }

    void Led::addMeasurement(cv::Point2f loc, Brightness brightness) {
        m_location = loc;
        m_brightnessHistory.push_back(brightness);

        // If we don't have an identifier, then our ID is unknown.
        // Otherwise, try and find it.
        if (!m_identifier) {
            m_id = -1;
        } else {
            m_id = m_identifier->getId(m_brightnessHistory, m_lastBright);
        }
    }

    KeyPointIterator Led::nearest(KeyPointList &keypoints,
                                  double threshold) const {
        // If we have no elements in the vector, return the end().
        if (keypoints.empty()) {
            return end(keypoints);
        }

        auto location = m_location;
        /// @todo can we use squared-norm instead of doing a square-root inside
        /// of a tight loop like this?
        auto computeDist = [location](KeyPointIterator it) {
            return sqrt(norm(location - it->pt));
        };

        // Find the distance to the first point and record it as the
        // current minimum distance;
        auto ret = begin(keypoints);
        auto minDist = computeDist(ret);

        // Search the rest of the elements to see if we can find a
        // better one.
        for (auto it = begin(keypoints), e = end(keypoints); it != e; ++it) {
            auto dist = computeDist(it);
            if (dist < minDist) {
                minDist = dist;
                ret = it;
            }
        }

        // If the closest is within the threshold, return it.  Otherwise,
        // return the end.
        if (minDist <= threshold) {
            return ret;
        }
        return end(keypoints);
    }

} // End namespace vbtracker
} // End namespace osvr
