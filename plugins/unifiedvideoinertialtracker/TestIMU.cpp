/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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
#include "ConfigParams.h"
#include "IMUStateMeasurements.h"

// Library/third-party includes
#include <osvr/Util/Angles.h>

// Standard includes
#include <iostream>
#include <memory>

#define CATCH_CONFIG_MAIN
#include "catch_typelist.h"
#include <catch.hpp>

using std::unique_ptr;
using namespace osvr;
using namespace vbtracker;
using namespace Eigen;
struct TestData {
    TestData() {
        /// Set up from a default configuration.
        ConfigParams params;
#if 0
        state.setErrorCovariance(
            kalman::types::DimVector<BodyState>::Constant(100).asDiagonal());
#endif

        /// configure the process model
        processModel.setDamping(params.linearVelocityDecayCoefficient,
                                params.angularVelocityDecayCoefficient);
        processModel.setNoiseAutocorrelation(
            kalman::types::Vector<6>::Map(params.processNoiseAutocorrelation));

        /// Save the IMU measurement parameters.
        imuVariance = Eigen::Vector3d::Constant(params.imu.orientationVariance);
    }
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    /// Quantity computed from the "camera pose" (room calibration output)
    Eigen::Quaterniond roomToCameraRotation = Eigen::Quaterniond::Identity();

    /// Quantity computed during room calibration output
    util::Angle yawCorrection = 0;

    /// Transform the IMU-measured quaternion using the calibration output
    /// mocked up in this struct.
    Eigen::Quaterniond xform(Eigen::Quaterniond const &quat) {
        return getTransformedOrientation(quat, roomToCameraRotation,
                                         yawCorrection);
    }

    /// Default constructor is no translation, identity rotation, 10 along
    /// main diagonal as the state covariance matrix.
    BodyState state;

    BodyProcessModel processModel;
    Eigen::Vector3d imuVariance;
};
namespace Eigen {
inline void outputQuat(std::ostream &os, Quaterniond const &q) {
    os << "[" << q.w() << " (" << q.vec().transpose() << ")]";
}
std::ostream &operator<<(std::ostream &os, Quaterniond const &q) {
    outputQuat(os, q);
    return os;
}
} // namespace Eigen

namespace Catch {
template <> struct TypelistTypeNameTrait<kalman::QFirst> {
    static const char *get() { return "kalman::QFirst"; }
};
template <> struct TypelistTypeNameTrait<kalman::QLast> {
    static const char *get() { return "kalman::QLast"; }
};
template <> struct TypelistTypeNameTrait<kalman::SplitQ> {
    static const char *get() { return "kalman::SplitQ"; }
};
} // namespace Catch

static const double SMALL_VALUE = 0.1;
template <typename MeasurementType>
inline void smallPositiveYChecks(TestData *data, MeasurementType &kalmanMeas) {
    using JacobianType = typename MeasurementType::JacobianType;
    Vector3d residual = kalmanMeas.getResidual(data->state);
    AND_THEN("residual should be 0, SMALL_VALUE, 0") {
        CAPTURE(residual.transpose());
        CHECK(residual.isApprox(Vector3d(0, SMALL_VALUE, 0)));
    }

    JacobianType jacobian = kalmanMeas.getJacobian(data->state);
    AND_THEN("the jacobian should be finite") {
        INFO("jacobian\n" << jacobian);
        REQUIRE(jacobian.allFinite());
        AND_THEN("the jacobian should not be zero") {
            REQUIRE_FALSE(jacobian.isApproxToConstant(0));
        }
    }

    auto correctionInProgress =
        kalman::beginCorrection(data->state, data->processModel, kalmanMeas);
    AND_THEN("computed deltaz should equal residual") {
        CAPTURE(residual.transpose());
        CAPTURE(correctionInProgress.deltaz.transpose());
        CHECK(residual.isApprox(correctionInProgress.deltaz));
    }
    CAPTURE(correctionInProgress.stateCorrection.transpose());
    AND_THEN("state correction should be finite") {
        REQUIRE(correctionInProgress.stateCorrectionFinite);
        AND_THEN("state correction should have a rotation component with small "
                 "positive y") {
            REQUIRE(correctionInProgress.stateCorrection[3] == Approx(0.));
            REQUIRE(correctionInProgress.stateCorrection[4] > 0);
            REQUIRE(correctionInProgress.stateCorrection[5] == Approx(0.));
        }
        AND_THEN("state correction should have an angular velocity component "
                 "with zero or small positive y") {
            REQUIRE(correctionInProgress.stateCorrection[9] == Approx(0.));
            REQUIRE(correctionInProgress.stateCorrection[10] >= 0.);
            REQUIRE(correctionInProgress.stateCorrection[11] == Approx(0.));
        }
        AND_THEN("state correction should not contain any translational/linear "
                 "velocity components") {
            REQUIRE(correctionInProgress.stateCorrection.head<3>()
                        .isApproxToConstant(0.));
            REQUIRE(correctionInProgress.stateCorrection.segment<3>(6)
                        .isApproxToConstant(0.));
        }
    }
}

