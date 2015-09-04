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
#include <osvr/Util/ClientReportTypesC.h>

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

        /// @brief Produce an estimate of the pose of the model-space origin in
        /// camera space, where the origin is at the center of the image as
        /// described by the camera matrix.
        ///
        /// This pose will be expressed in meters even though the beacon
        /// locations and camera focal depth are in millimeters.
        ///
        /// @return Returns true on success, false on failure to make a pose.
        bool EstimatePoseFromLeds(const LedGroup &leds, OSVR_PoseState &out);

        /// @brief Project the beacons into image space given the most-recent
        /// estimation of pose.
        /// @return true on success, false on failure.
        bool ProjectBeaconsToImage(std::vector<cv::Point2f> &outPose);

        /// @name Data set resets
        /// @brief Replace one of the data sets we're using with a new one.
        /// @{
        bool SetBeacons(const Point3Vector &beacons);
        bool SetCameraMatrix(const DoubleVecVec &cameraMatrix);
        bool SetDistCoeffs(const std::vector<double> &distCoeffs);
        /// @}
      private:
        /// @brief Implementation - doesn't set m_gotPose;
        bool m_estimatePoseFromLeds(const LedGroup &leds, OSVR_PoseState &out);

        Point3Vector m_beacons;     //< 3D location of LED beacons
        cv::Mat m_cameraMatrix;     //< 3x3 camera matrix
        cv::Mat m_distCoeffs;       //< Distortion coefficients
        size_t m_requiredInliers;   //< How many inliers do we require?
        size_t m_permittedOutliers; //< How many outliers do we allow?

        /// @name Pose cache
        /// @brief Stores the most-recent solution, in case we need it again
        /// (for example, to project our beacons into the image).
        /// @{
        /// @brief Have we produced a pose estimate yet?
        bool m_gotPose;
        /// @brief Rotation vector associated with the most-recent pose
        cv::Mat m_rvec;
        /// @brief Translation vector associated with the most-recent pose.
        cv::Mat m_tvec;
        /// @}
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BeaconBasedPoseEstimator_h_GUID_7B983CED_F2C5_4B86_109A_948863B665B1
