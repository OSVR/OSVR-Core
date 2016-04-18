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
#include "LoadRows.h"
#include "PoseFilter.h"
#include "newuoa.h"

#include <ConfigParams.h>
#include <MakeHDKTrackingSystem.h>
#include <TrackedBodyTarget.h>

#include <osvr/Util/EigenQuatExponentialMap.h>

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
#include <iostream>

template <std::size_t N> using Vec = Eigen::Matrix<double, N, 1>;

namespace osvr {
namespace vbtracker {
    struct OptimCommonData {
        CameraParameters const &camParams;
        ConfigParams const &initialParams;
    };
    class OptimData {
      public:
        OptimData(OptimData const &) = delete;
        OptimData(OptimData &&) = default;
        OptimData &operator=(OptimData const &) = delete;

        static OptimData make(ConfigParams const &params,
                              OptimCommonData const &commonData) {

            auto system = makeHDKTrackingSystem(params);
            /// @todo this shouldn't be required if we don't have an IMU?
            system->setCameraPose(Eigen::Isometry3d::Identity());
            auto &body = system->getBody(BodyId(0));
            auto &target = *(body.getTarget(TargetId(0)));
            return OptimData(std::move(system), body, target,
                             commonData.camParams);
        }
        static OptimData make(OptimCommonData const &commonData) {
            return OptimData::make(commonData.initialParams, commonData);
        }

        TrackingSystem &getSystem() { return *system_; }
        TrackedBody &getBody() { return *body_; }
        TrackedBodyTarget &getTarget() { return *target_; }
        CameraParameters const &getCamParams() { return camParams_; }

      private:
        OptimData(std::unique_ptr<TrackingSystem> &&system, TrackedBody &body,
                  TrackedBodyTarget &target, CameraParameters const &camParams)
            : camParams_(camParams), system_(std::move(system)), body_(&body),
              target_(&target) {}

        CameraParameters const &camParams_;
        std::unique_ptr<TrackingSystem> system_;
        TrackedBody *body_ = nullptr;
        TrackedBodyTarget *target_ = nullptr;
    };

    class FeedDataWithoutProcessing {
      public:
        void operator()(OptimData &optim, TimestampedMeasurements const &row) {
            auto inputData =
                makeImageOutputDataFromRow(row, optim.getCamParams());
            optim.getSystem().updateLedsFromVideoData(std::move(inputData));
        }
    };
    class MainAlgoUnderStudy {
      public:
        void operator()(OptimData &optim, TimestampedMeasurements const &row) {
            auto inputData =
                makeImageOutputDataFromRow(row, optim.getCamParams());
            auto indices = optim.getSystem().updateBodiesFromVideoData(
                std::move(inputData));
            gotPose = optim.getBody().hasPoseEstimate();
            if (gotPose) {
                pose = optim.getBody().getState().getIsometry();
            }
        }
        bool havePose() const { return gotPose; }
        Eigen::Isometry3d const &getPose() const { return pose; }

      private:
        bool gotPose = false;
        Eigen::Isometry3d pose;
    };

    class RansacOneEuro {
      public:
        void operator()(OptimData &optim, TimestampedMeasurements const &row) {
            gotPose = false;
            Eigen::Vector3d pos;
            Eigen::Quaterniond quat;
            auto gotRansac =
                optim.getTarget().uncalibratedRANSACPoseEstimateFromLeds(
                    optim.getCamParams(), pos, quat);
            if (gotRansac) {
                double dt = 1;
                if (isFirst) {
                    isFirst = false;
                } else {
                    dt = osvrTimeValueDurationSeconds(&row.tv, &last);
                }
                ransacPoseFilter.filter(dt, pos, quat);
                last = row.tv;
                gotPose = true;
                pose = ransacPoseFilter.getIsometry();
            }
        }
        bool havePose() const { return gotPose; }
        Eigen::Isometry3d const &getPose() const { return pose; }

      private:
        PoseFilter ransacPoseFilter;
        TimeValue last;
        bool isFirst = true;
        bool gotPose = false;
        Eigen::Isometry3d pose;
    };

    double costMeasurement(Eigen::Isometry3d const &refPose,
                           Eigen::Isometry3d const &expPose) {
        auto distanceAway = -1.;
        /// Arbitrary triangle facing the tracked object, point up, 1 meter
        /// away, per Welch appendix E
        using Point = std::array<double, 3>;
        auto corners = {Point{.2, -.2, distanceAway},
                        Point{-.2, -.2, distanceAway},
                        Point{0, .4, distanceAway}};
        double accum = 0;
        for (auto &corner : corners) {
            Eigen::Vector3d pt = Eigen::Vector3d::Map(corner.data());
            accum += ((refPose * pt) - (expPose * pt)).norm();
        }
        return accum / 3.;
    }
} // namespace vbtracker
} // namespace osvr