CATCH_TYPELIST_TESTCASE("identity calibration output", kalman::QFirst,
                        kalman::QLast, kalman::SplitQ) {
    using MeasurementType = OrientationMeasurementUsingPolicy<TypeParam>;
    using JacobianType = typename MeasurementType::JacobianType;
    unique_ptr<TestData> data(new TestData);
    GIVEN("an identity state") {
        WHEN("filtering in an identity measurement") {
            Quaterniond xformedMeas = data->xform(Quaterniond::Identity());

            THEN("the transformed measurement should equal the measurement") {
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(Quaterniond::Identity()));

                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                AND_THEN("residual should be zero") {
                    CAPTURE(kalmanMeas.getResidual(data->state));
                    REQUIRE(kalmanMeas.getResidual(data->state)
                                .isApproxToConstant(0.));
                }

                JacobianType jacobian = kalmanMeas.getJacobian(data->state);
                AND_THEN("the jacobian should be finite") {
                    INFO("jacobian\n" << jacobian);
                    REQUIRE(jacobian.allFinite());
                    AND_THEN("the jacobian should not be zero") {
                        REQUIRE_FALSE(jacobian.isApproxToConstant(0));
                    }
                    auto correctionInProgress = kalman::beginCorrection(
                        data->state, data->processModel, kalmanMeas);
                    AND_THEN("computed deltaz should be zero") {
                        CAPTURE(correctionInProgress.deltaz.transpose());
                        REQUIRE(
                            correctionInProgress.deltaz.isApproxToConstant(0.));
                    }
                    CAPTURE(correctionInProgress.stateCorrection.transpose());
                    AND_THEN("state correction should be finite") {
                        REQUIRE(correctionInProgress.stateCorrectionFinite);
                        AND_THEN("state correction should be zero") {
                            REQUIRE(correctionInProgress.stateCorrection
                                        .isApproxToConstant(0.));
                        }
                    }
                }
            }
        }
        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY(
                AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));
                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                smallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
    GIVEN("a state rotated about y") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitY()));
        data->state.setQuaternion(stateRotation);
        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY =
                stateRotation *
                Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));

                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                smallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
    GIVEN("a state rotated about x") {
        Quaterniond stateRotation(AngleAxisd(M_PI / 4., Vector3d::UnitX()));
        data->state.setQuaternion(stateRotation);
        WHEN("filtering in a small positive rotation about y") {
            Quaterniond smallPositiveRotationAboutY =
                stateRotation *
                Quaterniond(AngleAxisd(SMALL_VALUE, Vector3d::UnitY()));
            CAPTURE(SMALL_VALUE);
            CAPTURE(smallPositiveRotationAboutY);
            Quaterniond xformedMeas = data->xform(smallPositiveRotationAboutY);
            CAPTURE(xformedMeas);

            THEN("the transformed measurement should equal the measurement") {
                REQUIRE(xformedMeas.isApprox(smallPositiveRotationAboutY));

                /// Do the rest of the checks for a small rotation about y
                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                smallPositiveYChecks(data.get(), kalmanMeas);
            }
        }
    }
}

