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

#ifndef INCLUDED_TrackedBodyTarget_h_GUID_E4315530_5C4F_4DB4_2497_11686F0F6E0E
#define INCLUDED_TrackedBodyTarget_h_GUID_E4315530_5C4F_4DB4_2497_11686F0F6E0E

// Internal Includes
#include "Types.h"
#include "LedMeasurement.h"
#include "BeaconIdTypes.h"
#include "BodyIdTypes.h"
#include "BeaconSetupData.h"
#include "ModelTypes.h"

// Library/third-party includes
#include <osvr/Kalman/PureVectorState.h>
#include <osvr/Util/TimeValue.h>
#include <boost/assert.hpp>

// Standard includes
#include <vector>
#include <iosfwd>

namespace osvr {
namespace vbtracker {
    struct CameraParameters;

    /// @todo refactor? ported directly
    struct BeaconData {
        bool seen = false;
        double size = 0;
        cv::Point2d measurement = {0, 0};
        cv::Point2d residual = {0, 0};
        double variance = 0;
        void reset() { *this = BeaconData{}; }
    };

    class TrackedBody;
    struct BodyTargetInterface;
    /// Corresponds to a rigid arrangements of discrete beacons detected by
    /// video-based tracking - typically IR LEDs.
    class TrackedBodyTarget {
      public:
        TrackedBodyTarget(TrackedBody &body,
                          BodyTargetInterface const &bodyIface,
                          Eigen::Vector3d const &targetToBody,
                          TargetSetupData const &setupData, TargetId id);
        ~TrackedBodyTarget();
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        std::vector<BeaconData> const &getBeaconDebugData() const {
            return m_beaconDebugData;
        }

        UnderlyingBeaconIdType getNumBeacons() const {
            return m_beacons.size();
        }

        TrackedBody &getBody() { return m_body; }
        TrackedBody const &getBody() const { return m_body; }

        /// Get the target id within this body.
        TargetId getId() const { return m_id; }

        /// Get a fully-qualified (within a tracking system) id for this target.
        BodyTargetId getQualifiedId() const;

        /// Used to extract autocalibration results in a standalone calibration
        /// app, and to show information in a debug window.
        Eigen::Vector3d getBeaconAutocalibPosition(ZeroBasedBeaconId i) const;
        /// Used to extract autocalibration results in a standalone calibration
        /// app
        Eigen::Vector3d getBeaconAutocalibVariance(ZeroBasedBeaconId i) const;

        /// Called each frame with the results of the blob finding and
        /// undistortion (part of the first phase of the tracking system)
        ///
        /// @return number of LED measurements/blobs used locally on existing
        /// LEDs.
        std::size_t
        processLedMeasurements(LedMeasurementVec const &undistortedLeds);

        /// Update the pose estimate using the updated LEDs - part of the third
        /// phase of tracking.
        bool updatePoseEstimateFromLeds(
            CameraParameters const &camParams,
            osvr::util::time::TimeValue const &tv, BodyState &bodyState,
            osvr::util::time::TimeValue const &startingTime,
            bool validStateAndTime);

        /// Perform a simple RANSAC pose estimation from updated LEDs (third
        /// phase of tracking) without storing the results internally or
        /// changing internal state, or using any internal calibration
        /// transforms. Intended for use during initial room calibration
        /// (startup).
        ///
        /// @param camParams Camera parameters for the image source (no
        /// distortion)
        /// @param [out] xlate Pose estimate: translation in meters. Only
        /// modified if return value is true.
        /// @param [out] quat Pose estimate: rotation. Only modified if return
        /// value is true.
        /// @return true if a pose was estimated and the out parameters were
        /// modified.
        bool uncalibratedRANSACPoseEstimateFromLeds(
            CameraParameters const &camParams, Eigen::Vector3d &xlate,
            Eigen::Quaterniond &quat);

        /// Did this target yet, or last time it was asked to, compute a
        /// pose estimate?
        bool hasPoseEstimate() const { return m_hasPoseEstimate; }

        osvr::util::time::TimeValue const &getLastUpdate() const;

        /// Get the offset that was subtracted from all beacon positions upon
        /// initialization.
        Eigen::Vector3d const &getBeaconOffset() const {
            return m_beaconOffset;
        }

        /// Get all beacons/leds, including unrecognized ones
        LedGroup const &leds() const;

        /// Get a list of pointers to all recognized, in-range beacons/leds
        LedPtrList const &usableLeds() const;

      private:
        /// Get the beacon offset transformed into world space
        Eigen::Vector3d getStateCorrection() const;

        std::ostream &msg() const;
        void enterKalmanMode();
        void enterRANSACMode();

        void dumpBeaconsToConsole() const;

        LedGroup &leds();

        /// Update usableLeds() from leds()
        void updateUsableLeds();

        LedPtrList &usableLeds();

        ConfigParams const &getParams() const;
        void m_verifyInvariants() const {
            BOOST_ASSERT_MSG(m_beacons.size() ==
                                 m_beaconMeasurementVariance.size(),
                             "Must have the same number of beacons as default "
                             "measurement variances.");
            BOOST_ASSERT_MSG(
                m_beacons.size() == m_beaconFixed.size(),
                "Must have the same number of beacons as beacon fixed flags.");
            BOOST_ASSERT_MSG(m_beacons.size() ==
                                 m_beaconEmissionDirection.size(),
                             "Must have the same number of beacons as beacon "
                             "emission directions.");
        }
        using BeaconState = kalman::PureVectorState<3>;
        using BeaconStateVec = std::vector<std::unique_ptr<BeaconState>>;

        /// Reference to our parent body.
        TrackedBody &m_body;

        /// Our target id relative to our parent body.
        const TargetId m_id;

        /// Transformation from target coordinate frame to body coordinate
        /// frame.
        Eigen::Vector3d m_targetToBody;

        /// @name Parallel vectors, one entry per beacon
        /// @brief All should have the same number of values at all times.
        /// @{
        /// Vector of beacon states (with errors - for autocalibration)
        BeaconStateVec m_beacons;
        std::vector<double> m_beaconMeasurementVariance;
        /// Should this beacon be "fixed" (no auto-calibration?)
        std::vector<bool> m_beaconFixed;
        Vec3Vector m_beaconEmissionDirection;
        ///@}
        /// @todo will this always have the same number of entries?
        std::vector<BeaconData> m_beaconDebugData;

        Eigen::Vector3d m_beaconOffset;

        bool m_hasPoseEstimate = false;

        /// private implementation
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_TrackedBodyTarget_h_GUID_E4315530_5C4F_4DB4_2497_11686F0F6E0E
