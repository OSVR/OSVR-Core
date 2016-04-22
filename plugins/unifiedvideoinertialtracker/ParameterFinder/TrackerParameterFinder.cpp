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
#include "ParamFindingRoutine.h"
#include "ParameterSets.h"
#include "TransformFindingRoutine.h"

// Library/third-party includes
#include <boost/algorithm/string/predicate.hpp> // for argument handling

// Standard includes
#include <iostream>

/// Define to add a "press enter to exit" thing at the end.
#undef PAUSE_BEFORE_EXIT

/// Adding a new entry here means you must also add it to RECOGNIZED_ROUTINES,
/// routineToString, and the switch in main().
enum class OptimizationRoutine {
    RefTracker,
    ParamViaRansac,
    ParamViaRefTracker,
    Unrecognized = -1
};

static const auto DEFAULT_ROUTINE = OptimizationRoutine::RefTracker;

static const auto RECOGNIZED_ROUTINES = {
    OptimizationRoutine::RefTracker, OptimizationRoutine::ParamViaRansac,
    OptimizationRoutine::ParamViaRefTracker};

const char *routineToString(OptimizationRoutine routine) {
    switch (routine) {
    case OptimizationRoutine::RefTracker:
        return "RefTracker";
        break;
    case OptimizationRoutine::ParamViaRansac:
        return "ParamViaRansac";
        break;
    case OptimizationRoutine::ParamViaRefTracker:
        return "ParamViaRefTracker";
        break;
    case OptimizationRoutine::Unrecognized:
    default:
        return "ERROR - UNRECOGNIZED";
        break;
    }
}

OptimizationRoutine stringToRoutine(const char *routineName) {
    for (auto &routine : RECOGNIZED_ROUTINES) {
        if (boost::iequals(routineToString(routine), routineName)) {
            return routine;
        }
    }
    return OptimizationRoutine::Unrecognized;
}

int usage(const char *argv0) {
    std::cerr << "Usage: " << argv0 << "[<routine>]\n" << std::endl;
    std::cerr
        << "where <routine> is one of the following (case insensitive): \n";
    for (auto &routine : RECOGNIZED_ROUTINES) {
        std::cerr << "   " << routineToString(routine) << "\n";
    }
    std::cerr
        << "\nIf no routine is explicitly specified, the default routine is "
        << routineToString(DEFAULT_ROUTINE) << "\n";
    std::cerr << "Too many arguments, or an unrecognized routine parameter "
                 "(including anything vaguely 'help-ish') will trigger this "
                 "message."
              << std::endl;
    return 1;
}

int main(int argc, char *argv[]) {
    OptimizationRoutine routine = DEFAULT_ROUTINE;
    static const auto DATAFILE = "augmented-blobs.csv";

    if (argc > 2) {
        /// Passed too many args: show help.
        return usage(argv[0]);
    }

    if (argc == 2) {
        routine = stringToRoutine(argv[1]);
        if (OptimizationRoutine::Unrecognized == routine) {
            std::cerr << "Didn't recognize '" << argv[1]
                      << "' as an optimization routine.\n"
                      << std::endl;
            /// Didn't recognize that.
            return usage(argv[0]);
        }
        /// Hey, we did recognize that!
        std::cout << "Will execute optimization routine "
                  << routineToString(routine)
                  << " as specified on the command line." << std::endl;
    } else {
        std::cout << "No optimization routine specified on the command line, "
                     "will execute "
                  << routineToString(routine) << " by default." << std::endl;
    }

    std::cout << "Loading and parsing data from " << DATAFILE << "    ";
    auto data = osvr::vbtracker::loadData(DATAFILE);
    std::cout << "\n";

    const auto camParams =
        osvr::vbtracker::getHDKCameraParameters().createUndistortedVariant();

    osvr::vbtracker::ConfigParams params;
    params.performingOptimization = true;
    params.silent = true;
    params.debug = false;

    params.offsetToCentroid = false;
    params.includeRearPanel = false;
    params.imu.path = "";
    params.imu.useOrientation = false;
    params.imu.useAngularVelocity = false;

    /// Which parameter set do we want to optimize in the main optimization
    /// routines?
    namespace ps = osvr::vbtracker::optimization_param_sets;

    using ParamSet = ps::ProcessNoiseVarianceAndDecay;
    // using ParamSet = ps::BrightAndNew;
    // using ParamSet = ps::HighResidual;

    std::cout << "Starting optimization routine " << routineToString(routine)
              << std::endl;
    switch (routine) {
    case OptimizationRoutine::RefTracker:
        /// Use optimizer to compute the transforms for the reference tracker,
        /// which is mounted effectively rigidly to the desired tracker, but in
        /// an unknown relative pose (and a different base coordinate system)
        osvr::vbtracker::computeRefTrackerTransform(
            data, osvr::vbtracker::OptimCommonData{camParams, params});
        break;

    case OptimizationRoutine::ParamViaRansac:

        osvr::vbtracker::runOptimizer<osvr::vbtracker::RansacOneEuro, ParamSet>(
            data, osvr::vbtracker::OptimCommonData{camParams, params}, 30);

        break;

    case OptimizationRoutine::ParamViaRefTracker:

        osvr::vbtracker::runOptimizer<osvr::vbtracker::ReferenceTracker,
                                      ParamSet>(
            data, osvr::vbtracker::OptimCommonData{camParams, params}, 300);

        break;

    default:
        assert(false && "Should not happen - only recognized routines should "
                        "make it this far!");
    }

#ifdef PAUSE_BEFORE_EXIT
    std::cout << "Press enter to exit." << std::endl;
    std::cin.ignore();
#endif
    return 0;
}
