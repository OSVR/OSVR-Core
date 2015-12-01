/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#ifndef INCLUDED_VideoJacobian_h_GUID_F0FB11A4_0786_4285_B197_D0A3AFFCBFEC
#define INCLUDED_VideoJacobian_h_GUID_F0FB11A4_0786_4285_B197_D0A3AFFCBFEC

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

/// Computes the jacobian for a given quaternion q, position pos, and beacon
/// location beacon, as well as the focal length fl
///
/// Calculated with Maxima and transformed in a mostly-automated fashion to C++
/// code.
inline Eigen::Matrix<double, 2, 9>
getVideoJacobian(Eigen::Vector3d const &pos, Eigen::Vector3d const &incrot,
                 Eigen::Quaterniond const &q, double fl,
                 Eigen::Vector3d const &beacon) {
    auto v1 = pos[2] - beacon[0] * incrot[1] + incrot[0] * beacon[1];
    auto v2 = 1 / v1;
    auto v3 = fl * v2;
    auto v4 = (-beacon[1] * incrot[2]) + incrot[1] * beacon[2] + pos[0];
    auto v5 = 1 / (v1 * v1);
    auto v6 = beacon[0] * incrot[2] - incrot[0] * beacon[2] + pos[1];
    Eigen::Matrix<double, 2, 9> result;
    result << v3, 0, -v4 * v5 * fl, -beacon[1] * v4 * v5 * fl,
        beacon[2] * v2 * fl + beacon[0] * v4 * v5 * fl, -beacon[1] * v2 * fl,
        incrot[1] * v4 * v5 * fl,
        (-incrot[2] * v2 * fl) - incrot[0] * v4 * v5 * fl, incrot[1] * v2 * fl,
        0, v3, -v6 * v5 * fl, (-beacon[2] * v2 * fl) - beacon[1] * v6 * v5 * fl,
        beacon[0] * v6 * v5 * fl, beacon[0] * v2 * fl,
        incrot[2] * v2 * fl + incrot[1] * v6 * v5 * fl,
        -incrot[0] * v6 * v5 * fl, -incrot[0] * v2 * fl;
    return result;
}

#endif // INCLUDED_VideoJacobian_h_GUID_F0FB11A4_0786_4285_B197_D0A3AFFCBFEC
