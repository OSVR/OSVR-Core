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
#include <boost/algorithm/string/predicate.hpp> // for argument handling

// Standard includes
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>

/// Define to add a "press enter to exit" thing at the end.
#undef PAUSE_BEFORE_EXIT

template <std::size_t N> using Vec = Eigen::Matrix<double, N, 1>;

namespace osvr {
namespace vbtracker {

    /// Use for outputt
    static const Eigen::IOFormat FullFormat =
        Eigen::IOFormat(Eigen::FullPrecision, 0, ",");

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

    namespace reftracker {
        using TransformParams = Vec<12>;
        /// where we can store results from a run of
        /// computeRefTrackerTransform()
        /// for use later/elsewhere.
        TransformParams const &getRefTrackerTransformParams() {
            static const TransformParams data =
                (TransformParams() << -0.784279819996699, 0.5366959149054978,
                 2.875217220618106, 0.05491254139447894, -0.2160783271567548,
                 0.02682823763491078, 0.06807108476730465, -0.115693455929497,
                 -0.1595428852952166, -0.05042439619765038,
                 -0.08944802515619116, -0.0004848434226332274)
                    .finished();
            return data;
        }
        /// Indices into the array.
        enum {
            BaseXlate = 0,
            BaseRot = BaseXlate + 3,
            InnerXlate = BaseRot + 3,
            InnerRot = InnerXlate + 3
        };

        inline Eigen::Translation3d
        getBaseTranslation(TransformParams const &vec) {
            return Eigen::Translation3d(vec.head<3>());
        }
        inline Eigen::Quaterniond getBaseRotation(TransformParams const &vec) {
            return rot_exp(vec.segment<3>(BaseRot));
        }
        inline Eigen::Isometry3d getBaseTransform(TransformParams const &vec) {
            return makeIsometry(getBaseTranslation(vec), getBaseRotation(vec));
        }
        inline Eigen::Translation3d
        getInnerTranslation(TransformParams const &vec) {
            return Eigen::Translation3d(vec.segment<3>(InnerXlate));
        }
        inline Eigen::Quaterniond getInnerRotation(TransformParams const &vec) {
            return rot_exp(vec.segment<3>(InnerRot));
        }
        inline Eigen::Isometry3d getInnerTransform(TransformParams const &vec) {
            return makeIsometry(getInnerTranslation(vec),
                                getInnerRotation(vec));
        }
    } // namespace reftracker

    /// Interface for tracking references as used in optimization.
    class TrackingReference {
      public:
        virtual void operator()(OptimData &optim,
                                TimestampedMeasurements const &row) = 0;
        virtual bool havePose() const = 0;
        virtual Eigen::Isometry3d const &getPose() const = 0;

      protected:
        TrackingReference() = default;
    };

    class RansacOneEuro : public TrackingReference {
      public:
        void operator()(OptimData &optim,
                        TimestampedMeasurements const &row) override {
            gotPose = false;
            flippedPose_ = false;
            Eigen::Vector3d pos;
            Eigen::Quaterniond quat;
            auto gotRansac =
                optim.getTarget().uncalibratedRANSACPoseEstimateFromLeds(
                    optim.getCamParams(), pos, quat);
            if (gotRansac) {
                /// I kept my head upright, but sometimes RANSAC is doing the
                /// wrong thing and picking a pose rotated by 180 about z
                double yAxisYComponent = (quat * Eigen::Vector3d::UnitY()).y();
                if (yAxisYComponent < 0) {
                    // std::cout << "RANSAC picked upside-down!" << std::endl;
                    flippedPose_ = true;
                    return;
                }
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
        bool flippedPose() const { return flippedPose_; }
        bool havePose() const override { return gotPose; }
        Eigen::Isometry3d const &getPose() const override { return pose; }

      private:
        PoseFilter ransacPoseFilter;
        TimeValue last;
        bool isFirst = true;
        bool gotPose = false;
        bool flippedPose_ = false;
        Eigen::Isometry3d pose;
    };

    class ReferenceTracker {
      public:
        ReferenceTracker()
            : base_(reftracker::getBaseTransform(
                  reftracker::getRefTrackerTransformParams())),
              inner_(reftracker::getInnerTransform(
                  reftracker::getRefTrackerTransformParams())) {}

        void operator()(OptimData &optim, TimestampedMeasurements const &row) {
            // Update our pose by the reference pose transformed.
            pose_ = base_ * makeIsometry(row.xlate, row.rot) * inner_;
        }
        bool havePose() const { return true; }
        Eigen::Isometry3d const &getPose() const { return pose_; }

      private:
        Eigen::Isometry3d base_;
        Eigen::Isometry3d inner_;
        Eigen::Isometry3d pose_;
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
        std::cout << "Ref to VideoBase: "
                  << (baseXform * sample.refPose * innerXform)
                         .translation()
                         .transpose()
                  << std::endl;
        std::cout << "Vive to VideoBase: "
                  << (baseXform * sample.refPose).translation().transpose()
                  << std::endl;
        std::cout << "---------" << std::endl;
    }

    /// Optimization routine: compute the transform between the smoothed RANSAC
    /// results and the reference tracker.
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
            std::size_t flipped = 0;
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
                } else if (ransacOneEuro.flippedPose()) {
                    ++flipped;
                }
            }

