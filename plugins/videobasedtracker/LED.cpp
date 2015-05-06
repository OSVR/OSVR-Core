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

Led::Led(LedIdentifier *identifier, float x, float y, float brightness)
{
    m_identifier = identifier; m_location.x = x; m_location.y = y; m_id = -1;
    m_brightnessHistory.push_back(brightness);
}

Led::Led(LedIdentifier *identifier, cv::Point2f loc, float brightness)
{
    m_identifier = identifier; m_location = loc; m_id = -1;
    m_brightnessHistory.push_back(brightness);
}

void Led::addMeasurement(cv::Point2f loc, float brightness)
{
    m_location = loc;
    m_brightnessHistory.push_back(brightness);

    // If we don't have an identifier, then our ID is unknown.
    // Otherwise, try and find it.
    if (!m_identifier) {
        m_id = -1;
    }
    else {
        m_id = m_identifier->getId(m_brightnessHistory);
    }
}

std::vector<cv::KeyPoint>::iterator Led::nearest(std::vector<cv::KeyPoint> &vec
    , double threshold) const
{
    // If we have no elements in the vector, return the end().
    if (vec.size() == 0) { return vec.end(); }

    // Find the distance to the first point and record it as the
    // current minimum distance;
    std::vector<cv::KeyPoint>::iterator ret = vec.begin();
    double minDist = sqrt(norm(m_location - ret->pt));

    // Search the rest of the elements to see if we can find a
    // better one.
    std::vector<cv::KeyPoint>::iterator i = vec.begin();
    while (++i != vec.end()) {
        double dist = sqrt(norm(m_location - i->pt));
        if (dist < minDist) {
            minDist = dist;
            ret = i;
        }
    }

    // If the closest is within the threshold, return it.  Otherwise,
    // return the end.
    if (minDist <= threshold) {
        return ret;
    }
    else {
        return vec.end();
    }
}

} // End namespace vbtracker
} // End namespace osvr
