/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "TrackingSystem.h"
#include "TrackedBody.h"
#include "TrackedBodyTarget.h"
#include "UndistortMeasurements.h"
#include "ForEachTracked.h"
#include "TrackingSystem_Impl.h"
#include "SBDBlobExtractor.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <algorithm>
#include <iterator>
#include <iostream>
#include <string>
#include <stdexcept>

namespace osvr {
namespace vbtracker {

    TrackingSystem::TrackingSystem(ConfigParams const &params)
        : m_params(params), m_impl(new Impl(params)) {}

    TrackingSystem::~TrackingSystem() {}

    TrackedBody *TrackingSystem::createTrackedBody() {
        auto newId = BodyId(m_bodies.size());
        BodyPtr newBody(new TrackedBody(*this, newId));
        m_bodies.emplace_back(std::move(newBody));
        return m_bodies.back().get();
    }

    TrackedBodyTarget *TrackingSystem::getTarget(BodyTargetId target) {
        return getBody(target.first).getTarget(target.second);
    }

    ImageOutputDataPtr TrackingSystem::performInitialImageProcessing(
        util::time::TimeValue const &tv, cv::Mat const &frame,
        cv::Mat const &frameGray, CameraParameters const &camParams) {

        ImageOutputDataPtr ret(new ImageProcessingOutput);
        ret->tv = tv;
        ret->frame = frame;
        ret->frameGray = frameGray;
        ret->camParams = camParams.createUndistortedVariant();
        auto rawMeasurements =
            m_impl->blobExtractor->extractBlobs(ret->frameGray);
        ret->ledMeasurements = undistortLeds(rawMeasurements, camParams);
        return ret;
    }

    LedUpdateCount const &
    TrackingSystem::updateLedsFromVideoData(ImageOutputDataPtr &&imageData) {
        /// Clear internal data, we're invalidating things here.
        m_updated.clear();
        auto &updateCount = m_impl->updateCount;
        updateCount.clear();

        /// Update our frame cache, since we're taking ownership of the image
        /// data now.
        m_impl->frame = imageData->frame;
        m_impl->frameGray = imageData->frameGray;
        m_impl->camParams = imageData->camParams;

        /// Go through each target and try to process the measurements.
        forEachTarget(*this, [&](TrackedBodyTarget &target) {
            auto usedMeasurements =
                target.processLedMeasurements(imageData->ledMeasurements);
            if (usedMeasurements != 0) {
                updateCount[target.getQualifiedId()] = usedMeasurements;
            }
        });
        return updateCount;
    }

    BodyIndices const &
    TrackingSystem::updateBodiesFromVideoData(ImageOutputDataPtr &&imageData) {
        /// Do the second phase of stuff
        updateLedsFromVideoData(std::move(imageData));

        /// Do the third phase of tracking.
        updatePoseEstimates();

        /// Trigger debug display, if activated.
        m_impl->triggerDebugDisplay(*this);

        return m_updated;
    }

    void TrackingSystem::updatePoseEstimates() {

        auto const &updateCount = m_impl->updateCount;
        for (auto &bodyTargetWithMeasurements : updateCount) {
            auto targetPtr = getTarget(bodyTargetWithMeasurements.first);
            BOOST_ASSERT_MSG(targetPtr != nullptr, "We should never be "
                                                   "retrieving a nullptr for a "
                                                   "target with measurements!");
            if (!targetPtr) {
                throw std::logic_error("Logical impossibility: Couldn't "
                                       "retrieve a valid pointer for a target "
                                       "that we were just told updated its "
                                       "LEDs from data this frame.");
            }
            auto &target = *targetPtr;

            /// @todo right now assumes one target per body here!
            auto &body = target.getBody();
            util::time::TimeValue stateTime;
            BodyState state;
            auto newTime = m_impl->lastFrame;
            auto validState =
                body.getStateAtOrBefore(newTime, stateTime, state);
            auto gotPose = target.updatePoseEstimateFromLeds(
                m_impl->camParams, newTime, state, stateTime, validState);
            if (gotPose) {
                body.replaceStateSnapshot(stateTime, newTime, state);
                /// @todo deduplicate in making this list.
                m_updated.push_back(body.getId());
            }
        }
    }

    bool TrackedBody::hasPoseEstimate() const {
        /// @todo handle IMU here.
        auto ret = false;
        forEachTarget([&ret](TrackedBodyTarget &target) {
            ret = ret || target.hasPoseEstimate();
        });
        return ret;
    }

} // namespace vbtracker
} // namespace osvr