            std::cout << "Initial tracking complete: " << samples.size()
                      << " valid samples, " << flipped << " flipped."
                      << std::endl;
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
            std::cout << baseXlate.format(FullFormat) << "\n" << std::endl;
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
            std::cout << baseXlateRot.format(FullFormat) << "\n" << std::endl;
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
                x, {1e-8, 1e-2}, maxRuns,
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
                std::cout << x.format(FullFormat) << std::endl;

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

    template <typename TrackingReferenceType>
    void runOptimizer(
        std::vector<std::unique_ptr<TimestampedMeasurements>> const &data,
        OptimCommonData const &commonData, std::size_t maxRuns) {
        using ParamVec = Vec<5>;
        const double REALLY_BIG = 1000.;

        /// Set up initial values for the vector we'll optimize - load them from
        /// the config params.
        ParamVec x;
        {
            const auto &p = commonData.initialParams;
            const auto posProcessNoise = p.processNoiseAutocorrelation[0];
            const auto oriProcessNoise = p.processNoiseAutocorrelation[3];

            x << posProcessNoise, oriProcessNoise,
                p.measurementVarianceScaleFactor,
                p.linearVelocityDecayCoefficient,
                p.angularVelocityDecayCoefficient;
        }

        std::cout << "Optimizing, respectively: "
                     "position process noise autocorrelation, "
                     "orientation process noise autocorrelation, "
                     "video tracker measurement variance scale factor, "
                     "linear velocity decay coefficient, "
                     "angular velocity decay coefficient "
                     "\n";
        std::cout << "Initial vector:\n" << x.format(FullFormat) << std::endl;

        auto ret = ei_newuoa_wrapped(
            x, {1e-16, 1e-1}, static_cast<long>(maxRuns),
            [&](ParamVec const &paramVec) -> double {
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

                // params.beaconProcessNoise = paramVec[2];
                params.measurementVarianceScaleFactor = paramVec[2];

                params.linearVelocityDecayCoefficient = paramVec[3];
                params.angularVelocityDecayCoefficient = paramVec[4];

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
                    auto numResets = optim.getTarget().numTrackingResets();
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
                return REALLY_BIG;
            });
        std::cout << "Optimizer returned " << ret
                  << " and these parameter values:" << std::endl;
        std::cout << x.format(FullFormat) << std::endl;
    }

} // namespace vbtracker
} // namespace osvr

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
    /// max runs for the parameter optimizer
    static const std::size_t MAX_RUNS = 30;
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
        osvr::vbtracker::runOptimizer<osvr::vbtracker::RansacOneEuro>(
            data, osvr::vbtracker::OptimCommonData{camParams, params},
            MAX_RUNS);
        break;

    case OptimizationRoutine::ParamViaRefTracker:
        osvr::vbtracker::runOptimizer<osvr::vbtracker::ReferenceTracker>(
            data, osvr::vbtracker::OptimCommonData{camParams, params},
            MAX_RUNS);
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
