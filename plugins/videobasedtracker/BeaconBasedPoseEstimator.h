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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "LED.h"
#include <osvr/Util/ClientReportTypesC.h>
#include <vector>
#include <list>

namespace osvr {
namespace vbtracker {

    // Default 3D locations for the beacons on an OSVR HDK, in millimeters
    extern const std::vector<std::vector<double> > OsvrHdkLedLocations_SENSOR0;
    extern const std::vector<std::vector<double> > OsvrHdkLedLocations_SENSOR1;

    // Class to track an object that has identified LED beacons on it as seen in
    // a camera, where the absolute location of the LEDs with respect to a
    // common
    // frame of reference is known.  Returns the transformation that takes
    // points
    // from the model coordinate system to the camera coordinate system.
    class BeaconBasedPoseEstimator {
      public:
        // Constructor needs to be told the 3D locations of the beacons on the
        // object that is to be tracked.  These define the model coordinate
        // system.
        // It is also told the camera matrix and distortion coefficients, in a
        // format suitable to send to OpenCV.
        // See
        // http://docs.opencv.org/doc/tutorials/calib3d/camera_calibration/camera_calibration.html
        // for details on these formats.
        BeaconBasedPoseEstimator(
            const std::vector<std::vector<double> > &
                cameraMatrix //< 3x3 camera matrix for OpenCV
            ,
            const std::vector<double> &
                distCoeffs //< Distortion coefficients for OpenCV
            ,
            const std::vector<std::vector<double> > &
                beacons //< 3D beacon locations
            );

        // Produce an estimate of the pose of the model-space origin in camera
        // space, where the
        // origin is at the center of the image as described by the camera
        // matrix.
        // This pose will be expressed in meters even though the beacon
        // locations
        // and camera focal depth are in millimeters.
        // Returns true on success, false on failure to make a pose.
        bool EstimatePoseFromLeds(const std::list<osvr::vbtracker::Led> &leds,
                                  OSVR_PoseState &out);

        // Project the beacons into image space given the most-recent estimation
        // of
        // pose.
        // Returns true on success, false on failure.
        bool ProjectBeaconsToImage(std::vector<cv::Point2f> &outPose);

        // Replace one of the data sets we're using with a new one.
        bool SetBeacons(const std::vector<std::vector<double> > &beacons);
        bool
        SetCameraMatrix(const std::vector<std::vector<double> > &cameraMatrix);
        bool SetDistCoeffs(const std::vector<double> &distCoeffs);

      protected:
        std::vector<cv::Point3f> m_beacons; //< 3D location of LED beacons
        cv::Mat m_cameraMatrix;             //< 3x3 camera matrix
        cv::Mat m_distCoeffs;               //< Distortion coefficients

        // Stores the most-recent solution, in case we need it again (for
        // example, to
        // project our beacons into the image).
        bool m_gotPose; //< Have we produced a pose estimate yet?
        cv::Mat m_rvec; //< Rotation vector associated with the most-recent pose
        cv::Mat
            m_tvec; //< Translation vector associated with the most-recent pose.
    };

} // End namespace vbtracker
} // End namespace osvr
