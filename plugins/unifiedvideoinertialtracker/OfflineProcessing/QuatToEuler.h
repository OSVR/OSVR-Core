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

#ifndef INCLUDED_QuatToEuler_h_GUID_146FCB5A_7820_4B98_5BED_3223776E8CDF
#define INCLUDED_QuatToEuler_h_GUID_146FCB5A_7820_4B98_5BED_3223776E8CDF

// Internal Includes
#include "CSVCellGroup.h"

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <quat.h>

// Standard includes
// - none

namespace osvr {
namespace util {
    struct QuatAsEulerTag;

    inline Eigen::Vector3d getEuler(Eigen::Quaterniond const &q) {
        /// Convert an Eigen quat to a Quatlib quat.
        q_type quat;
        quat[Q_W] = q.w();
        quat[Q_X] = q.x();
        quat[Q_Y] = q.y();
        quat[Q_Z] = q.z();
        Eigen::Vector3d ret;
        // Actually gets yaw, pitch, roll in that order - not just any Euler
        // angles.
        q_to_euler(ret.data(), quat);
        // This method seems terribly unreliable.
        // return q.toRotationMatrix().eulerAngles(2, 0, 2);
        return ret;
    }

    template <typename T>
    inline void operator<<(CellGroupProxy<T, QuatAsEulerTag> &group,
                           Eigen::Vector3d const &euler) {
#if 0
        group << cell("rot.zprime", euler[0]) << cell("rot.xprime", euler[1])
              << cell("rot.z", euler[2]);
#else
        group << cell("yaw", euler[0]) << cell("pitch", euler[1])
              << cell("roll", euler[2]);

#endif
    }
    template <typename T>
    inline void operator<<(CellGroupProxy<T, QuatAsEulerTag> &group,
                           Eigen::Quaterniond const &q) {
        group << getEuler(q);
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_QuatToEuler_h_GUID_146FCB5A_7820_4B98_5BED_3223776E8CDF
