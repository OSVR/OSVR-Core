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

    Led::Led(LedIdentifier *identifier, LedMeasurement const &meas)
        : m_id(SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA),
          m_identifier(identifier) {
        /// Doesn't matter what the blobs keep ID pref is here, because this is
        /// a new blob so there's no ID to keep.
        addMeasurement(meas, false);
    }

    void Led::addMeasurement(LedMeasurement const &meas, bool blobsKeepId) {
        m_latestMeasurement = meas;
        m_brightnessHistory.push_back(meas.brightness);

        // If we don't have an identifier, then our ID is unknown.
        // Otherwise, try and find it.
        if (!m_identifier) {
            m_id = ZeroBasedBeaconId(
                SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA);
        } else {
            auto oldId = m_id;
            m_id = m_identifier->getId(m_id, m_brightnessHistory, m_lastBright,
                                       blobsKeepId);
#if 0
            m_newlyRecognized = oldId < 0 && m_id >= 0;
            auto lostRecognition = m_id < 0 && oldId >= 0;
            if (!m_newlyRecognized && !lostRecognition && oldId != m_id) {
                std::cout << "Identity theft detected!" << std::endl;
            }
#endif

            /// @todo it seems like LEDs are re-recognized every frame? so
            /// presumably it's possible that oldId != m_id without one being a
            /// sentinel.

            /// Right now, any change in ID is considered being "newly
            /// recognized".
            if (oldId != m_id) {
                /// If newly recognized, start at max novelty
                m_novelty = MAX_NOVELTY;
            } else if (m_novelty != 0) {
                /// Novelty decays linearly to 0
                m_novelty--;
            }
        }
    }

    KeyPointIterator Led::nearest(KeyPointList &keypoints,
                                  double threshold) const {
        // If we have no elements in the vector, return the end().
        if (keypoints.empty()) {
            return end(keypoints);
        }

        // Squaring the threshold to avoid doing a square-root in a tight loop.
        auto thresholdSquared = threshold * threshold;
        auto location = getLocation();

        auto computeDistSquared = [location](KeyPointIterator it) {
            auto diff = (location - it->pt);
            return diff.dot(diff);
        };

        // Find the distance to the first point and record it as the
        // current minimum distance;
        auto ret = begin(keypoints);
        auto minDistSq = computeDistSquared(ret);

        // Search the rest of the elements to see if we can find a
        // better one.
        for (auto it = begin(keypoints), e = end(keypoints); it != e; ++it) {
            auto distSq = computeDistSquared(it);
            if (distSq < minDistSq) {
                minDistSq = distSq;
                ret = it;
            }
        }

        // If the closest is within the threshold, return it.  Otherwise,
        // return the end.
        if (minDistSq <= thresholdSquared) {
            return ret;
        }
        return end(keypoints);
    }

    LedMeasurementVecIterator Led::nearest(LedMeasurementVec &meas,
                                           double threshold) const {
        // If we have no elements in the vector, return the end().
        if (meas.empty()) {
            return end(meas);
        }

        // Squaring the threshold to avoid doing a square-root in a tight loop.
        auto thresholdSquared = threshold * threshold;
        auto location = getLocation();

        auto computeDistSquared = [location](LedMeasurementVecIterator it) {
            auto diff = (location - it->loc);
            return diff.dot(diff);
        };

        // Find the distance to the first point and record it as the
        // current minimum distance;
        auto ret = begin(meas);
        auto minDistSq = computeDistSquared(ret);

        // Search the rest of the elements to see if we can find a
        // better one.
        for (auto it = begin(meas), e = end(meas); it != e; ++it) {
            auto distSq = computeDistSquared(it);
            if (distSq < minDistSq) {
                minDistSq = distSq;
                ret = it;
            }
        }

        // If the closest is within the threshold, return it.  Otherwise,
        // return the end.
        if (minDistSq <= thresholdSquared) {
            return ret;
        }
        return end(meas);
    }

    void Led::markMisidentified() {
        m_id = ZeroBasedBeaconId(
            SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA);
        if (!m_brightnessHistory.empty()) {
            m_brightnessHistory.clear();
            m_brightnessHistory.push_back(getMeasurement().brightness);
        }
    }

} // End namespace vbtracker
} // End namespace osvr