namespace osvr {
namespace vbtracker {
    template <typename Vec>
    inline Eigen::Quaterniond yawPitchRoll(Vec const &v) {
        Eigen::Quaterniond rotation =
            Eigen::AngleAxisd(v[2], Eigen::Vector3d::UnitZ()) *
            Eigen::AngleAxisd(v[1], Eigen::Vector3d::UnitX()) *
            Eigen::AngleAxisd(v[0], Eigen::Vector3d::UnitY());
        return rotation.normalized();
    }

    template <typename Vec> inline Eigen::Quaterniond euler(Vec const &v) {
        Eigen::Quaterniond rotation =
            Eigen::AngleAxisd(v[2], Eigen::Vector3d::UnitZ()) *
            Eigen::AngleAxisd(v[1], Eigen::Vector3d::UnitY()) *
            Eigen::AngleAxisd(v[0], Eigen::Vector3d::UnitZ());
        return rotation.normalized();
    }

    inline Eigen::Quaterniond yawPitchRoll(double y, double p, double r) {
        Eigen::Quaterniond rotation =
            Eigen::AngleAxisd(r, Eigen::Vector3d::UnitZ()) *
            Eigen::AngleAxisd(p, Eigen::Vector3d::UnitX()) *
            Eigen::AngleAxisd(y, Eigen::Vector3d::UnitY());
        return rotation.normalized();
    }