TEST_CASE("Sigma point reconstruction validity") {
    using namespace osvr::kalman;
    Matrix3d cov(Vector3d::Constant(10).asDiagonal());
    using Generator = SigmaPointGenerator<3>;
    using Reconstructor = ReconstructedDistributionFromSigmaPoints<3, 3>;
    const auto params = SigmaPointParameters(3);
    WHEN("Starting with a zero mean") {
        Vector3d mean = Vector3d::Zero();
        auto gen = Generator(mean, cov, params);
        auto recon = Reconstructor(gen, gen.getSigmaPoints());
        THEN("the reconstructed distribution should be approximately equal to "
             "the input") {
            CAPTURE(gen.getSigmaPoints());
            CAPTURE(recon.getMean());
            REQUIRE(recon.getMean().isApproxToConstant(0));
            CAPTURE(recon.getCov());
            REQUIRE(recon.getCov().isApprox(cov));
        }
    }

    WHEN("Starting with a non-zero mean") {
        Vector3d mean = Vector3d(1, 2, 3);
        auto gen = Generator(mean, cov, params);
        auto recon = Reconstructor(gen, gen.getSigmaPoints());
        THEN("the reconstructed distribution should be approximately equal to "
             "the input") {
            CAPTURE(gen.getSigmaPoints());
            CAPTURE(recon.getMean());
            REQUIRE(recon.getMean().isApprox(mean));
            CAPTURE(recon.getCov());
            REQUIRE(recon.getCov().isApprox(cov));
        }
    }
}

namespace osvr {
namespace kalman {

    template <std::size_t Dim, std::size_t OrigDim = Dim>
    class AugmentedSigmaPointGenerator {
      public:
        static_assert(OrigDim <= Dim, "Original, non-augmented dimension must "
                                      "be equal or less than the full "
                                      "dimension");
        static const std::size_t L = Dim;
        static const std::size_t OriginalDimension = OrigDim;
        static const std::size_t NumSigmaPoints = L * 2 + 1;
        using MeanVec = types::Vector<Dim>;
        using CovMatrix = types::SquareMatrix<Dim>;
        using SigmaPointsMat = types::Matrix<Dim, NumSigmaPoints>;
        using SigmaPointWeightVec = types::Vector<NumSigmaPoints>;
        template <std::size_t OutputDimension>
        using TransformedSigmaPointsMat =
            types::Matrix<OutputDimension, NumSigmaPoints>;
        AugmentedSigmaPointGenerator(MeanVec const &mean, CovMatrix const &cov,
                                     SigmaPointParameters params)
            : mean_(mean), covariance_(cov) {
            weights_ =
                SigmaPointWeightVec::Constant(params.getOtherEltWeights());
            weightsForCov_ = weights_;
            weights_[0] = params.getElt0MeanWeight();
            weightsForCov_[0] = params.getElt0CovWeight();
            scaledMatrixSqrt_ = cov.llt().matrixL();
            scaledMatrixSqrt_ *= params.gamma;
            sigmaPoints_ << mean, scaledMatrixSqrt_.colwise() + mean,
                (-scaledMatrixSqrt_).colwise() + mean;
        }

        SigmaPointsMat const &getSigmaPoints() const { return sigmaPoints_; }

        using SigmaPointBlock = Eigen::Block<SigmaPointsMat, OrigDim, 1>;
        using ConstSigmaPointBlock =
            Eigen::Block<const SigmaPointsMat, OrigDim, 1>;

        ConstSigmaPointBlock getSigmaPoint(std::size_t i) const {
            return sigmaPoints_.block<OrigDim, 1>(0, i);
        }

        SigmaPointWeightVec const &getWeightsForMean() const {
            return weights_;
        }
        SigmaPointWeightVec const &getWeightsForCov() const {
            return weightsForCov_;
        }

        MeanVec const &getMean() const { return mean_; }

      private:
        MeanVec mean_;
        CovMatrix covariance_;
        CovMatrix scaledMatrixSqrt_;
        SigmaPointsMat sigmaPoints_;
        SigmaPointWeightVec weights_;
        SigmaPointWeightVec weightsForCov_;
    };

    template <std::size_t XformedDim, typename SigmaPointsGenType>
    class SigmaDistributionReconstruction {
      public:
        static const std::size_t DIMENSION = XformedDim;
        using SigmaPointsGen = SigmaPointsGenType;
        static const std::size_t NumSigmaPoints =
            SigmaPointsGen::NumSigmaPoints;

        static const types::DimensionType OriginalDimension =
            SigmaPointsGen::OriginalDimension;
        using TransformedSigmaPointsMat =
            typename SigmaPointsGen::template TransformedSigmaPointsMat<
                XformedDim>;

        using CrossCovMatrix = types::Matrix<OriginalDimension, DIMENSION>;

