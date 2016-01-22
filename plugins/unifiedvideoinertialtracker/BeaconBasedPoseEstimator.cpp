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
#include <opencv2/calib3d/calib3d.hpp>

// Standard includes
#include <iostream>
#include <stdexcept>

namespace osvr {
namespace vbtracker {
    /// millimeters to meters
    static const double LINEAR_SCALE_FACTOR = 1000.;

    static const auto DEFAULT_MEASUREMENT_VARIANCE = 3.0;

    // The total number of frames that we can have dodgy Kalman tracking for
    // before RANSAC takes over again.
    static const std::size_t MAX_PROBATION_FRAMES = 10;

    static const std::size_t MAX_FRAMES_WITHOUT_MEASUREMENTS = 50;

    static const std::size_t MAX_FRAMES_WITHOUT_ID_BLOBS = 10;

    BeaconBasedPoseEstimator::BeaconBasedPoseEstimator(
        CameraParameters const &camParams, size_t requiredInliers,
        size_t permittedOutliers, ConfigParams const &params)
        : m_params(params), m_camParams(camParams) {
        m_gotPose = false;
        m_requiredInliers = requiredInliers;
        m_permittedOutliers = permittedOutliers;
    }

    bool BeaconBasedPoseEstimator::SetBeacons(
        const Point3Vector &beacons, Vec3Vector const &emissionDirection,
        std::vector<double> const &variance,
        BeaconIDPredicate const &autocalibrationFixedPredicate,
        double beaconAutocalibErrorScale) {
        // Our existing pose won't match anymore.
        m_gotPose = false;
        m_beacons.clear();
        m_updateBeaconCentroid(beacons);
        Eigen::Matrix3d beaconError =
            Eigen::Vector3d::Constant(m_params.initialBeaconError)
                .asDiagonal() *
            beaconAutocalibErrorScale;
        auto bNum = size_t{0};
        for (auto &beacon : beacons) {
            auto isFixed = autocalibrationFixedPredicate(bNum + 1);
            m_beaconFixed.push_back(isFixed);
            m_beacons.emplace_back(new BeaconState{
                cvToVector(beacon).cast<double>() - m_centroid,
                isFixed ? Eigen::Matrix3d::Zero() : beaconError});
            bNum++;
        }
        if (1 == variance.size()) {
            /// A single entry in the variance array implies use that for all
            /// beacons.
            m_beaconMeasurementVariance.resize(m_beacons.size(), variance[0]);
        } else {
            m_beaconMeasurementVariance = variance;
        }
        // ensure it's the right size
        m_beaconMeasurementVariance.resize(m_beacons.size(),
                                           DEFAULT_MEASUREMENT_VARIANCE);

        m_beaconEmissionDirection = emissionDirection;
        if (m_beaconEmissionDirection.size() != m_beacons.size()) {
            throw std::runtime_error("Beacon emission direction array size did "
                                     "not match number of beacons!");
        }
        // Prep the debug data.
        m_updateBeaconDebugInfoArray();
        return true;
    }

    void BeaconBasedPoseEstimator::m_updateBeaconCentroid(
        const Point3Vector &beacons) {
        if (m_params.offsetToCentroid) {
            Eigen::Vector3d beaconSum = Eigen::Vector3d::Zero();
            auto bNum = size_t{0};
            for (auto &beacon : beacons) {
                beaconSum += cvToVector(beacon).cast<double>();
                bNum++;
            }
            m_centroid = beaconSum / bNum;
            if (m_params.debug) {
                std::cout << "Beacon centroid: " << m_centroid.transpose()
                          << std::endl;
            }
        } else {
            m_centroid = Eigen::Vector3d::Map(m_params.manualBeaconOffset);
        }
    }

    void BeaconBasedPoseEstimator::m_updateBeaconDebugInfoArray() {
        m_beaconDebugData.resize(m_beacons.size());
    }