    inline Eigen::Isometry3d makeIsometry(Eigen::Vector3d const &xlate,
                                          Eigen::Quaterniond const &quat) {
        return Eigen::Isometry3d(Eigen::Translation3d(xlate)) *
               Eigen::Isometry3d(quat);
    }
    inline Eigen::Isometry3d makeIsometry(Eigen::Translation3d const &xlate,
                                          Eigen::Quaterniond const &quat) {
        return Eigen::Isometry3d(xlate) * Eigen::Isometry3d(quat);
    }
#if 0
    inline Eigen::Isometry3d
    makeIsometry(Eigen::Ref<Eigen::Vector3d const> const &xlate,
                 Eigen::Quaterniond const &quat) {
        return Eigen::Isometry3d(Eigen::Translation3d(xlate)) *
               Eigen::Isometry3d(quat);
    }
    inline Eigen::Isometry3d
    makeIsometry(Eigen::Ref<Eigen::Vector3d const> const &xlate) {
        return Eigen::Isometry3d(Eigen::Translation3d(xlate));
    }
#endif
    void computeRefTrackerTransform(
        std::vector<std::unique_ptr<TimestampedMeasurements>> const &data,
        OptimCommonData const &commonData) {

        /// Just run the tracking algo once - we can re-run the transform and
        /// comparison quicker for the optimizer.
        struct TrackedData {
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            Eigen::Isometry3d videoPose;
            Eigen::Isometry3d refPose;
        };
        using TrackedDataPtr = std::unique_ptr<TrackedData>;
        using TrackedSamples = std::vector<TrackedDataPtr>;

        TrackedSamples samples;
        {

            std::cout << "Starting processing tracking..." << std::endl;
            auto optim = OptimData::make(commonData);
            FeedDataWithoutProcessing mainAlgo;
            RansacOneEuro ransacOneEuro;

            /// Main algorithm loop
            for (auto const &rowPtr : data) {
                // even though we aren't using the results, we have to
                // include this first to update the LEDs.
                mainAlgo(optim, *rowPtr);
                ransacOneEuro(optim, *rowPtr);
                if (ransacOneEuro.havePose()) {
                    TrackedDataPtr newSample(new TrackedData);
                    newSample->videoPose = ransacOneEuro.getPose();
                    newSample->refPose =
                        makeIsometry(rowPtr->xlate, rowPtr->rot);
                    samples.emplace_back(std::move(newSample));
                }
            }

            std::cout << "Initial tracking complete." << std::endl;
        }

        if (samples.empty()) {

            std::cout << "No samples with pose?" << std::endl;
            return;
        }

        using ParamVec = Vec<12>;
        const auto numSamples = samples.size();
        const double numSamplesDouble = static_cast<double>(numSamples);
        static const double LARGE_VALUE = 100000;

        /// Indices into the array.
        enum {
            BaseXlate = 0,
            BaseRot = BaseXlate + 3,
            InnerXlate = BaseRot + 3,
            InnerRot = InnerXlate + 3
            // InnerRot = BaseRot + 3
        };

        ParamVec x = ParamVec::Zero();
        x.head<3>() << -0.316523, -0.740873, -2.0701;
        // x.segment<3>(InnerXlate) << 0, .2, .2;

        auto getBaseTranslation = [](ParamVec const &vec) {
            return Eigen::Translation3d(vec.head<3>());
        };
        auto getBaseRotation = [](ParamVec const &vec) {
            // return yawPitchRoll(vec[BaseRot], 0, 0);
            // return euler(vec.segment<3>(BaseRot));
            Eigen::Vector3d v = vec.segment<3>(BaseRot);
            return util::quat_exp_map(v).exp();
        };
#if 1
        auto getInnerTranslation = [](ParamVec const &vec) {
            return Eigen::Translation3d(vec.segment<3>(InnerXlate));
        };
#else

        auto getInnerTranslation = [](ParamVec const &vec) {
            return Eigen::Translation3d(Eigen::Vector3d(0, 0.2, 0.2));
        };
#endif
        auto getInnerRotation = [](ParamVec const &vec) {
            Eigen::Vector3d v = vec.segment<3>(InnerRot);
            return util::quat_exp_map(v).exp();
            // return euler(vec.segment<3>(InnerRot));
            // return yawPitchRoll(0, vec[InnerRot], vec[InnerRot + 1]);
        };

        auto simpleCost = [](Eigen::Isometry3d const &a,
                             Eigen::Isometry3d const &b) {
            auto angDistance =
                Eigen::Quaterniond(a.rotation())
                    .angularDistance(Eigen::Quaterniond(b.rotation()));
            auto linearDistance = (a.translation() - b.translation()).norm();
            return linearDistance + 10 * angDistance;
        };

        auto maxRuns = 30000;
        std::cout << "Starting actual optimization procedure, max runs = "
                  << maxRuns << std::endl;
        auto ret = ei_newuoa_wrapped(
            x, {1e-8, 1e-2}, maxRuns, [&](ParamVec const &paramVec) -> double {

                auto baseRot = getBaseRotation(paramVec);
                auto baseXlate = getBaseTranslation(paramVec);

                auto innerRot = getInnerRotation(paramVec);

                auto innerXlate = getInnerTranslation(paramVec);
#if 0
				/// angles for base - should not wrap.
				if (std::abs(Eigen::AngleAxisd(baseRot).angle()) >= (M_PI)) {
					return LARGE_VALUE;
				}
				/// angles for head - should be small.
				if (std::abs(Eigen::AngleAxisd(innerRot).angle()) >=
					(M_PI / 2)) {
					return LARGE_VALUE;
				}
				/// distances for head - should be on the order of 30cm.
				if ((innerXlate.vector().array().abs() > Eigen::Array3d::Constant(0.3)).any()) {

					return LARGE_VALUE;
				}
#endif

                // std::cout << x.transpose();

                Eigen::Isometry3d baseXform = makeIsometry(baseXlate, baseRot);
                Eigen::Isometry3d innerXform =
                    makeIsometry(innerXlate, innerRot);
                double accum = 0;
                for (auto const &samplePtr : samples) {
                    auto cost = simpleCost(samplePtr->videoPose * innerXform,
                                           baseXform * samplePtr->refPose);
                    accum += (cost / numSamplesDouble);
                }

                /// Cost accumulation/post-processing.
                // std::cout << " =  " << accum << std::endl;
                return accum;
            });
        std::cout << "Optimizer returned " << ret
                  << " and these parameter values:" << std::endl;
        Eigen::IOFormat format;
        format.precision = Eigen::FullPrecision;
        std::cout << x.format(format) << std::endl;
        {
            auto baseRot = getBaseRotation(x);
            auto baseXlate = getBaseTranslation(x);
            auto innerRot = getInnerRotation(x);
            auto innerXlate = getInnerTranslation(x);

            Eigen::Isometry3d baseXform = makeIsometry(baseXlate, baseRot);
            Eigen::Isometry3d innerXform = makeIsometry(innerXlate, innerRot);
            Eigen::Isometry3d invInner = innerXform.inverse();
            for (std::size_t i = 0; i < 3; ++i) {
                auto const &samplePtr = samples[i];
                std::cout << "HDK to VideoBase: "
                          << (samplePtr->videoPose).translation().transpose()
                          << std::endl;
                std::cout << "Vive to VideoBase: "
                          << (baseXform * samplePtr->refPose)
                                 .translation()
                                 .transpose()
                          << std::endl;
                std::cout << "HDK in Vive to VideoBase: "
                          << (samplePtr->videoPose * innerXform)
                                 .translation()
                                 .transpose()
                          << std::endl;
                std::cout << "Ref to VideoBase: "
                          << (baseXform * samplePtr->refPose * invInner)
                                 .translation()
                                 .transpose()
                          << std::endl;
                std::cout << "---------" << std::endl;
            }
        }
    }