        using MeanVec = types::Vector<XformedDim>;
        using CovMat = types::SquareMatrix<XformedDim>;
        SigmaDistributionReconstruction(
            SigmaPointsGen const &sigmaPoints,
            TransformedSigmaPointsMat const &xformedPointsMat)
            : xformedCov_(CovMat::Zero()), crossCov_(CrossCovMatrix::Zero()) {
            /// weighted average
            xformedMean_ = xformedPointsMat * sigmaPoints.getWeightsForMean();
            TransformedSigmaPointsMat zeroMeanPoints =
                xformedPointsMat.colwise() - xformedMean_;

            for (std::size_t i = 0; i < NumSigmaPoints; ++i) {
                auto weight = sigmaPoints.getWeightsForCov()[i];
                xformedCov_ += weight * zeroMeanPoints.col(i) *
                               zeroMeanPoints.col(i).transpose();
                crossCov_ += weight * sigmaPoints.getSigmaPoint(i) *
                             zeroMeanPoints.col(i).transpose();
            }
        }

        MeanVec const &getMean() const { return xformedMean_; }
        CovMat const &getCov() const { return xformedCov_; }
        // matrix of cross-covariance between original and transformed (such as
        // state and measurement)
        CrossCovMatrix const &getCrossCov() const { return crossCov_; }

      private:
        MeanVec xformedMean_;
        CovMat xformedCov_;
        CrossCovMatrix crossCov_;
    };

    template <typename State, typename Measurement>
    class SigmaPointCorrectionApplication {
      public:
        static const types::DimensionType StateDim =
            types::Dimension<State>::value;
        static const types::DimensionType MeasurementDim =
            types::Dimension<Measurement>::value;
        /// state augmented with measurement noise mean
        static const types::DimensionType AugmentedStateDim =
            StateDim + MeasurementDim;
        using StateVec = types::DimVector<State>;
        using StateSquareMatrix = types::DimSquareMatrix<State>;
        using MeasurementSquareMatrix = types::DimSquareMatrix<Measurement>;
        using AugmentedStateVec = types::Vector<AugmentedStateDim>;
        using AugmentedStateCovMatrix = types::SquareMatrix<AugmentedStateDim>;
        using SigmaPointsGen =
            AugmentedSigmaPointGenerator<AugmentedStateDim, StateDim>;
        static const types::DimensionType NumSigmaPoints =
            SigmaPointsGen::NumSigmaPoints;

        using TransformedSigmaPointMat =
            types::Matrix<MeasurementDim, NumSigmaPoints>;

        using Reconstruction =
            SigmaDistributionReconstruction<MeasurementDim, SigmaPointsGen>;

        using GainMatrix = types::Matrix<StateDim, MeasurementDim>;

        SigmaPointCorrectionApplication(State &s, Measurement &m,
                                        SigmaPointParameters params)
            : state(s), measurement(m),
              sigmaPoints(getAugmentedStateVec(s, m),
                          getAugmentedStateCov(s, m), params),
              transformedPoints(
                  transformSigmaPoints(state, measurement, sigmaPoints)),
              reconstruction(sigmaPoints, transformedPoints),
              innovationCovariance(computeInnovationCovariance(
                  state, measurement, reconstruction)),
              K(computeKalmanGain(innovationCovariance, reconstruction)),
              stateCorrection(K * reconstruction.getMean()) {}

        static AugmentedStateVec getAugmentedStateVec(State const &s,
                                                      Measurement const &m) {
            AugmentedStateVec ret;
            /// assuming measurement noise is zero mean
            ret << s.stateVector(), types::DimVector<Measurement>::Zero();
            return ret;
        }

        static AugmentedStateCovMatrix getAugmentedStateCov(State const &s,
                                                            Measurement &m) {
            AugmentedStateCovMatrix ret;
            ret << s.errorCovariance(),
                types::Matrix<StateDim, MeasurementDim>::Zero(),
                types::Matrix<MeasurementDim, StateDim>::Zero(),
                m.getCovariance(s);
            return ret;
        }

        /// Transforms sigma points by having the measurement compute the
        /// residual for a state whose state vector we update to each of the
        /// sigma points in turn.
        static TransformedSigmaPointMat
        transformSigmaPoints(State const &s, Measurement &m,
                             SigmaPointsGen const &sigmaPoints) {
            TransformedSigmaPointMat ret;
            State tempS = s;
            for (std::size_t i = 0; i < NumSigmaPoints; ++i) {
                tempS.setStateVector(sigmaPoints.getSigmaPoint(i));
                ret.col(i) = m.getResidual(tempS);
            }
            return ret;
        }

