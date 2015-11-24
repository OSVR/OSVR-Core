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
// - none

// Standard includes
// - none

/// Computes the jacobian for a given quaternion q, position pos, and beacon
/// location beacon, as well as the focal length fl
///
/// Calculated with Maxima and transformed in a mostly-automated fashion to C++
/// code.
inline Eigen::Matrix<double, 2, 10>
getVideoJacobian(Eigen::Quaterniond const &q, Eigen::Vector3d const &pos,
                 Eigen::Vector3d const &beacon, double fl) {
    auto v1 = 2 * beacon[1] * q.x();
    auto v2 = v1 - 2 * beacon[0] * q.y();
    auto v3 = q.x() * q.x();
    auto v4 = q.y() * q.y();
    auto v5 = 1 - 2 * (v3 + v4);
    auto v6 = q.x() * q.z();
    auto v7 = v6 - q.w() * q.y();
    auto v8 = q.y() * q.z();
    auto v9 = v8 + q.w() * q.x();
    auto v10 =
        2 * beacon[1] * v9 + 2 * beacon[0] * v7 + beacon[2] * v5 + pos[2];
    auto v11 = std::pow(1. / v10, 2);
    auto v12 = q.x() * q.y();
    auto v13 = v12 - q.w() * q.z();
    auto v14 = v6 + q.w() * q.y();
    auto v15 = std::pow(q.z(), 2);
    auto v16 = 1 - 2 * (v4 + v15);
    auto v17 =
        beacon[0] * v16 + 2 * beacon[2] * v14 + 2 * beacon[1] * v13 + pos[0];
    auto v18 = 2 * beacon[2] * q.y();
    auto v19 = 1. / v10;
    auto v20 = 2 * beacon[0] * q.z();
    auto v21 = v20 - 4 * q.x() * beacon[2] + 2 * q.w() * beacon[1];
    auto v22 = 2 * beacon[1] * q.y();
    auto v23 = 2 * beacon[2] * q.z();
    auto v24 =
        2 * beacon[1] * q.z() - 4 * beacon[2] * q.y() - 2 * beacon[0] * q.w();
    auto v25 = 2 * beacon[0] * q.x();
    auto v26 = v22 + v25;
    auto v27 = v19 * fl;
    auto v28 = q.w() * q.z() + v12;
    auto v29 = v8 - q.w() * q.x();
    auto v30 = 1 - 2 * (v3 + v15);
    auto v31 =
        beacon[1] * v30 + 2 * beacon[2] * v29 + 2 * beacon[0] * v28 + pos[1];
    Eigen::Matrix<double, 2, 10> result;
    result << (v18 - 2 * beacon[1] * q.z()) * v19 * fl - v2 * v11 * v17 * fl,
        (v22 + v23) * v19 * fl - v21 * v11 * v17 * fl,
        (v1 + 2 * q.w() * beacon[2] - 4 * beacon[0] * q.y()) * v19 * fl -
            v24 * v11 * v17 * fl,
        (-2 * q.w() * beacon[1] + 2 * q.x() * beacon[2] -
         4 * beacon[0] * q.z()) *
                v19 * fl -
            v26 * v11 * v17 * fl,
        v27, 0, -v11 * v17 * fl, v19 * v16 * fl - 2 * v7 * v11 * v17 * fl,
        2 * v13 * v19 * fl - 2 * v9 * v11 * v17 * fl,
        2 * v14 * v19 * fl - v5 * v11 * v17 * fl,
        /* end of row */ (v20 - 2 * q.x() * beacon[2]) * v19 * fl -
            v2 * v11 * v31 * fl,
        (-4 * beacon[1] * q.x() - 2 * q.w() * beacon[2] +
         2 * beacon[0] * q.y()) *
                v19 * fl -
            v21 * v11 * v31 * fl,
        (v25 + v23) * v19 * fl - v24 * v11 * v31 * fl,
        (2 * beacon[0] * q.w() + v18 - 4 * beacon[1] * q.z()) * v19 * fl -
            v26 * v11 * v31 * fl,
        0, v27, -v11 * v31 * fl, 2 * v28 * v19 * fl - 2 * v7 * v11 * v31 * fl,
        v19 * v30 * fl - 2 * v9 * v11 * v31 * fl,
        2 * v29 * v19 * fl - v5 * v11 * v31 * fl;
    return result;
}

#endif // INCLUDED_VideoJacobian_h_GUID_F0FB11A4_0786_4285_B197_D0A3AFFCBFEC
