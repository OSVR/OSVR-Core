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
    auto v1 = 2 * q.w() * beacon[1];
    auto v2 = v1 - 2 * beacon[0] * q.x();
    auto v3 = q.w() * q.w();
    auto v4 = q.x() * q.x();
    auto v5 = 1 - 2 * (v3 + v4);
    auto v6 = q.x() * q.y();
    auto v7 = q.w() * q.z() + v6;
    auto v8 = q.w() * q.y();
    auto v9 = v8 - q.x() * q.z();
    auto v10 =
        2 * beacon[0] * v9 + 2 * beacon[1] * v7 + pos[2] + v5 * beacon[2];
    auto v11 = 1. / v10 * v10;
    auto v12 = q.y() * q.y();
    auto v13 = 1 - 2 * (v4 + v12);
    auto v14 = q.x() * q.z() + v8;
    auto v15 = q.w() * q.x();
    auto v16 = v15 - q.y() * q.z();
    auto v17 =
        2 * beacon[1] * v16 + 2 * beacon[2] * v14 + beacon[0] * v13 + pos[0];
    auto v18 = 2 * q.x() * beacon[2];
    auto v19 = 1. / v10;
    auto v20 = 2 * beacon[0] * q.y();
    auto v21 = 2 * beacon[1] * q.z() + v20 - 4 * q.w() * beacon[2];
    auto v22 = 2 * beacon[1] * q.x();
    auto v23 = 2 * beacon[2] * q.y();
    auto v24 = (-2 * beacon[0] * q.z()) + 2 * beacon[1] * q.y() -
               4 * q.x() * beacon[2];
    auto v25 = 2 * beacon[0] * q.w();
    auto v26 = v22 + v25;
    auto v27 = v19 * fl;
    auto v28 = 1 - 2 * (v3 + v12);
    auto v29 = v6 - q.w() * q.z();
    auto v30 = q.y() * q.z() + v15;
    auto v31 =
        2 * beacon[0] * v30 + 2 * beacon[2] * v29 + beacon[1] * v28 + pos[1];
    Eigen::Matrix<double, 2, 10> result;
    result << (v18 - 2 * beacon[1] * q.y()) * v19 * fl - v2 * v11 * v17 * fl,
        (v22 + v23) * v19 * fl - v21 * v11 * v17 * fl,
        (v1 - 4 * beacon[0] * q.x() + 2 * beacon[2] * q.z()) * v19 * fl -
            v24 * v11 * v17 * fl,
        (2 * q.w() * beacon[2] - 4 * beacon[0] * q.y() -
         2 * beacon[1] * q.z()) *
                v19 * fl -
            v26 * v11 * v17 * fl,
        v27, 0, -v11 * v17 * fl, v13 * v19 * fl - 2 * v9 * v11 * v17 * fl,
        2 * v16 * v19 * fl - 2 * v7 * v11 * v17 * fl,
        2 * v14 * v19 * fl - v5 * v11 * v17 * fl,
        /* end of row */ (v20 - 2 * q.w() * beacon[2]) * v19 * fl -
            v2 * v11 * v31 * fl,
        ((-4 * q.w() * beacon[1]) + 2 * beacon[0] * q.x() -
         2 * beacon[2] * q.z()) *
                v19 * fl -
            v21 * v11 * v31 * fl,
        (v25 + v23) * v19 * fl - v24 * v11 * v31 * fl,
        (v18 - 4 * beacon[1] * q.y() + 2 * beacon[0] * q.z()) * v19 * fl -
            v26 * v11 * v31 * fl,
        0, v27, -v11 * v31 * fl, 2 * v30 * v19 * fl - 2 * v9 * v11 * v31 * fl,
        v28 * v19 * fl - 2 * v7 * v11 * v31 * fl,
        2 * v29 * v19 * fl - v5 * v11 * v31 * fl;
    return result;
}

#endif // INCLUDED_VideoJacobian_h_GUID_F0FB11A4_0786_4285_B197_D0A3AFFCBFEC
