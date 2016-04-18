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
    void runOptimizer(
        std::vector<std::unique_ptr<TimestampedMeasurements>> const &data,
        OptimCommonData const &commonData) {
        using ParamVec = Vec<3>;
        const double REALLY_BIG = 1000.;

        auto func = [&](ParamVec const &paramVec) -> double {
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
                /// Sometimes gets stuck in parameter ditches where we get very
                /// few tracked frames, and for some reason the above adjustment
                /// isn't working.
                auto effectiveCost = avgCost / samples;
                std::cout << "Dividing cost by number of tracked frames gives "
                             "effective cost of "
                          << effectiveCost << std::endl;
                return effectiveCost;
            }
            std::cout << "No samples with pose for both algorithms?"
                      << std::endl;
            return REALLY_BIG;
        };

        ParamVec x = {4.14e-6, 1e-2, 5e-2};
        auto ret = ei_newuoa_wrapped(x, {1e-16, 1e-1}, 25, func);
        std::cout << "Optimizer returned " << ret
                  << " and these parameter values:" << std::endl;
        Eigen::IOFormat format;
        format.precision = Eigen::FullPrecision;
        std::cout << x.transpose().format(format) << std::endl;
    }

} // namespace vbtracker
} // namespace osvr

int main() {
    // osvr::vbtracker::runOptimizer("augmented-blobs.csv");
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

    osvr::vbtracker::runOptimizer(
        data, osvr::vbtracker::OptimCommonData{camParams, params});

    std::cout << "Press enter to exit." << std::endl;
    std::cin.ignore();
    return 0;
}
