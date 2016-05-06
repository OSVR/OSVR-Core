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

#ifndef INCLUDED_OptimizationBase_h_GUID_C3489E14_63D4_4290_45C0_225AC84A8BF5
#define INCLUDED_OptimizationBase_h_GUID_C3489E14_63D4_4290_45C0_225AC84A8BF5

// Internal Includes
#include "UtilityFunctions.h"

#include <ConfigParams.h>
#include <MakeHDKTrackingSystem.h>
#include <TrackedBodyTarget.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenFilters.h>

// Standard includes
#include <memory>
#include <utility>

namespace osvr {
namespace vbtracker {

    /// Input from main to the optimization routine (wrapper)
    struct OptimCommonData {
        CameraParameters const &camParams;
        ConfigParams const &initialParams;
    };
    /// Creates and owns the tracking system created for each optimization run.
    /// For each row of data, one of the two LED-feeding algorithm functors must
    /// be called to update the system: either `FeedDataWithoutProcessing` (if
    /// you don't care about the "standard" tracking results) or
    /// `MainAlgoUnderStudy` (if you actually want the main algo to produce a
    /// pose estimate)
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
        TrackedBodyTarget const &getTarget() const { return *target_; }
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

    /// Algorithm functor to be called in a loop that processes measurements
    /// rows: all it does is feed in LEDs, (stage 2), but not process them into
    /// tracking results (stage 3). Do not use both this and
    /// `MainAlgoUnderStudy` in the same loop, or you'll feed in each
    /// measurement twice.
    class FeedDataWithoutProcessing {
      public:
        void operator()(OptimData &optim, TimestampedMeasurements const &row) {
            auto inputData =
                makeImageOutputDataFromRow(row, optim.getCamParams());
            optim.getSystem().updateLedsFromVideoData(std::move(inputData));
        }
    };

    /// Algorithm functor to be called in a loop that processes measurements
    /// rows: feeds in LEDs (stage 2) and attempts to generate a pose with the
    /// standard algorithm (stage 3). Do not use both this and
    /// `FeedDataWithoutProcessing` in the same loop, or you'll feed in each
    /// measurement twice.
    ///
    /// Reports the main algorithm's pose, if any.
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
        std::size_t getNumResets(OptimData const &optim) {
            return optim.getTarget().numTrackingResets();
        }

      private:
        bool gotPose = false;
        Eigen::Isometry3d pose;
    };

    /// Algorithm functor, call after either stage-2 functor: obtains a RANSAC
    /// pose and feeds it into a 1-euro filter.
    ///
    /// Reports a filtered version of the "brute-force" RANSAC PNP pose.
    class RansacOneEuro {
      public:
        void operator()(OptimData &optim, TimestampedMeasurements const &row) {
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
                    numFlips_++;
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
        std::size_t getNumFlips() const { return numFlips_; }
        bool havePose() const { return gotPose; }
        Eigen::Isometry3d const &getPose() const { return pose; }

      private:
        util::filters::PoseOneEuroFilterd ransacPoseFilter;
        TimeValue last;
        bool isFirst = true;
        bool gotPose = false;
        bool flippedPose_ = false;
        std::size_t numFlips_ = 0;
        Eigen::Isometry3d pose;
    };

    /// Algorithm functor, to use a transformed recorded tracker pose, typically
    /// instead of an alternate method of computing pose as a reference for cost
    /// computations since it's much faster and theoretically more objectively
    /// accurate.
    ///
    /// Reports the "reference tracker" transformed by the parameters in
    /// UtilityFunctions.h which may be estimated/updated for a given data set
    /// by the RefTracker optimization routine.
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
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_OptimizationBase_h_GUID_C3489E14_63D4_4290_45C0_225AC84A8BF5
