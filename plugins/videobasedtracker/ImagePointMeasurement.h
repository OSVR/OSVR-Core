/** @file
    @brief Header

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ImagePointMeasurement_h_GUID_BE292A08_8C31_4987_E179_CD2F0CE63183
#define INCLUDED_ImagePointMeasurement_h_GUID_BE292A08_8C31_4987_E179_CD2F0CE63183

// Internal Includes
#include "ProjectPoint.h"
#include <osvr/Kalman/FlexibleKalmanBase.h>
#include <osvr/Kalman/PureVectorState.h>
#include <osvr/Kalman/AugmentedState.h>
#include <osvr/Kalman/PoseState.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    using AugmentedStateWithBeacon =
        kalman::AugmentedState<kalman::pose_externalized_rotation::State,
                               kalman::PureVectorState<3>>;
    struct CameraModel {
        Eigen::Vector2d principalPoint;
        double focalLength;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    };
    /// Measurement class for auto-calibrating Kalman filter in video-based
    /// tracker.
    class ImagePointMeasurement {
      public:
        static const kalman::types::DimensionType DIMENSION = 2;
        using Vector = kalman::types::Vector<DIMENSION>;
        using SquareMatrix = kalman::types::SquareMatrix<DIMENSION>;
        using State = AugmentedStateWithBeacon;
        using Jacobian =
            kalman::types::Matrix<DIMENSION,
                                  kalman::types::Dimension<State>::value>;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        explicit ImagePointMeasurement(CameraModel const &cam)
            : m_variance(2.0), m_cam(cam) {}

        /// Updates some internal cached partial solutions.
        void updateFromState(State const &state) {
            // 3d position of beacon
            m_beacon = state.b().stateVector();
            m_rot = state.a().getCombinedQuaternion().toRotationMatrix();
            m_objExtRot = state.a().getQuaternion() * m_beacon;
            m_incRot = state.a().incrementalOrientation();
            m_rotatedObjPoint = m_rot * m_beacon;
            m_rotatedTranslatedPoint = m_rotatedObjPoint + state.a().position();
            m_xlate = state.a().position();
        }
        Vector getResidual(State const &state) const {
            // 3d position of beacon
            Eigen::Vector2d predicted =
                projectPoint(m_cam.focalLength, m_cam.principalPoint,
                             m_rotatedTranslatedPoint);
            return m_measurement - predicted;
        }

        void setMeasurement(Vector const &m) { m_measurement = m; }
        Eigen::Matrix<double, 2, 3> getBeaconJacobian() const {
            auto v1 = m_rot(0, 2) * m_beacon[2] + m_rot(0, 1) * m_beacon[1] +
                      m_beacon[0] * m_rot(0, 0) + m_xlate[0];
            auto v2 = m_beacon[2] * m_rot(2, 2) + m_beacon[1] * m_rot(2, 1) +
                      m_beacon[0] * m_rot(2, 0) + m_xlate[2];
            auto v3 = 1 / (v2 * v2);
            auto v4 = 1 / v2;
            auto v5 = m_rot(1, 2) * m_beacon[2] + m_beacon[1] * m_rot(1, 1) +
                      m_beacon[0] * m_rot(1, 0) + m_xlate[1];
            Eigen::Matrix<double, 2, 3> ret;
            ret << m_rot(0, 0) * v4 * m_cam.focalLength -
                       v1 * m_rot(2, 0) * v3 * m_cam.focalLength,
                m_rot(0, 1) * v4 * m_cam.focalLength -
                    v1 * m_rot(2, 1) * v3 * m_cam.focalLength,
                m_rot(0, 2) * v4 * m_cam.focalLength -
                    v1 * m_rot(2, 2) * v3 * m_cam.focalLength,
                m_rot(1, 0) * v4 * m_cam.focalLength -
                    v5 * m_rot(2, 0) * v3 * m_cam.focalLength,
                m_rot(1, 1) * v4 * m_cam.focalLength -
                    v5 * m_rot(2, 1) * v3 * m_cam.focalLength,
                m_rot(1, 2) * v4 * m_cam.focalLength -
                    v5 * m_rot(2, 2) * v3 * m_cam.focalLength;
            return ret;
        }
#if 0
        /// This version assumes incrot == 0
        Eigen::Matrix<double, 2, 3> getRotationJacobian() const {
            auto v1 = m_rotatedObjPoint[0] + m_xlate[0];
            auto v2 = m_rotatedObjPoint[2] + m_xlate[2];
            auto v3 = 1 / (v2 * v2);
            auto v4 = 1 / v2;
            auto v5 = m_rotatedObjPoint[1] + m_xlate[1];
            Eigen::Matrix<double, 2, 3> ret;
            ret << -v1 * m_rotatedObjPoint[1] * v3 * m_cam.focalLength,
                m_rotatedObjPoint[2] * v4 * m_cam.focalLength +
                    m_rotatedObjPoint[0] * v1 * v3 * m_cam.focalLength,
                -m_rotatedObjPoint[1] * v4 * m_cam.focalLength,
                (-m_rotatedObjPoint[2] * v4 * m_cam.focalLength) -
                    m_rotatedObjPoint[1] * v5 * v3 * m_cam.focalLength,
                m_rotatedObjPoint[0] * v5 * v3 * m_cam.focalLength,
                m_rotatedObjPoint[0] * v4 * m_cam.focalLength;
            return ret;
        }
#endif

#if 1
        /// This version also assumes incrot == 0 but does the computation in a
        /// much more elegant way.
        Eigen::Matrix<double, 2, 3> getRotationJacobian() const {
            // just grabbing x and y as an array for component-wise manip right
            // now.
            Eigen::Array2d rotXlated =
                m_rotatedTranslatedPoint.head<2>().array();
            Eigen::Array2d rotObj = m_rotatedObjPoint.head<2>().array();

            // Utility because a lot of this requires negatives applied in one
            // row but not the other.
            Eigen::Array2d negativePositive(-1, 1);
            Eigen::Array2d positiveNegative(1, -1);
            // Some common Z stuff
            double zRecip = 1. / m_rotatedTranslatedPoint.z();
            double rotObjZ = m_rotatedObjPoint.z();
            Eigen::Array2d mainDiagonal =
                rotXlated * rotObj.reverse() * negativePositive * zRecip;
            Eigen::Array2d otherDiagonal =
                (rotObj * rotXlated * zRecip + rotObjZ) * positiveNegative;
            Eigen::Array2d lastCol = rotObj.reverse() * negativePositive;
            Eigen::Matrix<double, 2, 3> prelim;
            prelim.leftCols<2>() << mainDiagonal[0], otherDiagonal[0],
                otherDiagonal[1], mainDiagonal[1];
            prelim.rightCols<1>() << lastCol.matrix();
            return prelim * zRecip * m_cam.focalLength;
        }
#endif

#if 0
        /// This substantially-more-complex-looking version does separately use
        /// the incremental rotation state.
        Eigen::Matrix<double, 2, 3> getRotationJacobian() const {
            /// Precalculated outer product of the incremental rotation to
            /// simplify the generated code.
            Eigen::Matrix3d incrotOP;
            incrotOP.triangularView<Eigen::Upper>() =
                m_incRot * m_incRot.transpose();

            auto v1 = 0.5 - incrotOP(1, 2) / 24;
            auto v2 = 2 * m_incRot[0] * v1;
            auto v3 = -incrotOP(1, 2) / 6;
            auto v4 = incrotOP(1, 2) / 6;
            auto v5 = -(incrotOP(1, 1) * m_incRot[2]) / 24;
            auto v6 =
                m_objExtRot[0] * (v5 + v4) + m_objExtRot[1] * (v3 + v2 + 1);
            auto v7 = -incrotOP(0, 2) / 6;
            auto v8 = v7 + 1;
            auto v9 = 0.5 - incrotOP(0, 2) / 24;
            auto v10 = incrotOP(1, 1) * v9;
            auto v11 = v3 + 1;
            auto v12 = incrotOP(0, 0) * v1;
            auto v13 = m_objExtRot[0] * (v10 - m_incRot[1] * v8) +
                       m_objExtRot[1] * (v12 + m_incRot[0] * v11) +
                       m_objExtRot[2] + m_xlate[2];
            auto v14 = 1 / (v13 * v13);
            auto v15 = -incrotOP(0, 1) / 6;
            auto v16 = v15 + 1;
            auto v17 = 0.5 - incrotOP(0, 1) / 24;
            auto v18 = incrotOP(2, 2) * v17;
            auto v19 = m_objExtRot[2] * (m_incRot[1] * v8 + v10) +
                       m_objExtRot[1] * (v18 - m_incRot[2] * v16) +
                       m_objExtRot[0] + m_xlate[0];
            auto v20 = 1 / v13;
            auto v21 = -(m_incRot[1] * incrotOP(2, 2)) / 24;
            auto v22 = 2 * m_incRot[1] * v9;
            auto v23 = incrotOP(0, 2) / 6;
            auto v24 = -(m_incRot[0] * incrotOP(2, 2)) / 24;
            auto v25 = -(incrotOP(0, 0) * m_incRot[2]) / 24;
            auto v26 =
                m_objExtRot[1] * (v7 + v25) + m_objExtRot[0] * (v23 + v22 - 1);
            auto v27 = incrotOP(0, 1) / 6;
            auto v28 = 2 * m_incRot[2] * v17;
            auto v29 = -(m_incRot[0] * incrotOP(1, 1)) / 24;
            auto v30 = -(incrotOP(0, 0) * m_incRot[1]) / 24;
            auto v31 =
                m_objExtRot[1] * (v30 + v15) + m_objExtRot[0] * (v29 + v27);
            auto v32 = m_objExtRot[0] * (v18 + m_incRot[2] * v16) +
                       m_objExtRot[2] * (v12 - m_incRot[0] * v11) +
                       m_objExtRot[1] + m_xlate[1];
            Eigen::Matrix<double, 2, 3> ret;
            ret << v20 * (m_objExtRot[1] * (v21 + v4) +
                          (v5 + v3) * m_objExtRot[2]) *
                           m_cam.focalLength -
                       v6 * v14 * v19 * m_cam.focalLength,
                v20 * (m_objExtRot[1] * (v24 + v23) +
                       (v22 + v7 + 1) * m_objExtRot[2]) *
                        m_cam.focalLength -
                    v26 * v14 * v19 * m_cam.focalLength,
                v20 * ((v29 + v15) * m_objExtRot[2] +
                       m_objExtRot[1] * (v28 + v27 - 1)) *
                        m_cam.focalLength -
                    v31 * v14 * v19 * m_cam.focalLength,
                v20 * (m_objExtRot[0] * (v21 + v3) +
                       (v4 + v2 - 1) * m_objExtRot[2]) *
                        m_cam.focalLength -
                    v6 * v14 * v32 * m_cam.focalLength,
                v20 * (m_objExtRot[0] * (v24 + v7) +
                       (v25 + v23) * m_objExtRot[2]) *
                        m_cam.focalLength -
                    v26 * v14 * v32 * m_cam.focalLength,
                v20 * ((v30 + v27) * m_objExtRot[2] +
                       m_objExtRot[0] * (v28 + v15 + 1)) *
                        m_cam.focalLength -
                    v31 * v14 * v32 * m_cam.focalLength;
            return ret;
        }
#endif
        Jacobian getJacobian(State const &state) const {
            Jacobian ret;
            ret <<
                // with respect to change in x or y
                Eigen::Matrix2d::Identity() *
                    (m_cam.focalLength / m_rotatedTranslatedPoint.z()),
                // with respect to change in z
                -m_rotatedTranslatedPoint.head<2>() * m_cam.focalLength /
                    (m_rotatedTranslatedPoint.z() *
                     m_rotatedTranslatedPoint.z()),
                // with respect to change in incremental rotation
                getRotationJacobian(),
                // with respect to change in linear/angular velocity
                Eigen::Matrix<double, 2, 6>::Zero(),
                // with respect to change in beacon position
                getBeaconJacobian();
            return ret;
        }

        void setVariance(double s) {
            if (s > 0) {
                m_variance = s;
            }
        }
        SquareMatrix getCovariance(State &state) const {
            /// @todo make this better, perhaps state dependent?
            return Vector::Constant(m_variance).asDiagonal();
        }

      private:
        double m_variance;
        Vector m_measurement;
        CameraModel m_cam;
        Eigen::Vector3d m_beacon;
        Eigen::Vector3d m_objExtRot;
        Eigen::Vector3d m_incRot;
        Eigen::Vector3d m_rotatedObjPoint;
        Eigen::Vector3d m_rotatedTranslatedPoint;
        Eigen::Vector3d m_xlate;
        Eigen::Matrix3d m_rot;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_ImagePointMeasurement_h_GUID_BE292A08_8C31_4987_E179_CD2F0CE63183
