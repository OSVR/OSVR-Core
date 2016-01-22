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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_BeaconBasedPoseEstimator_h_GUID_7B983CED_F2C5_4B86_109A_948863B665B1
#define INCLUDED_BeaconBasedPoseEstimator_h_GUID_7B983CED_F2C5_4B86_109A_948863B665B1

// Internal Includes
#include "Types.h"
#include "LED.h"
#include "CameraParameters.h"

// Library/third-party includes
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Kalman/PureVectorState.h>
#include <osvr/Kalman/PoseState.h>
#include <osvr/Kalman/PoseSeparatelyDampedConstantVelocity.h>

// Standard includes
#include <vector>
#include <list>
#include <memory>
#include <iosfwd>

namespace osvr {
namespace vbtracker {

    struct BeaconData {
        bool seen = false;
        double size = 0;
        cv::Point2d measurement = {0, 0};
        cv::Point2d residual = {0, 0};
        double variance = 0;
        void reset() { *this = BeaconData{}; }
    };

    /// @brief Class to track an object that has identified LED beacons
    /// on it as seen in a camera, where the absolute location of the
    /// LEDs with respect to a common frame of reference is known.
    /// Returns the transformation that takes points from the model
    /// coordinate system to the camera coordinate system.
    class BeaconBasedPoseEstimator {
      public:
        static BeaconIDPredicate getDefaultBeaconFixedPredicate() {
            return [](int id) { return id <= 4; };
        }

        /// @brief Constructor that expects its beacons to be set later.
        /// It is told the camera matrix and distortion coefficients, in a
        /// format suitable to send to OpenCV. See
        /// http://docs.opencv.org/doc/tutorials/calib3d/camera_calibration/camera_calibration.html
        /// for details on these formats.
        /// @param camParams Intrinsic camera parameters (camera matrix and
        /// distortion)
        /// @param beacons 3D beacon locations
        /// @param requiredInliers How many "good" points must be available
        /// @param permittedOutliers How many additional "bad" points we can
        /// have
        BeaconBasedPoseEstimator(CameraParameters const &camParams,
                                 size_t requiredInliers = 4,
                                 size_t permittedOutliers = 2,
                                 ConfigParams const &params = ConfigParams{});
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        /// @brief Produce an estimate of the pose of the model-space origin in
        /// camera space, where the origin is at the center of the image as
        /// described by the camera matrix.
        ///
        /// This pose will be expressed in meters even though the beacon
        /// locations and camera focal depth are in millimeters.
        ///
        /// @return Returns true on success, false on failure to make a pose.
        bool EstimatePoseFromLeds(LedGroup &leds, OSVR_TimeValue const &tv,
                                  OSVR_PoseState &out);

        std::size_t getNumBeacons() const { return m_beacons.size(); }

        /// @brief Project the beacons into image space given the most-recent
        /// estimation of pose.
        /// @return true on success, false on failure.
        bool ProjectBeaconsToImage(std::vector<cv::Point2f> &outPose);

        /// Some uses of this may require explicitly disabling kalman mode until
        /// a condition is met. This permits that.
        void permitKalmanMode(bool permitKalman);

        /// @name State getting methods
        /// @brief They extract state in the OSVR units (meters, not mm, for
        /// instance) even when the internal storage may vary.
        /// @{
        OSVR_PoseState GetState() const;
        OSVR_PoseState GetPredictedState(double dt) const;
        Eigen::Vector3d GetLinearVelocity() const;
        Eigen::Vector3d GetAngularVelocity() const;
        /// @}

        /// @name Data set resets
        /// @brief Replace one of the data sets we're using with a new one.
        /// @{
        bool SetBeacons(const Point3Vector &beacons,
                        Vec3Vector const &emissionDirection,
                        std::vector<double> const &variance,
                        BeaconIDPredicate const &autocalibrationFixedPredicate,
                        double beaconAutocalibErrorScale = 1);
        bool SetCameraParameters(CameraParameters const &camParams);
        /// @}

        void dumpBeaconLocationsToStream(std::ostream &os) const;

