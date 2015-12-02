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
#include "cvToEigen.h"
#include "ImagePointMeasurement.h" // for projectPoint

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenInterop.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    /// millimeters to meters
    static const double LINEAR_SCALE_FACTOR = 1000.;
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
        m_beacons.clear();
        static const double INITIAL_BEACON_ERROR = 0.0;
        Eigen::Matrix3d beaconError =
            Eigen::Vector3d::Constant(INITIAL_BEACON_ERROR).asDiagonal();
        for (auto &beacon : beacons) {
            m_beacons.emplace_back(new BeaconState{
                cvToVector(beacon).cast<double>(), beaconError});
        }

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

        // Extract just the pieces we'll use
        m_focalLength = m_cameraMatrix.at<double>(0, 0);
        m_principalPoint = Eigen::Vector2d{m_cameraMatrix.at<double>(0, 2),
                                           m_cameraMatrix.at<double>(1, 2)};
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
        /// @todo use these in the kalman path

        //    std::cout << "XXX distCoeffs =" << std::endl << m_distCoeffs <<
        //    std::endl;
        return true;
    }

    OSVR_PoseState BeaconBasedPoseEstimator::GetState() const {
        OSVR_PoseState ret;
        util::eigen_interop::map(ret).rotation() = m_state.getQuaternion();
        util::eigen_interop::map(ret).translation() =
            m_state.getPosition() / LINEAR_SCALE_FACTOR; // convert from mm to m
        return ret;
    }

    Eigen::Vector3d BeaconBasedPoseEstimator::GetLinearVelocity() const {
        return m_state.getVelocity() / LINEAR_SCALE_FACTOR;
    }

    Eigen::Vector3d BeaconBasedPoseEstimator::GetAngularVelocity() const {
        return m_state.getAngularVelocity();
    }

    bool
    BeaconBasedPoseEstimator::EstimatePoseFromLeds(const LedGroup &leds,
                                                   OSVR_TimeValue const &tv,
                                                   OSVR_PoseState &outPose) {
        auto ret = m_estimatePoseFromLeds(leds, tv, outPose);
        m_gotPose = ret;
        return ret;
    }

    bool
    BeaconBasedPoseEstimator::m_estimatePoseFromLeds(const LedGroup &leds,
                                                     OSVR_TimeValue const &tv,
                                                     OSVR_PoseState &outPose) {
        bool result = false;
        if (!m_gotPose || !m_gotPrev) {
            // Must use the direct approach
            result = m_pnpransacEstimator(leds);
        } else {
            auto dt = osvrTimeValueDurationSeconds(&tv, &m_prev);
            // Can use kalman approach
            result = m_kalmanAutocalibEstimator(leds, dt);
        }

        if (!result) {
            return false;
        }
        m_prev = tv;
        /// @todo need to advance this if we do the kalman thing no matter if we
        /// get results or not
        m_gotPrev = true;

        //==============================================================
        // Put into OSVR format.
        outPose = GetState();

        return true;
    }

    bool BeaconBasedPoseEstimator::m_pnpransacEstimator(const LedGroup &leds) {
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
                objectPoints.push_back(
                    vec3dToCVPoint3f(m_beacons[id]->stateVector()));
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
            std::vector<cv::Point3f> inlierObjectPoints;
            std::vector<cv::Point2f> inlierImagePoints;
            for (int i = 0; i < inlierIndices.rows; i++) {
                inlierObjectPoints.push_back(objectPoints[i]);
                inlierImagePoints.push_back(imagePoints[i]);
            }
            std::vector<cv::Point2f> reprojectedPoints;
            cv::projectPoints(inlierObjectPoints, m_rvec, m_tvec,
                              m_cameraMatrix, m_distCoeffs, reprojectedPoints);

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

        m_resetState(cvToVector3d(m_tvec), cvRotVecToQuat(m_rvec));
#if 0
        cv::Mat rvec;
        m_rvec.copyTo(rvec);
        cv::Mat roundTripped = eiQuatToRotVec(m_state.getQuaternion());
        std::cout << "Round-trip error: " << rvec - roundTripped << std::endl;
        {
            static int i = 0;
            if (i == 0) {
                const double pixelReprojectionErrorForSingleAxisMax = 4;
                if (inlierIndices.rows > 0) {

                    double squaredError = 0;
                    for (int i = 0; i < inlierIndices.rows; i++) {

                        Eigen::Vector3d objPoint =
                            cvToVector(objectPoints[i]).cast<double>();
                        Eigen::Vector2d imgPoint = projectPoint(
                            m_state.getPosition(), m_state.getQuaternion(),
                            m_focalLength, m_principalPoint, objPoint);
                        std::cout << "reproj residual "
                                  << (imgPoint -
                                      cvToVector(imagePoints[i]).cast<double>())
                                         .transpose()
                                  << std::endl;
                        double err = (imgPoint -
                                      cvToVector(imagePoints[i]).cast<double>())
                                         .squaredNorm();
                        squaredError += err;
                        if (std::sqrt(err) >
                            pixelReprojectionErrorForSingleAxisMax) {
                            std::cout
                                << "Got a reprojection error of "
                                << std::sqrt(err)
                                << " using the Kalman-utilized projection code!"
                                << std::endl;
                        }
                    }
                    std::cout
                        << "RMS error for kalman-utilized projection code: "
                        << std::sqrt(squaredError) << std::endl;
                }
            }
            i = (i + 1) % 200;
        }
#endif
        return true;
    }

    bool BeaconBasedPoseEstimator::ProjectBeaconsToImage(
        std::vector<cv::Point2f> &out) {
        // Make sure we have a pose.  Otherwise, we can't do anything.
        if (!m_gotPose) {
            return false;
        }
        // Convert our beacon-states into OpenCV-friendly structures.
        Point3Vector beacons;
        for (auto const &beacon : m_beacons) {
            beacons.push_back(vec3dToCVPoint3f(beacon->stateVector()));
        }
        // Do the OpenCV projection.
        cv::projectPoints(beacons, m_rvec, m_tvec, m_cameraMatrix, m_distCoeffs,
                          out);
        return true;
    }

} // namespace vbtracker
} // namespace osvr
