/** @file
    @brief Implementation

    @date 2015, 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015, 2016 Sensics, Inc.
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
#include "PoseEstimator_RANSAC.h"
#include "LED.h"
#include "CameraParameters.h"
#include "cvToEigen.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>

// Standard includes
#include <algorithm>

namespace osvr {
namespace vbtracker {
    bool RANSACPoseEstimator::operator()(CameraParameters const &camParams,
                                         LedPtrList const &leds,
                                         BeaconStateVec const &beacons,
                                         std::vector<BeaconData> &beaconDebug,
                                         Eigen::Vector3d &outXlate,
                                         Eigen::Quaterniond &outQuat) {

        // We need to get a pair of matched vectors of points: 2D locations
        // with in the image and 3D locations in model space.  There needs to
        // be a correspondence between the points in these vectors, such that
        // the ith element in one matches the ith element in the other.  We
        // make these by looking up the locations of LEDs with known identifiers
        // and pushing both onto the vectors at the same time.

        std::vector<cv::Point3f> objectPoints;
        std::vector<cv::Point2f> imagePoints;
        std::vector<ZeroBasedBeaconId> beaconIds;
        for (auto const &led : leds) {
            auto id = makeZeroBased(led->getID());
            auto index = asIndex(id);
            beaconDebug[index].variance = -1;
            beaconDebug[index].measurement = led->getLocationForTracking();
            beaconIds.push_back(id);

            /// Effectively invert the image points here so we get the output of
            /// a coordinate system we want.
            imagePoints.push_back(led->getLocationForTracking());
            objectPoints.push_back(
                vec3dToCVPoint3f(beacons[index]->stateVector()));
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

        cv::Mat rvec;
        cv::Mat tvec;
#if CV_MAJOR_VERSION == 2
        cv::solvePnPRansac(
            objectPoints, imagePoints, camParams.cameraMatrix,
            camParams.distortionParameters, rvec, tvec, usePreviousGuess,
            iterationsCount, 8.0f,
            static_cast<int>(objectPoints.size() - m_permittedOutliers),
            inlierIndices);
#elif CV_MAJOR_VERSION == 3
        // parameter added to the OpenCV 3.0 interface in place of the number of
        // inliers
        /// @todo how to determine this requested confidence from the data we're
        /// given?
        double confidence = 0.99;
        auto ransacResult = cv::solvePnPRansac(
            objectPoints, imagePoints, camParams.cameraMatrix,
            camParams.distortionParameters, rvec, tvec, usePreviousGuess,
            iterationsCount, 8.0f, confidence, inlierIndices);
        if (!ransacResult) {
            return false;
        }
#else
#error "Unrecognized OpenCV version!"
#endif

        //==========================================================================
        // Make sure we got all the inliers we needed.  Otherwise, reject this
        // pose.
        if (inlierIndices.rows < static_cast<int>(m_requiredInliers)) {
            return false;
        }

        //==========================================================================
        // Reproject the inliers into the image and make sure they are actually
        // close to the expected location; otherwise, we have a bad pose.
        const double pixelReprojectionErrorForSingleAxisMax = 4;
        if (inlierIndices.rows > 0) {
            std::vector<cv::Point3f> inlierObjectPoints;
            std::vector<cv::Point2f> inlierImagePoints;
            std::vector<ZeroBasedBeaconId> inlierBeaconIds;
            for (int i = 0; i < inlierIndices.rows; i++) {
                inlierObjectPoints.push_back(objectPoints[i]);
                inlierImagePoints.push_back(imagePoints[i]);
                inlierBeaconIds.push_back(beaconIds[i]);
            }
            std::vector<cv::Point2f> reprojectedPoints;
            cv::projectPoints(
                inlierObjectPoints, rvec, tvec, camParams.cameraMatrix,
                camParams.distortionParameters, reprojectedPoints);

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

            /// Now, we will sort that vector of inlier beacon IDs so we can
            /// rapidly binary search it to flag the LEDs we used.

            // Need a custom comparator for the ID type.
            auto idComparator = [](ZeroBasedBeaconId const &lhs,
                                   ZeroBasedBeaconId const &rhs) {
                return lhs.value() < rhs.value();
            };
            std::sort(begin(inlierBeaconIds), end(inlierBeaconIds),
                      idComparator);
            // This lambda wraps binary_search to do what it says: check to see
            // if a given beacon ID is in the list of inlier beacons.
            auto isAnInlierBeacon = [&inlierBeaconIds, &idComparator](
                ZeroBasedBeaconId const &needle) {
                return std::binary_search(begin(inlierBeaconIds),
                                          end(inlierBeaconIds), needle,
                                          idComparator);
            };
            for (auto &led : leds) {
                if (isAnInlierBeacon(led->getID())) {
                    led->markAsUsed();
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
        // down, and Z pointing along the camera viewing direction, if the input
        // points are not inverted.

        outXlate = cvToVector3d(tvec);
        outQuat = cvRotVecToQuat(rvec);
        return true;
    }

    /// Variance in Meters^2
    static const double InitialPositionStateError = 0.;
    /// Variance in Radians^2
    static const double InitialOrientationStateError = 0.5;
    static const double InitialVelocityStateError = 0.;
    static const double InitialAngVelStateError = 0.;
    static const double InitialStateError[] = {
        InitialPositionStateError,    InitialPositionStateError,
        InitialPositionStateError,    InitialOrientationStateError,
        InitialOrientationStateError, InitialOrientationStateError,
        InitialVelocityStateError,    InitialVelocityStateError,
        InitialVelocityStateError,    InitialAngVelStateError,
        InitialAngVelStateError,      InitialAngVelStateError};
    bool RANSACPoseEstimator::operator()(EstimatorInOutParams const &p,
                                         LedPtrList const &leds) {
        Eigen::Vector3d xlate;
        Eigen::Quaterniond quat;
        /// Call the main pose estimation to get the vector and quat.
        {
            auto ret = (*this)(p.camParams, leds, p.beacons, p.beaconDebug,
                               xlate, quat);
            if (!ret) {
                return false;
            }
        }
        /// OK, so if we're here, estimation succeeded and we have valid data in
        /// xlate and quat.
        p.state.position() = xlate;
        p.state.setQuaternion(quat);
/// Zero things we can't measure.
#if 1
        p.state.incrementalOrientation() = Eigen::Vector3d::Zero();
        p.state.velocity() = Eigen::Vector3d::Zero();
        p.state.angularVelocity() = Eigen::Vector3d::Zero();
#endif
        using StateVec = kalman::types::DimVector<BodyState>;
        using StateSquareMatrix = kalman::types::DimSquareMatrix<BodyState>;

        StateSquareMatrix covariance = StateVec(InitialStateError).asDiagonal();
        /// @todo Copy the existing angular velocity error covariance
        /*
        covariance.bottomRightCorner<3, 3>() =
            p.state.errorCovariance().bottomRightCorner<3, 3>();
            */
        p.state.setErrorCovariance(covariance);
        return true;
    }

} // namespace vbtracker
} // namespace osvr
