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
#include "RoomCalibration.h"

// Library/third-party includes
#include <boost/assert.hpp>

#include <util/Stride.h>

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
        auto newId = BodyId(static_cast<BodyId::wrapped_type>(m_bodies.size()));
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
        m_impl->lastFrame = imageData->tv;

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
    inline void
    validateTargetPointerFromUpdateList(TrackedBodyTarget *targetPtr) {

        BOOST_ASSERT_MSG(targetPtr != nullptr, "We should never be "
                                               "retrieving a nullptr for a "
                                               "target with measurements!");
        if (!targetPtr) {
            throw std::logic_error("Logical impossibility: Couldn't "
                                   "retrieve a valid pointer for a target "
                                   "that we were just told updated its "
                                   "LEDs from data this frame.");
        }
    }
    void TrackingSystem::updatePoseEstimates() {
        if (!isRoomCalibrationComplete()) {
            /// If we need calibration, we need calibration. Go get it done.
            calibrationVideoPhaseThree();
            return;
        }

        auto const &updateCount = m_impl->updateCount;
        for (auto &bodyTargetWithMeasurements : updateCount) {
            auto targetPtr = getTarget(bodyTargetWithMeasurements.first);
            validateTargetPointerFromUpdateList(targetPtr);
            auto &target = *targetPtr;

            /// @todo right now assumes one target per body here!
            auto &body = target.getBody();
            util::time::TimeValue stateTime = {};
            BodyState state;
            auto newTime = m_impl->lastFrame;
            auto validState =
                body.getStateAtOrBefore(newTime, stateTime, state);
            auto initialTime = stateTime;

            auto gotPose = target.updatePoseEstimateFromLeds(
                m_impl->camParams, newTime, state, stateTime, validState);
            if (gotPose) {
                body.replaceStateSnapshot(initialTime, newTime, state);
#if 0
                static auto s = ::util::Stride{101};
                if (++s) {
                    std::cout << body.getState().position().transpose() << "\n";
                }
#endif
                /// @todo deduplicate in making this list.
                m_updated.push_back(body.getId());
            }
        }
        /// Prune history after video update.
        for (auto &body : m_bodies) {
            body->pruneHistory();
        }
    }

    void TrackingSystem::calibrationVideoPhaseThree() {
        auto const &updateCount = m_impl->updateCount;
        for (auto &bodyTargetWithMeasurements : updateCount) {
            auto &bodyTargetId = bodyTargetWithMeasurements.first;
            if (!m_impl->calib.wantVideoData(*this, bodyTargetId)) {
                // If the room calibrator doesn't want video tracking data from
                // this target, then move along.
                continue;
            }
            auto targetPtr = getTarget(bodyTargetId);
            validateTargetPointerFromUpdateList(targetPtr);
            auto &target = *targetPtr;
            Eigen::Vector3d xlate;
            Eigen::Quaterniond quat;
            auto gotPose = target.uncalibratedRANSACPoseEstimateFromLeds(
                m_impl->camParams, xlate, quat);
            if (gotPose) {
                m_impl->calib.processVideoData(*this, bodyTargetId,
                                               m_impl->lastFrame, xlate, quat);
            }
        }

        m_impl->calib.postCalibrationUpdate(*this);
    }

    void
    TrackingSystem::calibrationHandleIMUData(BodyId id,
                                             util::time::TimeValue const &tv,
                                             Eigen::Quaterniond const &quat) {
        m_impl->calib.processIMUData(*this, id, tv, quat);
        m_impl->calib.postCalibrationUpdate(*this);
    }

    void TrackingSystem::setCameraPose(Eigen::Isometry3d const &camPose) {
        m_impl->haveCameraPose = true;
        m_impl->cameraPose = camPose;
        m_impl->cameraPoseInv = camPose.inverse();
    }

    bool TrackingSystem::haveCameraPose() const {
        return m_impl->haveCameraPose;
    }

    Eigen::Isometry3d const &TrackingSystem::getCameraPose() const {
        return m_impl->cameraPose;
    }

    Eigen::Isometry3d const &TrackingSystem::getRoomToCamera() const {
        return m_impl->cameraPoseInv;
    }

    bool TrackingSystem::isRoomCalibrationComplete() {
        /// @todo should just be able to do this by checking the state of the
        /// calibrator.

        /// If this is true, we know it's true. If it's false, we must go check.
        if (m_impl->roomCalibCompleteCached) {
            return true;
        }
        /// Update the cached value.
        m_impl->roomCalibCompleteCached =
            vbtracker::isRoomCalibrationComplete(*this);
        return m_impl->roomCalibCompleteCached;
    }

} // namespace vbtracker
} // namespace osvr
