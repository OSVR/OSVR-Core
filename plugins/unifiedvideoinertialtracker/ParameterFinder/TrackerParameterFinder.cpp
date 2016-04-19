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
#include <algorithm>
#include <iostream>
#include <numeric>

template <std::size_t N> using Vec = Eigen::Matrix<double, N, 1>;

namespace osvr {
namespace vbtracker {

    template<typename T>
    inline T clamp(T minVal, T maxVal, T v) {
        return std::max(minVal, std::min(maxVal, v));
    }

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

    template <typename VecType>
    inline Eigen::Quaterniond rot_exp(VecType const &v) {
        Eigen::Vector3d vec = v;
        return util::quat_exp_map(vec).exp();
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

    inline Eigen::Isometry3d
    makeIsometry(Eigen::Ref<Eigen::Vector3d const> const &xlate) {
        return Eigen::Isometry3d(Eigen::Translation3d(xlate));
    }

    struct TrackedData {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        Eigen::Isometry3d videoPose;
        Eigen::Isometry3d refPose;
    };
    using TrackedDataPtr = std::unique_ptr<TrackedData>;
    using TrackedSamples = std::vector<TrackedDataPtr>;

    inline void outputTransformedSample(Eigen::Isometry3d const &baseXform,
                                        Eigen::Isometry3d const &innerXform,
                                        TrackedData const &sample) {
        std::cout << "HDK to VideoBase: "
                  << (sample.videoPose).translation().transpose() << std::endl;
        std::cout << "Vive to VideoBase: "
                  << (baseXform * sample.refPose).translation().transpose()
                  << std::endl;
        std::cout << "Ref to VideoBase: "
                  << (baseXform * sample.refPose * innerXform)
                         .translation()
                         .transpose()
                  << std::endl;
        std::cout << "---------" << std::endl;
    }
    void computeRefTrackerTransform(
        std::vector<std::unique_ptr<TimestampedMeasurements>> const &data,
        OptimCommonData const &commonData) {

        /// Just run the tracking algo once - we can re-run the transform and
        /// comparison quicker for the optimizer.

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

        const auto numSamples = samples.size();
        const double numSamplesDouble = static_cast<double>(numSamples);
        static const double LARGE_VALUE = 100000;

        auto simpleCost = [](Eigen::Isometry3d const &a,
                             Eigen::Isometry3d const &b,
                             double angleScale = 1) {
            auto angDistance =
                Eigen::Quaterniond(a.rotation())
                    .angularDistance(Eigen::Quaterniond(b.rotation()));
            auto linearDistance = (a.translation() - b.translation()).norm();
            return linearDistance + angleScale * angDistance;
        };

        Eigen::IOFormat format;
        format.precision = Eigen::FullPrecision;

        auto maxRuns = 30000;
        std::cout << "Starting actual optimization procedures..." << std::endl;
        Vec<3> baseXlate = -Vec<3>(-0.316523, -0.740873, -2.0701);

        {
            std::cout << "Optimizing base translation" << std::endl;
            auto ret = ei_newuoa_wrapped(
                baseXlate, {1e-8, 1e-2}, 10000,
                [&](Vec<3> const &vec) -> double {
                    Eigen::Isometry3d baseXform = makeIsometry(vec);
                    /// Accumulate the cost of all the samples
                    return std::accumulate(
                        samples.begin(), samples.end(), 0.0,
                        [&](double prev, TrackedDataPtr const &s) {
                            /// Don't include angular cost.
                            auto cost = simpleCost(s->videoPose,
                                                   baseXform * s->refPose, 0);
                            return prev + (cost / numSamplesDouble);
                        });
                });

            std::cout << "Result: cost " << ret << std::endl;
            std::cout << baseXlate.format(format) << "\n" << std::endl;
            std::cout << "First sample:\n";
            outputTransformedSample(makeIsometry(baseXlate),
                                    Eigen::Isometry3d::Identity(),
                                    *(samples[0]));
        }
        Vec<6> baseXlateRot = Vec<6>::Zero();
        baseXlateRot.head<3>() = baseXlate;
        {
            std::cout << "\nOptimizing base transform, max runs = " << maxRuns
                      << std::endl;
            auto ret = ei_newuoa_wrapped(
                baseXlateRot, {1e-8, 1e-2}, 10000,
                [&](Vec<6> const &vec) -> double {
                    Eigen::Isometry3d baseXform =
                        makeIsometry(vec.head<3>(), rot_exp(vec.tail<3>()));
                    /// Accumulate the cost of all the samples
                    return std::accumulate(
                        samples.begin(), samples.end(), 0.0,
                        [&](double prev, TrackedDataPtr const &s) {
                            auto cost = simpleCost(s->videoPose,
                                                   baseXform * s->refPose);
                            return prev + (cost / numSamplesDouble);
                        });
                });
            std::cout << "Result: cost " << ret << std::endl;
            std::cout << baseXlateRot.format(format) << "\n" << std::endl;
            std::cout << "First sample:\n";
            outputTransformedSample(
                makeIsometry(baseXlateRot.head<3>(),
                             rot_exp(baseXlateRot.tail<3>())),
                Eigen::Isometry3d::Identity(), *(samples[0]));
        }

        {
            /// Indices into the array.
            enum {
                BaseXlate = 0,
                BaseRot = BaseXlate + 3,
                InnerXlate = BaseRot + 3,
                InnerRot = InnerXlate + 3
            };

            using ParamVec = Vec<12>;
            ParamVec x = ParamVec::Zero();
            x.head<3>() = baseXlateRot.head<3>();
            x.segment<3>(BaseRot) = baseXlateRot.tail<3>();

            auto getBaseTranslation = [](ParamVec const &vec) {
                return Eigen::Translation3d(vec.head<3>());
            };
            auto getBaseRotation = [](ParamVec const &vec) {
                return rot_exp(vec.segment<3>(BaseRot));
            };
            auto getInnerTranslation = [](ParamVec const &vec) {
                return Eigen::Translation3d(vec.segment<3>(InnerXlate));
            };
            auto getInnerRotation = [](ParamVec const &vec) {
                return rot_exp(vec.segment<3>(InnerRot));
            };

            std::cout << "Starting optimization procedure for full transform, "
                         "max runs = "
                      << maxRuns << std::endl;
            auto ret = ei_newuoa_wrapped(
                x, {1e-8, 1e-2}, maxRuns,
                [&](ParamVec const &paramVec) -> double {
                    Eigen::Isometry3d baseXform =
                        makeIsometry(getBaseTranslation(paramVec),
                                     getBaseRotation(paramVec));
                    Eigen::Isometry3d innerXform =
                        makeIsometry(getInnerTranslation(paramVec),
                                     getInnerRotation(paramVec));
                    /// Accumulate the cost of all the samples
                    return std::accumulate(
                        samples.begin(), samples.end(), 0.0,
                        [&](double prev, TrackedDataPtr const &s) {
                            auto cost = costMeasurement(s->videoPose,
                                                        baseXform * s->refPose *
                                                            innerXform);
                            return prev + (cost / numSamplesDouble);
                        });
                });
            {
                std::cout << "Optimizer returned " << ret
                          << " and these parameter values:" << std::endl;
                std::cout << x.format(format) << std::endl;
                auto baseRot = getBaseRotation(x);
                auto baseXlate = getBaseTranslation(x);
                auto innerRot = getInnerRotation(x);
                auto innerXlate = getInnerTranslation(x);

                Eigen::Isometry3d baseXform = makeIsometry(baseXlate, baseRot);
                Eigen::Isometry3d innerXform =
                    makeIsometry(innerXlate, innerRot);
                for (std::size_t i = 0; i < 3; ++i) {
                    outputTransformedSample(baseXform, innerXform,
                                            *(samples[i]));
                }
            }
        }
    }

    void runOptimizer(
        std::vector<std::unique_ptr<TimestampedMeasurements>> const &data,
        OptimCommonData const &commonData) {
        using ParamVec = Vec<2>;
        const double REALLY_BIG = 1000.;

        ParamVec x = {0.04348175147568786, 0.07100278320909659};
        auto ret = ei_newuoa_wrapped(
            x, {1e-16, 1e-1}, 10, [&](ParamVec const &paramVec) -> double {
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

                //params.beaconProcessNoise = paramVec[2];
                //params.measurementVarianceScaleFactor = paramVec[2];

                auto optim = OptimData::make(params, commonData);

                MainAlgoUnderStudy mainAlgo;
                RansacOneEuro ransacOneEuro;
                std::size_t samples = 0;
                double accum = 0;
                // std::cout << "Starting processing data rows..." << std::endl;

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
                    /// Sometimes gets stuck in parameter ditches where we get
                    /// very few tracked frames
                    auto effectiveCost = avgCost / samples;
#if 0
                    std::cout << "Overall average cost: " << avgCost << " over "
                              << samples << " eligible frames, " << std::endl;
                    std::cout
                        << "Dividing cost by number of tracked frames gives "
                           "effective cost of "
                        << effectiveCost << std::endl;
#else
                    std::cout << effectiveCost
                              << "\t effective cost (average cost of "
                              << avgCost << " over " << samples
                              << " eligible frames)\n";
#endif
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
    params.silent = true;

    params.highResidualVariancePenalty = 15;
    params.initialBeaconError = 1e-16;
    params.beaconProcessNoise = 1.e-21;
    params.shouldSkipBrightLeds = true;
    params.measurementVarianceScaleFactor = 0.03020921164465682;
    //params.brightLedVariancePenalty = 16;
    params.offsetToCentroid = false;
    params.manualBeaconOffset[0] = params.manualBeaconOffset[1] =
        params.manualBeaconOffset[2] = 0;
    //params.linearVelocityDecayCoefficient = 0.9;
    //params.angularVelocityDecayCoefficient = 1;
    params.debug = false;
    params.imu.path = "";

#if 1
    osvr::vbtracker::runOptimizer(
        data, osvr::vbtracker::OptimCommonData{camParams, params});
#else

    /// Use optimizer to compute the transforms for the reference tracker, which
    /// is mounted effectively rigidly to the desired tracker, but in an unknown
    /// relative pose (and a different base coordinate system)
    osvr::vbtracker::computeRefTrackerTransform(
        data, osvr::vbtracker::OptimCommonData{camParams, params});
#endif
    std::cout << "Press enter to exit." << std::endl;
    std::cin.ignore();
    return 0;
}