    void runOptimizer(
        std::vector<std::unique_ptr<TimestampedMeasurements>> const &data,
        OptimCommonData const &commonData) {
        using ParamVec = Vec<3>;
        const double REALLY_BIG = 1000.;

        ParamVec x = {4.14e-6, 1e-2, 5e-2};
        auto ret = ei_newuoa_wrapped(
            x, {1e-16, 1e-1}, 25, [&](ParamVec const &paramVec) -> double {
                ConfigParams params = commonData.initialParams;

                /// Update config from provided param vec
                /// positional noise
                params.processNoiseAutocorrelation[0] =
                    params.processNoiseAutocorrelation[1] =
                        params.processNoiseAutocorrelation[2] = paramVec[0];
                /// rotational noise
                params.processNoiseAutocorrelation[3] =
                    params.processNoiseAutocorrelation[4] =
                        params.processNoiseAutocorrelation[5] = paramVec[1];

                params.measurementVarianceScaleFactor = paramVec[2];

                auto optim = OptimData::make(params, commonData);

                MainAlgoUnderStudy mainAlgo;
                RansacOneEuro ransacOneEuro;
                std::size_t samples = 0;
                double accum = 0;
                std::cout << "Starting processing data rows..." << std::endl;

                /// Main algorithm loop
                for (auto const &rowPtr : data) {
                    mainAlgo(optim, *rowPtr);
                    ransacOneEuro(optim, *rowPtr);
                    if (ransacOneEuro.havePose() && mainAlgo.havePose()) {
                        auto cost = costMeasurement(ransacOneEuro.getPose(),
                                                    mainAlgo.getPose());
                        accum += cost;
                        samples++;
                    }
                }

                /// Cost accumulation/post-processing.
                if (samples > 0) {
                    auto avgCost = (accum / static_cast<double>(samples));
                    std::cout << "Overall average cost: " << avgCost << " over "
                              << samples << " eligible frames " << std::endl;
                    /// Sometimes gets stuck in parameter ditches where we get
                    /// very few tracked frames, and for some reason the above
                    /// adjustment isn't working.
                    auto effectiveCost = avgCost / samples;
                    std::cout
                        << "Dividing cost by number of tracked frames gives "
                           "effective cost of "
                        << effectiveCost << std::endl;
                    return effectiveCost;
                }
                std::cout << "No samples with pose for both algorithms?"
                          << std::endl;
                return REALLY_BIG;
            });
        std::cout << "Optimizer returned " << ret
                  << " and these parameter values:" << std::endl;
        Eigen::IOFormat format;
        format.precision = Eigen::FullPrecision;
        std::cout << x.transpose().format(format) << std::endl;
    }

} // namespace vbtracker
} // namespace osvr

int main() {
    auto data = osvr::vbtracker::loadData("augmented-blobs.csv");
    const auto camParams =
        osvr::vbtracker::getHDKCameraParameters().createUndistortedVariant();

    osvr::vbtracker::ConfigParams params;
    params.performingOptimization = true;
    params.highResidualVariancePenalty = 15;
    params.initialBeaconError = 1e-16;
    params.beaconProcessNoise = 0;
    params.shouldSkipBrightLeds = true;
    params.brightLedVariancePenalty = 16;
    params.offsetToCentroid = false;
    params.manualBeaconOffset[0] = params.manualBeaconOffset[1] =
        params.manualBeaconOffset[2] = 0;
    params.linearVelocityDecayCoefficient = 1;
    params.angularVelocityDecayCoefficient = 1;
    params.debug = false;
    params.imu.path = "";

#if 0
    osvr::vbtracker::runOptimizer(
        data, osvr::vbtracker::OptimCommonData{camParams, params});
#endif

    /// Use optimizer to compute the transforms for the reference tracker, which
    /// is mounted effectively rigidly to the desired tracker, but in an unknown
    /// relative pose (and a different base coordinate system)
    osvr::vbtracker::computeRefTrackerTransform(
        data, osvr::vbtracker::OptimCommonData{camParams, params});

    std::cout << "Press enter to exit." << std::endl;
    std::cin.ignore();
    return 0;
}
