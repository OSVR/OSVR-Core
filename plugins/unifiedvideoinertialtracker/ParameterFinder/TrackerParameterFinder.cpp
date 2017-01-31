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

#include <osvr/TypePack/ForEachType.h>
#include <osvr/TypePack/Head.h>
#include <osvr/TypePack/List.h>

// Library/third-party includes
#include <boost/algorithm/string/predicate.hpp> // for argument handling

// Standard includes
#include <iostream>

/// Define to add a "press enter to exit" thing at the end.
#undef PAUSE_BEFORE_EXIT

/// Parameter sets for the routines that start with "Param"
namespace ps {
using namespace osvr::vbtracker::optimization_param_sets;
using ParamSets =
    osvr::typepack::list<ProcessNoiseAndDecay, ProcessNoiseVarianceAndDecay,
                         BrightAndNew, HighResidual, VariancePenalties>;
using DefaultParamSet = osvr::typepack::head<ps::ParamSets>;
} // namespace ps

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

struct PrintParamSetOptions {
    template <typename T> void operator()(T const &) {
        std::cerr
            << "   "
            << osvr::vbtracker::optimization_param_sets::ParamSetName<T>::get()
            << "\n";
    }
};

int usage(const char *argv0) {
    std::cerr << "Usage: " << argv0 << " [<routine> [<paramset> [--cost]]]\n"
              << std::endl;
    std::cerr
        << "where <routine> is one of the following (case insensitive): \n";
    for (auto &routine : RECOGNIZED_ROUTINES) {
        std::cerr << "   " << routineToString(routine) << "\n";
    }
    std::cerr
        << "\nThe various ParamViaX routines take an optional additional "
           "argument specifying the parameter set that they optimize, one "
           "of:\n";

    osvr::typepack::for_each_type<ps::ParamSets>(PrintParamSetOptions{});
    std::cerr << "as well as an additional optional switch, --cost, if you'd "
                 "like to just run the current parameters through and compute "
                 "the cost, rather than optimize.\n\n";
    std::cerr
        << "\nIf no routine is explicitly specified, the default routine is "
        << routineToString(DEFAULT_ROUTINE) << "\n";
    std::cerr << "Too many arguments, or an unrecognized routine parameter "
                 "(including anything vaguely 'help-ish') will trigger this "
                 "message."
              << std::endl;
    return 1;
}

template <typename RefSource> class ParseArgumentAsParamSet {
  public:
    ParseArgumentAsParamSet(osvr::vbtracker::ParamOptimizerFunc &outFunc,
                            std::string &paramSetName)
        : outFunc_(outFunc), paramSetName_(paramSetName) {}
    template <typename T> void operator()(T const &, const char *arg) {
        using osvr::vbtracker::optimization_param_sets::ParamSetName;
        if (boost::iequals(ParamSetName<T>::get(), arg)) {
            // we've got a match!
            outFunc_ = &osvr::vbtracker::runOptimizer<RefSource, T>;
            paramSetName_ = ParamSetName<T>::get();
        }
    }

  private:
    osvr::vbtracker::ParamOptimizerFunc &outFunc_;
    std::string &paramSetName_;
};

template <typename RefSource>
int parseParamSetForParamOptimizer(osvr::vbtracker::ParamOptimizerFunc &func,
                                   bool &costOnly, int argc, char *argv[]) {
    if (argc > 4) {
        std::cerr << "Too many command line arguments!" << std::endl;
        return usage(argv[0]);
    }
    if (argc > 3) {
        if (boost::iequals(argv[3], "--cost")) {
            std::cout << "Will run for just cost-only." << std::endl;
            costOnly = true;
        } else {
            std::cerr << "Too many command line arguments - didn't recognize "
                         "the last one!"
                      << std::endl;
            return usage(argv[0]);
        }
    }
    if (argc == 2) {
        // specified routine only, no param set
        std::cout << "Will process default parameter set "
                  << osvr::vbtracker::optimization_param_sets::ParamSetName<
                         ps::DefaultParamSet>::get()
                  << "\n";
        func = &osvr::vbtracker::runOptimizer<RefSource, ps::DefaultParamSet>;
        costOnly = false;
        return 0;
    }
    // OK, they specified a param set.
    osvr::vbtracker::ParamOptimizerFunc result;
    std::string paramSetName;
    ParseArgumentAsParamSet<RefSource> functor(result, paramSetName);
    osvr::typepack::for_each_type<ps::ParamSets>(functor, argv[2]);
    if (result) {
        std::cout << "Will process parameter set " << paramSetName
                  << " as specified on the command line.\n";
        func = result;
        return 0;
    }

    std::cerr << "Did not recognize " << argv[2]
              << " as one of the known parameter sets to optimize!"
              << std::endl;

    return usage(argv[0]);
}

int main(int argc, char *argv[]) {
    OptimizationRoutine routine = DEFAULT_ROUTINE;
    static const auto DATAFILE = "augmented-blobs.csv";

    auto withUsage = [&] { return usage(argv[0]); };
    auto tooManyArguments = [&] {
        std::cerr << "Too many command line arguments!" << std::endl;
        return withUsage();
    };

    if (argc > 1) {
        routine = stringToRoutine(argv[1]);
        if (OptimizationRoutine::Unrecognized == routine) {
            std::cerr << "Didn't recognize '" << argv[1]
                      << "' as an optimization routine.\n"
                      << std::endl;
            /// Didn't recognize that.
            return withUsage();
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

    bool costOnly = false;
    osvr::vbtracker::ParamOptimizerFunc paramOptFunc;
    {
        int ret = 0;
        switch (routine) {
        case OptimizationRoutine::ParamViaRansac:
            ret =
                parseParamSetForParamOptimizer<osvr::vbtracker::RansacOneEuro>(
                    paramOptFunc, costOnly, argc, argv);
            if (ret != 0) {
                /// There was an error, and the function already told the user
                /// about it.
                return ret;
            }
            break;

        case OptimizationRoutine::ParamViaRefTracker:

            ret = parseParamSetForParamOptimizer<
                osvr::vbtracker::ReferenceTracker>(paramOptFunc, costOnly, argc,
                                                   argv);
            if (ret != 0) {
                /// There was an error, and the function already told the user
                /// about it.
                return ret;
            }
            break;

        case OptimizationRoutine::RefTracker:
        /// no param set here
        default:
            if (argc > 2) {

                std::cerr << "Too many command line arguments!" << std::endl;
                return withUsage();
            }
        }
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

        paramOptFunc(data, costOnly,
                     osvr::vbtracker::OptimCommonData{camParams, params}, 30);
        break;

    case OptimizationRoutine::ParamViaRefTracker:

        paramOptFunc(data, costOnly,
                     osvr::vbtracker::OptimCommonData{camParams, params}, 300);
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
