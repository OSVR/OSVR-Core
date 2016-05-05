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

#ifndef INCLUDED_UtilityFunctions_h_GUID_18928B12_FA9D_4285_102C_3106E5EEE14C
#define INCLUDED_UtilityFunctions_h_GUID_18928B12_FA9D_4285_102C_3106E5EEE14C

// Internal Includes
#include <osvr/Util/TimeValue.h>

#include <LedMeasurement.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/EigenQuatExponentialMap.h>

// Standard includes
#include <array>
#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

namespace osvr {
namespace vbtracker {

    using osvr::util::time::TimeValue;
    struct TimestampedMeasurements {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        TimeValue tv;
        Eigen::Vector3d xlate;
        Eigen::Quaterniond rot;
        LedMeasurementVec measurements;
        bool ok = false;
    };
    using TimestampedMeasurementsPtr = std::unique_ptr<TimestampedMeasurements>;
    using MeasurementsRows = std::vector<TimestampedMeasurementsPtr>;

    template <std::size_t N> using Vec = Eigen::Matrix<double, N, 1>;
    /// Use for output of parameters
    inline Eigen::IOFormat const &getFullFormat() {
        static const Eigen::IOFormat FullFormat =
            Eigen::IOFormat(Eigen::FullPrecision, 0, ",", ",\n");
        return FullFormat;
    }

    inline double getReallyBigCost() { return 1000000.; }

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
                (TransformParams() << -1.066453847658292, 1.501958189971162,
                 2.909478550532421, 0.006159814184246497, -0.1565547643661447,
                 0.005975689966339379, 0.09264509945031509, -0.5488212361743223,
                 -0.1770928261336589, 0.02482363164948076, -0.2499304636098044,
                 0.03060138186823489)
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

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_UtilityFunctions_h_GUID_18928B12_FA9D_4285_102C_3106E5EEE14C