        std::vector<BeaconData> const &getBeaconDebugData() const {
            return m_beaconDebugData;
        }

        Eigen::Vector3d getBeaconAutocalibPosition(std::size_t i) const;

        Eigen::Vector3d getBeaconAutocalibVariance(std::size_t i) const;

      private:
        void m_updateBeaconCentroid(const Point3Vector &beacons);
        void m_updateBeaconDebugInfoArray();
        /// @brief Internal position differs in scale and origin from external.
        /// This function deals with that for you.
        Eigen::Vector3d m_convertInternalPositionRepToExternal(
            Eigen::Vector3d const &pos) const;

        /// @brief Implementation - doesn't set m_gotPose;
        bool m_estimatePoseFromLeds(LedGroup &leds, OSVR_TimeValue const &tv,
                                    OSVR_PoseState &out);

        /// @brief The internals of m_estimatePoseFromLeds that use
        /// cv::computePnPRansac to compute an estimate.
        bool m_pnpransacEstimator(LedGroup &leds);

        /// @brief The internals of m_estimatePoseFromLeds that use a Kalman
        /// filter with beacon position auto-calibration to compute an estimate.
        bool m_kalmanAutocalibEstimator(LedGroup &leds, double dt);

        /// @brief A method that determines if the Kalman filter has gotten
        /// itself into a bad situation and we should start again with RANSAC.
        ///
        /// Note that this clears the values it checks, since it resets a value
        /// that will cause the estimator to use RANSAC for the next frame
        /// dispatched.
        bool m_forceRansacIfKalmanNeedsReset(LedGroup const &leds);

        /// @brief Resets the Kalman filter main state based on the
        /// direct-calculation outputs.
        void m_resetState(Eigen::Vector3d const &xlate,
                          Eigen::Quaterniond const &quat);
        using BeaconState = kalman::PureVectorState<3>;
        using BeaconStateVec = std::vector<std::unique_ptr<BeaconState>>;
        BeaconStateVec m_beacons;
        std::vector<double> m_beaconMeasurementVariance;
        /// Should this beacon be "fixed" (no auto-calibration?)
        std::vector<bool> m_beaconFixed;
        Vec3Vector m_beaconEmissionDirection;

        std::vector<BeaconData> m_beaconDebugData;

        CameraParameters m_camParams;
        size_t m_requiredInliers;   //< How many inliers do we require?
        size_t m_permittedOutliers; //< How many outliers do we allow?

        ConfigParams const m_params;

        /// Sensor centroid, subtracted out of the beacon coordinates when
        /// initially set. May be user-configured in which case it may not be
        /// the actual centroid, but servies the same purpose.
        Eigen::Vector3d m_centroid;

        /// Timestamp of previous frame
        OSVR_TimeValue m_prev;
        /// whether m_prev is a valid timestamp
        bool m_gotPrev = false;

        bool m_permitKalman = true;

        /// @name Pose cache
        /// @brief Stores the most-recent solution, in case we need it again
        /// (for example, to project our beacons into the image).
        /// @{
        /// @brief Did we produce a pose estimate last cycle?
        bool m_gotPose = false;
        /// @brief Did our last cycle process any measurements?
        bool m_gotMeasurement = false;
        /// @brief Rotation vector associated with the most-recent pose
        cv::Mat m_rvec;
        /// @brief Translation vector associated with the most-recent pose.
        cv::Mat m_tvec;
        using State = kalman::pose_externalized_rotation::State;
        State m_state;
        using ProcessModel =
            osvr::kalman::PoseSeparatelyDampedConstantVelocityProcessModel;
        ProcessModel m_model;
        /// @}

        /// @name Kalman startup status
        /// @{
        /// How long we've been turning in low ratios of good to bad residuals.
        std::size_t m_framesInProbation = 0;
        /// How long we've had what might have been valid measurements but
        /// excluded all of them.
        std::size_t m_framesWithoutUtilizedMeasurements = 0;

        std::size_t m_framesWithoutIdentifiedBlobs = 0;
        /// @}
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BeaconBasedPoseEstimator_h_GUID_7B983CED_F2C5_4B86_109A_948863B665B1
