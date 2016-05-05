/** @file
    @brief Header

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

#ifndef INCLUDED_TransformFindingRoutine_h_GUID_A09E7693_04E5_4FA2_C929_F23CBEC12306
#define INCLUDED_TransformFindingRoutine_h_GUID_A09E7693_04E5_4FA2_C929_F23CBEC12306

// Internal Includes
#include "OptimizationBase.h"
#include "UtilityFunctions.h"
#include "newuoa.h"

// Library/third-party includes
// - none

// Standard includes
#include <numeric>

namespace osvr {
namespace vbtracker {

    /// Optimization routine: compute the transform between the smoothed RANSAC
    /// results and the reference tracker.
    void computeRefTrackerTransform(MeasurementsRows const &data,
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

            std::cout << "Initial tracking complete: " << samples.size()
                      << " valid samples, " << ransacOneEuro.getNumFlips()
                      << " flipped." << std::endl;
        }

        if (samples.empty()) {

            std::cout << "No samples with pose?" << std::endl;
            return;
        }

        const auto numSamples = samples.size();
        const double numSamplesDouble = static_cast<double>(numSamples);

        auto simpleCost = [](Eigen::Isometry3d const &a,
                             Eigen::Isometry3d const &b,
                             double angleScale = 1) {
            auto angDistance =
                Eigen::Quaterniond(a.rotation())
                    .angularDistance(Eigen::Quaterniond(b.rotation()));
            auto linearDistance = (a.translation() - b.translation()).norm();
            return linearDistance + angleScale * angDistance;
        };

        auto maxRuns = 30000;
        std::cout << "Starting actual optimization procedures..." << std::endl;
        Vec<3> baseXlate = -Vec<3>(-0.316523, -0.740873, -2.0701);

        {
            std::cout << "Optimizing base translation" << std::endl;
            auto ret = ei_newuoa_wrapped(
                baseXlate, {1e-6, 1e-1}, 10000,
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
            std::cout << baseXlate.format(getFullFormat()) << "\n" << std::endl;
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
                baseXlateRot, {1e-6, 1e-1}, 10000,
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
            std::cout << baseXlateRot.format(getFullFormat()) << "\n"
                      << std::endl;
            std::cout << "First sample:\n";
            outputTransformedSample(
                makeIsometry(baseXlateRot.head<3>(),
                             rot_exp(baseXlateRot.tail<3>())),
                Eigen::Isometry3d::Identity(), *(samples[0]));
        }

        {
            using namespace reftracker;
            TransformParams x = TransformParams::Zero();
            x.head<3>() = baseXlateRot.head<3>();
            x.segment<3>(BaseRot) = baseXlateRot.tail<3>();

            std::cout << "Starting optimization procedure for full transform, "
                         "max runs = "
                      << maxRuns << std::endl;
            auto ret = ei_newuoa_wrapped(
                x, {1e-6, 1e-1}, maxRuns,
                [&](TransformParams const &paramVec) -> double {
                    Eigen::Isometry3d baseXform = getBaseTransform(paramVec);
                    Eigen::Isometry3d innerXform = getInnerTransform(paramVec);
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
                std::cout << x.format(getFullFormat()) << std::endl;

                Eigen::Isometry3d baseXform = getBaseTransform(x);
                Eigen::Isometry3d innerXform = getInnerTransform(x);
                std::cout << "The first three samples, transformed for your "
                             "viewing pleasure:\n";
                for (std::size_t i = 0; i < 3; ++i) {
                    outputTransformedSample(baseXform, innerXform,
                                            *(samples[i]));
                }
            }
        }
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_TransformFindingRoutine_h_GUID_A09E7693_04E5_4FA2_C929_F23CBEC12306
