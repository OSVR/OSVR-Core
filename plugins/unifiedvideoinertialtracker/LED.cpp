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
        : m_identifier(identifier) {
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
            m_id = ZeroBasedBeaconId(SENTINEL_NO_IDENTIFIER_OBJECT);
        } else {
            auto const oldId = m_id;
            m_id = m_identifier->getId(m_id, m_brightnessHistory, m_lastBright,
                                       blobsKeepId);
			using Id = ZeroBasedBeaconId;
            if (Id(SENTINEL_MARKED_MISIDENTIFIED) == oldId &&
                (Id(
                     SENTINEL_NO_IDENTIFIER_OBJECT_OR_INSUFFICIENT_DATA) ==
                     m_id ||
                 Id(
                     SENTINEL_NO_PATTERN_RECOGNIZED_DESPITE_SUFFICIENT_DATA) ==
                     m_id)) {
                /// Make the "misidentified" sentinel a little stickier than
                /// "insufficient data" or "no pattern recognized" so we can see
                /// it on the debug view.
                m_id = Id(SENTINEL_MARKED_MISIDENTIFIED);
            }

            /// @todo Identify "theft" is possible and takes place - right now
            /// it's handled just the same as any other change in ID.

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

    void Led::markMisidentified() {
        m_id = ZeroBasedBeaconId(SENTINEL_MARKED_MISIDENTIFIED);
        if (!m_brightnessHistory.empty()) {
            m_brightnessHistory.clear();
            m_brightnessHistory.push_back(getMeasurement().brightness);
        }
    }

} // End namespace vbtracker
} // End namespace osvr
