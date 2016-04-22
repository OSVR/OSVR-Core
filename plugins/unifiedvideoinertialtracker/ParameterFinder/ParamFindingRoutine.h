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

#ifndef INCLUDED_ParamFindingRoutine_h_GUID_C2088279_D54B_4D8B_562E_5748C748DAD0
#define INCLUDED_ParamFindingRoutine_h_GUID_C2088279_D54B_4D8B_562E_5748C748DAD0

// Internal Includes
#include "OptimizationBase.h"
#include "UtilityFunctions.h"
#include "newuoa.h"

// Library/third-party includes
// - none

// Standard includes
#include <iomanip>

namespace osvr {
namespace vbtracker {

    /// The main optimization routine, in which we run the tracker repeatedly
    /// with different parameters and compare its results at each step to some
    /// source of reference data.
    template <typename TrackingReferenceType, typename ParamSet>
    void runOptimizer(MeasurementsRows const &data,
                      OptimCommonData const &commonData, std::size_t maxRuns) {

        std::cout << "Max runs: " << maxRuns << std::endl;

        /// Set up initial values for the vector we'll optimize - load them from
        /// the config params.
        using ParamVec = Vec<ParamSet::Dimension>;
        ParamVec x = ParamSet::getInitialVec(commonData);

        std::cout << "Optimizing, respectively: "
                  << ParamSet::getVecElementNames() << "\n";
        std::cout << "Initial vector:\n"
                  << x.format(getFullFormat()) << std::endl;

        auto ret = ei_newuoa_wrapped(
            x, ParamSet::getRho(), static_cast<long>(maxRuns),
            [&](ParamVec const &paramVec) -> double {
                ConfigParams params = commonData.initialParams;

                /// Update config from provided param vec
                ParamSet::updateParamsFromVec(params, paramVec);

                auto optim = OptimData::make(params, commonData);

                MainAlgoUnderStudy mainAlgo;
                TrackingReferenceType ref;
                std::size_t samples = 0;
                double accum = 0;

                /// Main algorithm loop
                for (auto const &rowPtr : data) {
                    mainAlgo(optim, *rowPtr);
                    ref(optim, *rowPtr);
                    if (ref.havePose() && mainAlgo.havePose()) {
                        auto cost =
                            costMeasurement(ref.getPose(), mainAlgo.getPose());
                        accum += cost;
                        samples++;
                    }
                }

                /// Cost accumulation/post-processing.
                if (samples > 0) {
                    auto avgCost = (accum / static_cast<double>(samples));
                    auto numResets = mainAlgo.getNumResets(optim);
                    /// Sometimes gets stuck in parameter ditches where we get
                    /// very few tracked frames
                    auto effectiveCost = avgCost * numResets / samples;
                    std::cout
                        << std::setw(15) << std::to_string(effectiveCost)
                        << " effective cost (average cost of " << std::setw(9)
                        << avgCost << " over " << std::setw(4) << samples
                        << " eligible frames with " << std::setw(2) << numResets
                        << " resets)\n";
                    return effectiveCost;
                }
                std::cout << "No samples with pose for both algorithms?"
                          << std::endl;
                return getReallyBigCost();
            });
        std::cout << "Optimizer returned " << ret
                  << " and these parameter values:" << std::endl;
        std::cout << x.format(getFullFormat()) << std::endl;
        std::cout << "for parameters described as, respectively,\n"
                  << ParamSet::getVecElementNames() << std::endl;
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ParamFindingRoutine_h_GUID_C2088279_D54B_4D8B_562E_5748C748DAD0
