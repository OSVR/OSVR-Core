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

// Library/third-party includes
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/ClientReportTypesC.h>
#include <osvr/Kalman/PureVectorState.h>
#include <osvr/Kalman/PoseState.h>
#include <osvr/Kalman/PoseDampedConstantVelocity.h>

// Standard includes
#include <vector>
#include <list>
#include <memory>

namespace osvr {
namespace vbtracker {

    /// @name Default 3D locations for the beacons on an OSVR HDK, in
    /// millimeters
    /// @{
    extern const Point3Vector OsvrHdkLedLocations_SENSOR0;
    extern const Point3Vector OsvrHdkLedLocations_SENSOR1;
    /// @}

    extern const std::vector<double> OsvrHdkLedVariances_SENSOR0;

    /// @brief Class to track an object that has identified LED beacons
    /// on it as seen in a camera, where the absolute location of the
    /// LEDs with respect to a common frame of reference is known.
    /// Returns the transformation that takes points from the model
    /// coordinate system to the camera coordinate system.
    class BeaconBasedPoseEstimator {
      public:
        /// @brief Constructor needs to be told the 3D locations of the beacons
        /// on the object that is to be tracked.  These define the model
        /// coordinate system.
        /// It is also told the camera matrix and distortion coefficients, in a
        /// format suitable to send to OpenCV. See
        /// http://docs.opencv.org/doc/tutorials/calib3d/camera_calibration/camera_calibration.html
        /// for details on these formats.
        /// @param cameraMatrix 3x3 camera matrix for OpenCV
        /// @param distCoeffs Distortion coefficients for OpenCV
        /// @param beacons 3D beacon locations
        /// @param requiredInliers How many "good" points must be available
        /// @param permittedOutliers How many additional "bad" points we can
        /// have
        BeaconBasedPoseEstimator(const DoubleVecVec &cameraMatrix,
                                 const std::vector<double> &distCoeffs,
                                 const Point3Vector &beacons,
                                 size_t requiredInliers = 4,
                                 size_t permittedOutliers = 2);
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        /// @brief Produce an estimate of the pose of the model-space origin in
        /// camera space, where the origin is at the center of the image as
        /// described by the camera matrix.
        ///
        /// This pose will be expressed in meters even though the beacon
        /// locations and camera focal depth are in millimeters.
        ///
        /// @return Returns true on success, false on failure to make a pose.
        bool EstimatePoseFromLeds(const LedGroup &leds,
                                  OSVR_TimeValue const &tv,
                                  OSVR_PoseState &out);

        /// @brief Project the beacons into image space given the most-recent
        /// estimation of pose.
        /// @return true on success, false on failure.
        bool ProjectBeaconsToImage(std::vector<cv::Point2f> &outPose);

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
        bool SetBeacons(const Point3Vector &beacons);
        bool SetBeacons(const Point3Vector &beacons, double variance);
        bool SetBeacons(const Point3Vector &beacons, std::vector<double> const& variance);
        bool SetCameraMatrix(const DoubleVecVec &cameraMatrix);
        bool SetDistCoeffs(const std::vector<double> &distCoeffs);
        /// @}
      private:
        void m_updateBeaconCentroid(const Point3Vector &beacons);
        /// @brief Internal position differs in scale and origin from external.
        /// This function deals with that for you.
        Eigen::Vector3d
        m_convertInternalPositionRepToExternal(Eigen::Vector3d const &pos) const;

        /// @brief Implementation - doesn't set m_gotPose;
        bool m_estimatePoseFromLeds(const LedGroup &leds,
                                    OSVR_TimeValue const &tv,
                                    OSVR_PoseState &out);

        /// @brief The internals of m_estimatePoseFromLeds that use
        /// cv::computePnPRansac to compute an estimate.
        bool m_pnpransacEstimator(const LedGroup &leds);

        /// @brief The internals of m_estimatePoseFromLeds that use a Kalman
        /// filter with beacon position auto-calibration to compute an estimate.
        bool m_kalmanAutocalibEstimator(const LedGroup &leds, double dt);

        /// @brief Resets the Kalman filter main state based on the
        /// direct-calculation outputs.
        void m_resetState(Eigen::Vector3d const &xlate,
                          Eigen::Quaterniond const &quat);
        using BeaconState = kalman::PureVectorState<3>;
        using BeaconStateVec = std::vector<std::unique_ptr<BeaconState>>;
        BeaconStateVec m_beacons;
        std::vector<double> m_beaconMeasurementVariance;
        Eigen::Vector2d m_principalPoint;
        double m_focalLength;
        cv::Mat m_cameraMatrix;     //< 3x3 camera matrix
        cv::Mat m_distCoeffs;       //< Distortion coefficients
        size_t m_requiredInliers;   //< How many inliers do we require?
        size_t m_permittedOutliers; //< How many outliers do we allow?

        /// Sensor centroid, subtracted out of the beacon coordinates when
        /// initially set.
        Eigen::Vector3d m_centroid;

        /// Timestamp of previous frame
        OSVR_TimeValue m_prev;
        /// whether m_prev is a valid timestamp
        bool m_gotPrev = false;

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
        using ProcessModel = osvr::kalman::PoseDampedConstantVelocityProcessModel;
        ProcessModel m_model;
        /// @}

        /// @name Kalman startup status
        /// @{
        /// How long we've been turning in low ratios of good to bad residuals.
        std::size_t m_framesInProbation = 0;
        /// @}
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BeaconBasedPoseEstimator_h_GUID_7B983CED_F2C5_4B86_109A_948863B665B1