    bool BeaconBasedPoseEstimator::SetCameraParameters(
        CameraParameters const &camParams) {
        // Our existing pose won't match anymore.
        m_gotPose = false;

        m_camParams = camParams;
        return true;
    }

    OSVR_PoseState BeaconBasedPoseEstimator::GetState() const {
        OSVR_PoseState ret;
        util::eigen_interop::map(ret).rotation() = m_state.getQuaternion();
        util::eigen_interop::map(ret).translation() =
            m_convertInternalPositionRepToExternal(m_state.position());
        return ret;
    }

    Eigen::Vector3d BeaconBasedPoseEstimator::GetLinearVelocity() const {
        return m_state.velocity() / LINEAR_SCALE_FACTOR;
    }

    Eigen::Vector3d BeaconBasedPoseEstimator::GetAngularVelocity() const {
        return m_state.angularVelocity();
    }

    bool BeaconBasedPoseEstimator::EstimatePoseFromLeds(
        LedGroup &leds, OSVR_TimeValue const &tv, OSVR_PoseState &outPose) {
        auto ret = m_estimatePoseFromLeds(leds, tv, outPose);
        m_gotPose = ret;
        return ret;
    }
    Eigen::Vector3d
    BeaconBasedPoseEstimator::m_convertInternalPositionRepToExternal(
        Eigen::Vector3d const &pos) const {
        return (pos + m_centroid) / LINEAR_SCALE_FACTOR;
    }
    bool BeaconBasedPoseEstimator::m_estimatePoseFromLeds(
        LedGroup &leds, OSVR_TimeValue const &tv, OSVR_PoseState &outPose) {
        if (m_params.streamBeaconDebugInfo) {
            /// Only bother resetting if anyone is actually going to receive the
            /// data.
            for (auto &data : m_beaconDebugData) {
                data.reset();
            }
        }

        /// Check on the health of the Kalman filter.
        auto didReset = m_forceRansacIfKalmanNeedsReset(leds);
        if (didReset && m_params.debug) {
            std::cout << "Video-based tracker: lost fix, in-flight reset"
                      << std::endl;
        }

        bool usedKalman = false;
        bool result = false;
        if (!m_gotPose || !m_gotPrev || !m_permitKalman) {
            // Must use the direct approach
            result = m_pnpransacEstimator(leds);
        } else {
            auto dt = osvrTimeValueDurationSeconds(&tv, &m_prev);
            // Can use kalman approach
            result = m_kalmanAutocalibEstimator(leds, dt);
            usedKalman = true;
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
        if (usedKalman) {
            auto currentTime = util::time::getNow();
            auto dt2 = osvrTimeValueDurationSeconds(&currentTime, &tv);
            outPose = GetPredictedState(dt2 + m_params.additionalPrediction);
        }
        return true;
    }

    bool BeaconBasedPoseEstimator::m_forceRansacIfKalmanNeedsReset(
        LedGroup const &leds) {
        auto needsReset = (m_framesInProbation > MAX_PROBATION_FRAMES) ||
                          (m_framesWithoutUtilizedMeasurements >
                           MAX_FRAMES_WITHOUT_MEASUREMENTS);
        /// Additional case: We've been a while without blobs...
        if (!needsReset &&
            m_framesWithoutIdentifiedBlobs > MAX_FRAMES_WITHOUT_ID_BLOBS) {
            // In this case, we force ransac once we get blobs once again.
            auto const beaconsSize = m_beacons.size();
            for (auto const &led : leds) {
                if (!led.identified()) {
                    continue;
                }
                auto id = led.getID();
                if (id < beaconsSize) {
                    needsReset = true;
                    break;
                }
            }
        }
        if (needsReset) {
            m_framesInProbation = 0;
            m_framesWithoutUtilizedMeasurements = 0;
            m_framesWithoutIdentifiedBlobs = 0;

            /// Need to reset all elements of the state vector
            /// The directly-set ones will be set once RANSAC has a pose, but
            /// incremental orientation, and the velocities, aren't directly set
            /// by ransac and we don't want old values to hang around there,
            /// it's a good way to poison a new state.
            using StateVec = kalman::types::DimVector<State>;
            m_state.setStateVector(StateVec::Zero());

            /// This is what triggers RANSAC instead of the Kalman mode for the
            /// next frame.
            m_gotPose = false;
        }
        return needsReset;
    }

    bool BeaconBasedPoseEstimator::m_pnpransacEstimator(LedGroup &leds) {
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
                m_beaconDebugData[id].variance = -1;
                m_beaconDebugData[id].measurement = led.getLocation();
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

#if CV_MAJOR_VERSION == 2
        cv::solvePnPRansac(
            objectPoints, imagePoints, m_camParams.cameraMatrix,
            m_camParams.distortionParameters, m_rvec, m_tvec, usePreviousGuess,
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
            objectPoints, imagePoints, m_camParams.cameraMatrix,
            m_camParams.distortionParameters, m_rvec, m_tvec, usePreviousGuess,
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
            cv::projectPoints(
                inlierObjectPoints, m_rvec, m_tvec, m_camParams.cameraMatrix,
                m_camParams.distortionParameters, reprojectedPoints);

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

        m_gotMeasurement = true;
        m_resetState(cvToVector3d(m_tvec), cvRotVecToQuat(m_rvec));
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
        cv::projectPoints(beacons, m_rvec, m_tvec, m_camParams.cameraMatrix,
                          m_camParams.distortionParameters, out);
        return true;
    }

    void BeaconBasedPoseEstimator::permitKalmanMode(bool permitKalman) {
        m_permitKalman = permitKalman;
    }

    Eigen::Vector3d
    BeaconBasedPoseEstimator::getBeaconAutocalibPosition(std::size_t i) const {
        return m_beacons.at(i)->stateVector();
    }

    Eigen::Vector3d
    BeaconBasedPoseEstimator::getBeaconAutocalibVariance(std::size_t i) const {
        return m_beacons.at(i)->errorCovariance().diagonal();
    }

#if 0
    static const double InitialStateError[] = {.01, .01, .1,  1.,  1.,  .1,
                                               10., 10., 10., 10., 10., 10.};
#endif
    static const double InitialStateError[] = {0,   0,   0,   0,   0,   0,
                                               10., 10., 10., 10., 10., 10.};
    void
    BeaconBasedPoseEstimator::m_resetState(Eigen::Vector3d const &xlate,
                                           Eigen::Quaterniond const &quat) {
        // Note that here, units are millimeters and radians, and x and z are
        // the lateral translation dimensions, with z being distance from camera
        using StateVec = kalman::types::DimVector<State>;

        m_state.position() = xlate;

        m_state.setQuaternion(quat);
        m_state.setErrorCovariance(StateVec(InitialStateError).asDiagonal());

        m_model.setDamping(m_params.linearVelocityDecayCoefficient,
                           m_params.angularVelocityDecayCoefficient);
        m_model.setNoiseAutocorrelation(
            kalman::types::Vector<6>(m_params.processNoiseAutocorrelation));

        if (m_params.debug && m_permitKalman) {
            std::cout << "Video-based tracker: Beacon entering run state: pos:"
                      << m_state.position().transpose() << "\n orientation: "
                      << m_state.getQuaternion().coeffs().transpose()
                      << std::endl;
        }
    }

    void BeaconBasedPoseEstimator::dumpBeaconLocationsToStream(
        std::ostream &os) const {
        Eigen::IOFormat ourFormat(Eigen::StreamPrecision, 0, ",");
        for (auto const &beacon : m_beacons) {
            os << (beacon->stateVector() + m_centroid)
                      .transpose()
                      .format(ourFormat)
               << std::endl;
        }
    }

} // namespace vbtracker
} // namespace osvr
