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

// Internal Includes
#include "BeaconBasedPoseEstimator.h"

// Library/third-party includes
#include <osvr/Util/QuatlibInteropC.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    // clang-format off
    // Default 3D locations for the beacons on an OSVR HDK face plate, in
    // millimeters
    const Point3Vector OsvrHdkLedLocations_SENSOR0 = {
        cv::Point3f( -85, 3, 24.09 ),
        cv::Point3f( -83.2, -14.01, 13.89 ),
        cv::Point3f( -47, 51, 24.09 ),
        cv::Point3f( 47, 51, 24.09 ),
        cv::Point3f( 86.6, 2.65, 24.09 ),
        cv::Point3f( 85.5, -14.31, 13.89 ),
        cv::Point3f( 85.2, 19.68, 13.89 ),
        cv::Point3f( 21, 51, 24.09 ),  // Original spec was 13.09, new position works better
        cv::Point3f( -21, 51, 24.09 ), // Original spec was 13.09, new position works better
        cv::Point3f( -84.2, 19.99, 13.89 ),
        cv::Point3f( -60.41, 47.55, 44.6 ),
        cv::Point3f( -80.42, 20.48, 42.9 ),
        cv::Point3f( -82.01, 2.74, 42.4 ),
        cv::Point3f( -80.42, -14.99, 42.9 ),
        cv::Point3f( -60.41, -10.25, 48.1 ),
        cv::Point3f( -60.41, 15.75, 48.1 ),
        cv::Point3f( -30.41, 32.75, 50.5 ),
        cv::Point3f( -31.41, 47.34, 47 ),
        cv::Point3f( -0.41, -15.25, 51.3 ),
        cv::Point3f( -30.41, -27.25, 50.5 ),
        cv::Point3f( -60.44, -41.65, 45.1 ),
        cv::Point3f( -22.41, -41.65, 47.8 ),
        cv::Point3f( 21.59, -41.65, 47.8 ),
        cv::Point3f( 59.59, -41.65, 45.1 ),
        cv::Point3f( 79.63, -14.98, 42.9 ),
        cv::Point3f( 29.59, -27.25, 50.5 ),
        cv::Point3f( 81.19, 2.74, 42.4 ),
        cv::Point3f( 79.61, 20.48, 42.9 ),
        cv::Point3f( 59.59, 47.55, 44.6 ),
        cv::Point3f( 30.59, 47.55, 47 ),
        cv::Point3f( 29.59, 32.75, 50.5 ),
        cv::Point3f( -0.41, 20.75, 51.3 ),
        cv::Point3f( 59.59, 15.75, 48.1 ),
        cv::Point3f( 59.59, -10.25, 48.1 )
    };

    // Default 3D locations for the beacons on an OSVR HDK back plate, in
    // millimeters
    const Point3Vector OsvrHdkLedLocations_SENSOR1 = {
        cv::Point3f( 1, 23.8, 0 ),
        cv::Point3f( 11, 5.8, 0 ),
        cv::Point3f( 9, -23.8, 0 ),
        cv::Point3f( 0, -8.8, 0 ),
        cv::Point3f( -9, -23.8, 0 ),
        cv::Point3f( -12, 5.8, 0 )
    };
    // clang-format on

    BeaconBasedPoseEstimator::BeaconBasedPoseEstimator(
        const DoubleVecVec &cameraMatrix, const std::vector<double> &distCoeffs,
        const Point3Vector &beacons, size_t requiredInliers,
        size_t permittedOutliers) {
        SetBeacons(beacons);
        SetCameraMatrix(cameraMatrix);
        SetDistCoeffs(distCoeffs);
        m_gotPose = false;
        m_requiredInliers = requiredInliers;
        m_permittedOutliers = permittedOutliers;
    }

    bool BeaconBasedPoseEstimator::SetBeacons(const Point3Vector &beacons) {
        // Our existing pose won't match anymore.
        m_gotPose = false;
        m_beacons = beacons;

        return true;
    }

    bool BeaconBasedPoseEstimator::SetCameraMatrix(
        const DoubleVecVec &cameraMatrix) {
        // Our existing pose won't match anymore.
        m_gotPose = false;

        // Construct the camera matrix from the vectors we received.
        if (cameraMatrix.size() != 3) {
            return false;
        }
        for (size_t i = 0; i < cameraMatrix.size(); i++) {
            if (cameraMatrix[i].size() != 3) {
                return false;
            }
        }
        cv::Mat newCameraMatrix(static_cast<int>(cameraMatrix.size()),
                                static_cast<int>(cameraMatrix[0].size()),
                                CV_64F);
        for (int i = 0; i < cameraMatrix.size(); i++) {
            for (int j = 0; j < cameraMatrix[i].size(); j++) {
                newCameraMatrix.at<double>(i, j) = cameraMatrix[i][j];
            }
        }

        m_cameraMatrix = newCameraMatrix;
        //    std::cout << "XXX cameraMatrix =" << std::endl << m_cameraMatrix
        //    << std::endl;
        return true;
    }

    bool BeaconBasedPoseEstimator::SetDistCoeffs(
        const std::vector<double> &distCoeffs) {
        // Our existing pose won't match anymore.
        m_gotPose = false;

        // Construct the distortion matrix from the vectors we received.
        if (distCoeffs.size() < 5) {
            return false;
        }
        cv::Mat newDistCoeffs(static_cast<int>(distCoeffs.size()), 1, CV_64F);
        for (int i = 0; i < distCoeffs.size(); i++) {
            newDistCoeffs.at<double>(i, 0) = distCoeffs[i];
        }

        m_distCoeffs = newDistCoeffs;
        //    std::cout << "XXX distCoeffs =" << std::endl << m_distCoeffs <<
        //    std::endl;
        return true;
    }

    bool
    BeaconBasedPoseEstimator::EstimatePoseFromLeds(const LedGroup &leds,
                                                   OSVR_PoseState &outPose) {
        auto ret = m_estimatePoseFromLeds(leds, outPose);
        m_gotPose = ret;
        return ret;
    }

    bool
    BeaconBasedPoseEstimator::m_estimatePoseFromLeds(const LedGroup &leds,
                                                     OSVR_PoseState &outPose) {
        // We need to get a pair of matched vectors of points: 2D locations
        // with in the image and 3D locations in model space.  There needs to
        // be a correspondence between the points in these vectors, such that
        // the ith element in one matches the ith element in the other.  We
        // make these by looking up the locations of LEDs with known identifiers
        // and pushing both onto the vectors at the same time.

        std::vector<cv::Point3f> objectPoints;
        std::vector<cv::Point2f> imagePoints;
        auto const beaconsSize = m_beacons.size();
        for (auto const &led : leds) {
            if (!led.identified()) {
                continue;
            }
            auto id = led.getID();
            if (id < beaconsSize) {
                imagePoints.push_back(led.getLocation());
                objectPoints.push_back(m_beacons[id]);
            }
        }

        // Make sure we have enough points to do our estimation.
        if (objectPoints.size() < m_permittedOutliers + m_requiredInliers) {
            return false;
        }

        // Produce an estimate of the translation and rotation needed to take
        // points from model space into camera space.  We allow for at most
        // m_permittedOutliers outliers. Even in simulation data, we sometimes
        // find duplicate IDs for LEDs, indicating that we are getting
        // mis-identified ones sometimes.
        // We tried using the previous guess to reduce the amount of computation
        // being done, but this got us stuck in infinite locations.  We seem to
        // do okay without using it, so leaving it out.
        // @todo Make number of iterations into a parameter.
        bool usePreviousGuess = false;
        int iterationsCount = 5;
        cv::Mat inlierIndices;
        cv::solvePnPRansac(
            objectPoints, imagePoints, m_cameraMatrix, m_distCoeffs, m_rvec,
            m_tvec, usePreviousGuess, iterationsCount, 8.0f,
            static_cast<int>(objectPoints.size() - m_permittedOutliers),
            inlierIndices);

        //==========================================================================
        // Make sure we got all the inliers we needed.  Otherwise, reject this
        // pose.
        if (inlierIndices.rows < m_requiredInliers) {
          return false;
        }

        //==========================================================================
        // Reproject the inliers into the image and make sure they are actually
        // close to the expected location; otherwise, we have a bad pose.
        const double pixelReprojectionErrorForSingleAxisMax = 4;
        if (inlierIndices.rows > 0) {
          std::vector<cv::Point3f>  inlierObjectPoints;
          std::vector<cv::Point2f> inlierImagePoints;
          for (int i = 0; i < inlierIndices.rows; i++) {
            inlierObjectPoints.push_back(objectPoints[i]);
            inlierImagePoints.push_back(imagePoints[i]);
          }
          std::vector<cv::Point2f> reprojectedPoints;
          cv::projectPoints(inlierObjectPoints, m_rvec, m_tvec, m_cameraMatrix,
            m_distCoeffs, reprojectedPoints);
          for (size_t i = 0; i < reprojectedPoints.size(); i++) {
            if (reprojectedPoints[i].x - inlierImagePoints[i].x > 
                pixelReprojectionErrorForSingleAxisMax) {
              return false;
            }
            if (reprojectedPoints[i].y - inlierImagePoints[i].y > 
              pixelReprojectionErrorForSingleAxisMax) {
              return false;
            }
          }
        }

        //==========================================================================
        // Convert this into an OSVR representation of the transformation that
        // gives the pose of the HDK origin in the camera coordinate system,
        // switching units to meters and encoding the angle in a unit
        // quaternion.
        // The matrix described by rvec and tvec takes points in model space
        // (the space where the LEDs are defined, which is in mm away from an
        // implied origin) into a coordinate system where the center is at the
        // camera's origin, with X to the right, Y down, and Z in the direction
        // that the camera is facing (but still in the original units of mm):
        //  |Xc|   |r11 r12 r13 t1| |Xm|
        //  |Yc| = |r21 r22 r23 t2|*|Ym|
        //  |Zc|   |r31 r32 r33 t3| |Zm|
        //                          |1 |
        //  That is, it rotates into the camera coordinate system and then adds
        // the translation, which is in the camera coordinate system.
        //  This is the transformation we want, since it reports the sensor's
        // position and orientation in camera space, except that we want to
        // convert
        // the units into meters and the orientation into a Quaternion.
        //  NOTE: This is a right-handed coordinate system with X pointing
        // towards the right from the camera center of projection, Y pointing
        // down, and Z pointing along the camera viewing direction.

        // Compose the transform in original units.
        // We start by making a 3x3 rotation matrix out of the rvec, which
        // is done by a function that for some reason is named Rodrigues.
        cv::Mat rot;
        cv::Rodrigues(m_rvec, rot);

        // @todo: Replace this code with Eigen code.

        if (rot.type() != CV_64F) {
            return false;
        }

        // Get the forward transform
        // Scale to meters
        q_xyz_quat_type forward;
        forward.xyz[Q_X] = m_tvec.at<double>(0);
        forward.xyz[Q_Y] = m_tvec.at<double>(1);
        forward.xyz[Q_Z] = m_tvec.at<double>(2);
        q_vec_scale(forward.xyz, 1e-3, forward.xyz);

        // Fill in a 4x4 matrix that starts as the identity
        // matrix with the 3x3 part from the rotation matrix.
        // We need to transpose the matrix to make it row
        // major.
        q_matrix_type rot4x4;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if ((i < 3) && (j < 3)) {
                    rot4x4[i][j] = rot.at<double>(j, i);
                } else {
                    if (i == j) {
                        rot4x4[i][j] = 1;
                    } else {
                        rot4x4[i][j] = 0;
                    }
                }
            }
        }
        q_from_row_matrix(forward.quat, rot4x4);

        //==============================================================
        // Put into OSVR format.
        osvrPose3FromQuatlib(&outPose, &forward);

        return true;
    }

    bool BeaconBasedPoseEstimator::ProjectBeaconsToImage(
        std::vector<cv::Point2f> &out) {
        // Make sure we have a pose.  Otherwise, we can't do anything.
        if (!m_gotPose) {
            return false;
        }

        cv::projectPoints(m_beacons, m_rvec, m_tvec, m_cameraMatrix,
                          m_distCoeffs, out);
        return true;
    }

} // End namespace vbtracker
} // End namespace osvr
