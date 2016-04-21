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
#include "CSVTools.h"
#include "ConfigParams.h"
#include "MakeHDKTrackingSystem.h"
#include "TrackedBodyTarget.h"
#include "newuoa.h"

// Library/third-party includes
#include <Eigen/Core>

// Standard includes
// - none

/// Friendlier wrapper around newuoa
template <typename Function, typename Vec>
inline double ei_newuoa(long npt, Vec &x, std::pair<double, double> rho,
                        long maxfun, Function &&f) {

    double rhoBeg, rhoEnd;
    std::tie(rhoBeg, rhoEnd) = rho;
    if (rhoEnd > rhoBeg) {
        std::swap(rhoBeg, rhoEnd);
    }
    long n = x.size();
    auto workingSpaceNeeded = (npt + 13) * (npt + n) + 3 * n * (n + 3) / 2;
    Eigen::VectorXd workingSpace(workingSpaceNeeded);
    return newuoa(std::forward<Function>(f), n, npt, x.data(), rhoBeg, rhoEnd,
                  maxfun, workingSpace.data());
}

using ParamVec = Eigen::Vector4d;

void updateConfigFromVec(osvr::vbtracker::ConfigParams &params,
                         ParamVec const &paramVec) {
    /// positional noise
    params.processNoiseAutocorrelation[0] =
        params.processNoiseAutocorrelation[1] =
            params.processNoiseAutocorrelation[2] = paramVec[0];
    /// rotational noise
    params.processNoiseAutocorrelation[3] =
        params.processNoiseAutocorrelation[4] =
            params.processNoiseAutocorrelation[5] = paramVec[1];

    params.beaconProcessNoise = paramVec[2];

    params.measurementVarianceScaleFactor = paramVec[3];
}

double runTracker(long n, double *x) {
    osvr::vbtracker::ConfigParams params;
    updateConfigFromVec(params, ParamVec::Map(x));
}

int main() {
    // initial values.
    ParamVec x = {4.14e-6, 1e-2, 0, 5e-2};
    auto npt = x.size() * 2; // who knows?

    auto ret =
        ei_newuoa(npt, x, {1e-8, 1e-4}, 10, [&](long n, double *x) -> double {
            using namespace osvr::vbtracker;
            ConfigParams params;
            updateConfigFromVec(params, ParamVec::Map(x));
            auto system = makeHDKTrackingSystem(params);
            auto &target = *(system->getBody(BodyId(0)).getTarget(TargetId(0)));

            /// @todo

            return 0;
        });
    return 0;
}
