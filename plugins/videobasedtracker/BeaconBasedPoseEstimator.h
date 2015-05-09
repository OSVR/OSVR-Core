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
extern const std::vector< std::vector<double> > OsvrHdkLedLocations_DEFAULT;

// Fake 3D locations for the beacons on an OSVR HDK, in millimeters.
// These are the ones used to construct the simulated video for debugging.
extern const std::vector< std::vector<double> > OsvrHdkLedLocations_FAKE;

// Class to track an object that has identified LED beacons on it as seen in
// a camera, where the absolute location of the LEDs with respect to a common
// frame of reference is known.  Returns the transformation that takes points
// from the model coordinate system to the camera coordinate system.
class BeaconBasedPoseEstimator {
public:
    // Constructor needs to be told the 3D locations of the beacons on the
    // object that is to be tracked.  These define the model coordinate system.
    // It is also told the camera matrix and distortion coefficients, in a
    // format suitable to send to OpenCV.
    // See http://docs.opencv.org/doc/tutorials/calib3d/camera_calibration/camera_calibration.html
    // for details on these formats.
    BeaconBasedPoseEstimator(
        const std::vector < std::vector<double> > &cameraMatrix  //< 3x3 camera matrix for OpenCV
        , const std::vector<double> &distCoeffs             //< Distortion coefficients for OpenCV
        , const std::vector< std::vector<double> > &beacons = OsvrHdkLedLocations_DEFAULT   //< 3D beacon locations
        );

    // Produce an estimate of the pose of the model-space origin in camera space, where the
    // origin is at the center of the image as described by the camera matrix.
    // This pose will be expressed in meters even though the beacon locations
    // and camera focal depth are in millimeters.
    // Returns true on success, false on failure to make a pose.
    bool EstimatePoseFromLeds(
        const std::list<osvr::vbtracker::Led> &leds
        , OSVR_PoseState &out
        , cv::Mat &rvec             //< Also returns the OpenCV results for debugging
        , cv::Mat &tvec             //< Also returns the OpenCV results for debugging
        );

    // Replace one of the data sets we're using with a new one.
    void setBeacons(const std::vector< std::vector<double> > &beacons);
    void setCameraMatrix(const std::vector< std::vector<double> > &cameraMatrix);
    void setDistCoeffs(const std::vector<double> &distCoeffs);

protected:
    std::vector< std::vector<double> >   m_beacons;      //< 3D location of LED beacons
    std::vector< std::vector<double> >   m_cameraMatrix; //< 3x3 camera matrix
    std::vector<double>                  m_distCoeffs;   //< Distortion coefficients
};

} // End namespace vbtracker
} // End namespace osvr
