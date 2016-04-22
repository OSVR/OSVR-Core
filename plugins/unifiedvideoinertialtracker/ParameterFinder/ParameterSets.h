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

#ifndef INCLUDED_ParameterSets_h_GUID_3059CE4C_7E9B_4784_4678_5B01F94BC979
#define INCLUDED_ParameterSets_h_GUID_3059CE4C_7E9B_4784_4678_5B01F94BC979

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    namespace optimization_param_sets {
        /// Trait for param set name.
        template <typename T> struct ParamSetName;

        struct ProcessNoiseVarianceAndDecay {
            /// required part of interface
            static const size_t Dimension = 5;

            /// Internal for convenience use.
            using ParamVec = Vec<Dimension>;

            /// required part of interface
            static ParamVec getInitialVec(OptimCommonData const &commonData) {

                ParamVec x;
                const auto &p = commonData.initialParams;
                const auto posProcessNoise = p.processNoiseAutocorrelation[0];
                const auto oriProcessNoise = p.processNoiseAutocorrelation[3];

                x << posProcessNoise, oriProcessNoise,
                    p.measurementVarianceScaleFactor,
                    p.linearVelocityDecayCoefficient,
                    p.angularVelocityDecayCoefficient;
                return x;
            }

            /// required part of interface
            static std::pair<double, double> getRho() { return {1e-10, 1e0}; }

            /// required part of interface
            static void updateParamsFromVec(ConfigParams &params,
                                            ParamVec const &x) {
                // Update config from provided param vec
                /// positional noise
                params.processNoiseAutocorrelation[0] =
                    params.processNoiseAutocorrelation[1] =
                        params.processNoiseAutocorrelation[2] = x[0];
                /// rotational noise
                params.processNoiseAutocorrelation[3] =
                    params.processNoiseAutocorrelation[4] =
                        params.processNoiseAutocorrelation[5] = x[1];

                params.measurementVarianceScaleFactor = x[2];

                params.linearVelocityDecayCoefficient = x[3];
                params.angularVelocityDecayCoefficient = x[4];
            }

            /// required part of interface
            static const char *getVecElementNames() {
                return "position process noise autocorrelation, "
                       "orientation process noise autocorrelation, "
                       "video tracker measurement variance scale factor, "
                       "linear velocity decay coefficient, "
                       "angular velocity decay coefficient";
            }
        };

        template <> struct ParamSetName<ProcessNoiseVarianceAndDecay> {
            static const char *get() { return "ProcessNoiseVarianceAndDecay"; }
        };

        struct BrightAndNew {
            /// required part of interface
            static const size_t Dimension = 2;

            /// Internal for convenience use.
            using ParamVec = Vec<Dimension>;

            /// required part of interface
            static ParamVec getInitialVec(OptimCommonData const &commonData) {
                ParamVec x;
                const auto &p = commonData.initialParams;
                x << p.brightLedVariancePenalty, p.tuning.noveltyPenaltyBase;
                return x;
            }

            /// required part of interface
            static std::pair<double, double> getRho() { return {1e-5, 1e1}; }

            /// required part of interface
            static void updateParamsFromVec(ConfigParams &p,
                                            ParamVec const &x) {
                // Update config from provided param vec
                p.brightLedVariancePenalty = x[0];
                p.shouldSkipBrightLeds = false;
                p.tuning.noveltyPenaltyBase = x[1];
            }

            /// required part of interface
            static const char *getVecElementNames() {
                return "bright LED variance penalty,\n"
                       "novelty penalty base";
            }
        };

        template <> struct ParamSetName<BrightAndNew> {
            static const char *get() { return "BrightAndNew"; }
        };

        struct HighResidual {
            /// required part of interface
            static const size_t Dimension = 2;

            /// Internal for convenience use.
            using ParamVec = Vec<Dimension>;

            /// required part of interface
            static ParamVec getInitialVec(OptimCommonData const &commonData) {
                ParamVec x;
                const auto &p = commonData.initialParams;
                x << p.maxResidual, p.highResidualVariancePenalty;
                return x;
            }

            /// required part of interface
            static std::pair<double, double> getRho() { return {1e-4, 1e1}; }

            /// required part of interface
            static void updateParamsFromVec(ConfigParams &p,
                                            ParamVec const &x) {
                // Update config from provided param vec
                p.maxResidual = x[0];
                p.highResidualVariancePenalty = x[2];
            }

            /// required part of interface
            static const char *getVecElementNames() {
                return "max residual,\n"
                       "high residual variance penalty";
            }
        };

        template <> struct ParamSetName<HighResidual> {
            static const char *get() { return "HighResidual"; }
        };
    } // namespace optimization_param_sets
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ParameterSets_h_GUID_3059CE4C_7E9B_4784_4678_5B01F94BC979