        static MeasurementSquareMatrix
        computeInnovationCovariance(State const &s, Measurement &m,
                                    Reconstruction const &recon) {
            return recon.getCov() + m.getCovariance(s);
        }

        static GainMatrix computeKalmanGain(MeasurementSquareMatrix const &Pvv,
                                            Reconstruction const &recon) {

            // Solve for K in K=Pxy Pvv^-1
            // where the cross-covariance matrix from the reconstruction is
            // transpose(Pxy) and Pvv is the reconstructed covariance plus the
            // measurement covariance
            // (Actually solves with transpose(Pvv) * transpose(K) =
            // transpose(Pxy) )
            GainMatrix ret = Pvv.transpose().ldlt().solve(recon.getCrossCov());
            return ret;
        }

        void finishCorrection() {
            state.setStateVector(state.stateVector() + stateCorrection);

            StateSquareMatrix newP = state.errorCovariance() -
                                     K * innovationCovariance * K.transpose();
            state.setErrorCovariance(newP);
            // Let the state do any cleanup it has to (like fixing externalized
            // quaternions)
            state.postCorrect();
        }

        State &state;
        Measurement &measurement;
        SigmaPointsGen sigmaPoints;
        TransformedSigmaPointMat transformedPoints;
        Reconstruction reconstruction;
        MeasurementSquareMatrix innovationCovariance;
        GainMatrix K;
        StateVec stateCorrection;
    };
    template <typename State, typename Measurement>
    inline SigmaPointCorrectionApplication<State, Measurement>
    beginUnscentedCorrection(State &s, Measurement &m,
                             SigmaPointParameters params) {
        return SigmaPointCorrectionApplication<State, Measurement>(s, m,
                                                                   params);
    }
} // namespace kalman
} // namespace osvr
#if 0
CATCH_TYPELIST_TESTCASE("unscented with identity calibration output",
                        kalman::IMUOrientationMeasForUnscented) {
#endif
TEST_CASE("unscented with identity calibration output") {
    using TypeParam = kalman::IMUOrientationMeasForUnscented;

    const auto params = kalman::SigmaPointParameters(3);
    // using MeasurementType = OrientationMeasurementUsingPolicy<TypeParam>;
    using MeasurementType = TypeParam;
    unique_ptr<TestData> data(new TestData);
    GIVEN("an identity state") {
        WHEN("filtering in an identity measurement") {
            Quaterniond xformedMeas = data->xform(Quaterniond::Identity());

            THEN("the transformed measurement should equal the measurement") {
                CAPTURE(xformedMeas);
                REQUIRE(xformedMeas.isApprox(Quaterniond::Identity()));

                MeasurementType kalmanMeas{xformedMeas, data->imuVariance};
                AND_THEN("residual should be zero") {
                    CAPTURE(kalmanMeas.getResidual(data->state));
                    REQUIRE(kalmanMeas.getResidual(data->state)
                                .isApproxToConstant(0.));
                }
                using StateDim =
                    kalman::types::Dimension<decltype(data->state)>;
                using MeasurementDim =
                    kalman::types::Dimension<MeasurementType>;
                static const auto m = MeasurementDim::value;
                static const auto n = StateDim::value;
                auto inProgress = kalman::beginUnscentedCorrection(
                    data->state, kalmanMeas, params);

                AND_THEN("transformed sigma points should not all equal 0") {
                    REQUIRE_FALSE(
                        inProgress.transformedPoints.isApproxToConstant(0.));
                }
                // kalman::SigmaDistributionReconstruction<MeasurementDim::value,
                // typename decltype(inProgress)::SigmaPointsGen>
                auto &recon = inProgress.reconstruction;
                AND_THEN("propagated mean residual should be zero") {
                    CAPTURE(recon.getMean().transpose());
                    REQUIRE(recon.getMean().isApproxToConstant(0.));
                }
                AND_THEN(
                    "state correction should be finite - specifically, zero") {
                    CAPTURE(inProgress.stateCorrection.transpose());
                    REQUIRE(inProgress.stateCorrection.array().allFinite());
                    REQUIRE(inProgress.stateCorrection.isApproxToConstant(0));
                }
            }
        }
    }
}