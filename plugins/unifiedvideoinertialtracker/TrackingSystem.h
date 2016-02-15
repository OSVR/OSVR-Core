/** @file
    @brief Header

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

#ifndef INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50
#define INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50

// Internal Includes
#include "ConfigParams.h"
#include "ImageProcessing.h"
#include "BodyIdTypes.h"
#include "CameraParameters.h"

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/TypeSafeIdHash.h>
#include <opencv2/core/core.hpp>

// Standard includes
#include <vector>
#include <memory>
#include <cstddef>
#include <unordered_map>

namespace osvr {
namespace vbtracker {
    class TrackedBody;
    class TrackedBodyTarget;
    using BodyIndices = std::vector<BodyId>;

    using LedUpdateCount = std::unordered_map<BodyTargetId, std::size_t>;

    class TrackingSystem {
      public:
        /// @name Setup and Teardown
        /// @{
        TrackingSystem(ConfigParams const &params);
        ~TrackingSystem();
        TrackedBody *createTrackedBody();
        /// @}

        /// @name Runtime methods
        /// @{
        /// Perform the initial phase of image processing. This does not modify
        /// the bodies, so it can happen in parallel/background processing. It's
        /// also the most expensive, so that's handy.
        ImageOutputDataPtr performInitialImageProcessing(
            util::time::TimeValue const &tv, cv::Mat const &frame,
            cv::Mat const &frameGray, CameraParameters const &camParams);
        /// This is the second phase of the video-based tracking algorithm - the
        /// part that actually changes LED state.
        ///
        /// @param imageData Output from the first step - **please std::move()
        /// the output of the first step into this step.**
        ///
        /// If you don't require updated poses, you can stop after this step,
        /// not proceeding to the third and final phase, and still keep track of
        /// which beacons are which.
        ///
        /// @return a reference to an internal map of body IDs to counts of
        /// used LED measurements for debugging.
        LedUpdateCount const &
        updateLedsFromVideoData(ImageOutputDataPtr &&imageData);

        /// The combined second and third phases of the video-based tracking
        /// algorithm. The third phase uses the updated LED data stored in each
        /// target to arrive at updated pose estimates.
        ///
        /// These two phases are combined in one call to ensure preconditions -
        /// it would be invalid to call the third phase without immediately
        /// previously calling this second phase, but no state handover needs to
        /// take place.
        ///
        /// @param imageData Output from the first step - **please std::move()
        /// the output of the first step into this step.**
        ///
        /// @return A reference to a vector of body indices that were updated
        /// with this latest frame.
        BodyIndices const &
        updateBodiesFromVideoData(ImageOutputDataPtr &&imageData);

        /// All parts of the tracking algorithm combined for convenience.
        ///
        /// @return A reference to a vector of body indices that were
        /// updated with this latest frame.
        BodyIndices const &processFrame(util::time::TimeValue const &tv,
                                        cv::Mat const &frame,
                                        cv::Mat const &frameGray,
                                        CameraParameters const &camParams) {
            auto imageOutput =
                performInitialImageProcessing(tv, frame, frameGray, camParams);
            return updateBodiesFromVideoData(std::move(imageOutput));
        }
        /// @}

        /// @name Accessors
        /// @{
        std::size_t getNumBodies() const { return m_bodies.size(); }
        TrackedBody &getBody(BodyId i) { return *m_bodies.at(i.value()); }
        TrackedBody const &getBody(BodyId i) const {
            return *m_bodies.at(i.value());
        }
        TrackedBodyTarget *getTarget(BodyTargetId target);
        /// @}

        /// @todo refactor;
        ConfigParams const &getParams() const { return m_params; }

        bool haveCameraPose() const;
        void setCameraPose(Eigen::Isometry3d const &camPose);

        /// This gets rTc - the pose of the camera in the room.
        Eigen::Isometry3d const &getCameraPose() const;
        /// This gets cTr - the inverse of the camera pose, transforms from the
        /// room coordinate system to the camera coordinate system.
        Eigen::Isometry3d const &getRoomToCamera() const;

        bool isRoomCalibrationComplete();

        /// private impl;
        struct Impl;

        /// Called by TrackedBody::incorporateNewMeasurementFromIMU() if room
        /// calibration is not complete.
        void calibrationHandleIMUData(BodyId id,
                                      util::time::TimeValue const &tv,
                                      Eigen::Quaterniond const &quat);

      private:
        /// The third phase of the tracking algorithm - LEDs have been updated
        /// with new measurements, we just need to estimate poses.
        void updatePoseEstimates();

        /// Alternate internals called by updatePoseEstimates() when room
        /// calibration is incomplete.
        void calibrationVideoPhaseThree();

        using BodyPtr = std::unique_ptr<TrackedBody>;
        ConfigParams m_params;

        BodyIndices m_updated;
        std::vector<BodyPtr> m_bodies;

        std::unique_ptr<Impl> m_impl;

        friend class TrackingDebugDisplay;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50
