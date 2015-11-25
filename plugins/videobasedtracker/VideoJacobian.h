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
    /// Begin manually-created shortcut substitutions to improve performance
    /// through SIMD and reduce number of exponents that have to be manually
    /// fixed.
    Eigen::Vector3d incrotSqr = incrot.array() * incrot.array();
    Eigen::Vector3d incrotCu = incrot.array() * incrot.array() * incrot.array();
    Eigen::Vector4d qSqr = q.coeffs().array() * q.coeffs().array();
    Eigen::Vector4d qCu =
        q.coeffs().array() * q.coeffs().array() * q.coeffs().array();
    /// End manual section

    auto v1 = q.w() * q.y();
    auto v2 = q.x() * q.z() + v1;
    auto v3 = incrotSqr[2] + incrotSqr[1] + incrotSqr[0];
    auto v4 = sqrt(v3);
    auto v5 = 1 / v4;
    auto v6 = cos(v4);
    auto v7 = sin(v4);
    auto v8 = 1 / v3;
    auto v9 = v7 * v7;
    auto v10 = incrot[0] * incrot[2] * v8 * v9;
    auto v11 = v10 - incrot[1] * v5 * v6 * v7;
    auto v12 = q.x() * q.y();
    auto v13 = v12 - q.w() * q.z();
    auto v14 = incrot[1] * incrot[2] * v8 * v9;
    auto v15 = v14 + incrot[0] * v5 * v6 * v7;
    auto v16 = 1 - 2 * (qSqr[0] + qSqr[1]);
    auto v17 = incrotSqr[0] * v8 * v9;
    auto v18 = incrotSqr[1] * v8 * v9;
    auto v19 = 1 - 2 * (v17 + v18);
    auto v20 = v16 * v19 + 4 * v13 * v15 + 4 * v2 * v11;
    auto v21 = q.w() * q.x();
    auto v22 = v21 - q.y() * q.z();
    auto v23 = 1 - 2 * (qSqr[0] + qSqr[2]);
    auto v24 = q.w() * q.z() + v12;
    auto v25 = 2 * v24 * v19 + 2 * v23 * v15 + 4 * v22 * v11;
    auto v26 = 1 - 2 * (qSqr[1] + qSqr[2]);
    auto v27 = q.y() * q.z() + v21;
    auto v28 = v1 - q.x() * q.z();
    auto v29 = 2 * v28 * v19 + 4 * v27 * v15 + 2 * v26 * v11;
    auto v30 = beacon[0] * v29 + beacon[1] * v25 + beacon[2] * v20 + pos[2];
    auto v31 = 1 / v30;
    auto v32 = v31 * fl;
    auto v33 = 1 / (v30 * v30);
    auto v34 = incrot[0] * incrot[1] * v8 * v9;
    auto v35 = v34 - incrot[2] * v5 * v6 * v7;
    auto v36 = v10 + incrot[1] * v5 * v6 * v7;
    auto v37 = incrotSqr[2] * v8 * v9;
    auto v38 = 1 - 2 * (v18 + v37);
    auto v39 = v26 * v38 + 4 * v28 * v36 + 4 * v27 * v35;
    auto v40 = 2 * v2 * v38 + 2 * v16 * v36 + 4 * v13 * v35;
    auto v41 = 2 * v22 * v38 + 4 * v24 * v36 + 2 * v23 * v35;
    auto v42 = beacon[1] * v41 + beacon[2] * v40 + beacon[0] * v39 + pos[0];
    auto v43 = 1 / (v4 * v4 * v4);
    auto v44 = 2 * incrotCu[0] * v43 * v6 * v7;
    auto v45 = 2 * incrot[0] * incrotSqr[1] * v43 * v6 * v7;
    auto v46 = 1 / (v3 * v3);
    auto v47 = -2 * incrotCu[0] * v46 * v9;
    auto v48 = -2 * incrot[0] * incrotSqr[1] * v46 * v9;
    auto v49 = 2 * incrot[0] * v8 * v9;
    auto v50 = v49 + v48 + v47 + v45 + v44;
    auto v51 = v6 * v6;
    auto v52 = 2 * incrot[0] * incrot[1] * incrot[2] * v43 * v6 * v7;
    auto v53 = v5 * v6 * v7;
    auto v54 = -2 * incrot[0] * incrot[1] * incrot[2] * v46 * v9;
    auto v55 = (-incrotSqr[0] * v8 * v9) + v54 + v53 + v52 -
               incrotSqr[0] * v43 * v6 * v7 + incrotSqr[0] * v8 * v51;
    auto v56 = -incrot[0] * incrot[1] * v8 * v51;
    auto v57 = incrot[0] * incrot[1] * v43 * v6 * v7;
    auto v58 = 2 * incrotSqr[0] * incrot[2] * v43 * v6 * v7;
    auto v59 = -2 * incrotSqr[0] * incrot[2] * v46 * v9;
    auto v60 = incrot[2] * v8 * v9;
    auto v61 = v60 + v34 + v59 + v58 + v57 + v56;
    auto v62 = beacon[1] * ((-4 * v24 * v50) + 2 * v23 * v55 + 4 * v22 * v61) +
               beacon[2] * ((-2 * v16 * v50) + 4 * v13 * v55 + 4 * v2 * v61) +
               beacon[0] * ((-4 * v28 * v50) + 4 * v27 * v55 + 2 * v26 * v61);
    auto v63 = 2 * incrot[0] * incrotSqr[2] * v43 * v6 * v7;
    auto v64 = -2 * incrot[0] * incrotSqr[2] * v46 * v9;
    auto v65 = v64 + v48 + v63 + v45;
    auto v66 = incrot[0] * incrot[1] * v8 * v51;
    auto v67 = -incrot[0] * incrot[1] * v43 * v6 * v7;
    auto v68 = -incrot[0] * incrot[1] * v8 * v9;
    auto v69 = v60 + v68 + v59 + v58 + v67 + v66;
    auto v70 = -incrot[0] * incrot[2] * v8 * v51;
    auto v71 = 2 * incrotSqr[0] * incrot[1] * v43 * v6 * v7;
    auto v72 = incrot[0] * incrot[2] * v43 * v6 * v7;
    auto v73 = -2 * incrotSqr[0] * incrot[1] * v46 * v9;
    auto v74 = incrot[1] * v8 * v9;
    auto v75 = v10 + v74 + v73 + v72 + v71 + v70;
    auto v76 = 2 * incrotCu[1] * v43 * v6 * v7;
    auto v77 = -2 * incrotCu[1] * v46 * v9;
    auto v78 = 2 * incrot[1] * v8 * v9;
    auto v79 = v78 + v77 + v73 + v76 + v71;
    auto v80 = -v5 * v6 * v7;
    auto v81 = v18 + v54 + v80 + v52 + incrotSqr[1] * v43 * v6 * v7 -
               incrotSqr[1] * v8 * v51;
    auto v82 = 2 * incrotSqr[1] * incrot[2] * v43 * v6 * v7;
    auto v83 = -2 * incrotSqr[1] * incrot[2] * v46 * v9;
    auto v84 = v60 + v68 + v83 + v82 + v67 + v66;
    auto v85 = beacon[0] * ((-4 * v28 * v79) + 2 * v26 * v81 + 4 * v27 * v84) +
               beacon[2] * ((-2 * v16 * v79) + 4 * v2 * v81 + 4 * v13 * v84) +
               beacon[1] * ((-4 * v24 * v79) + 4 * v22 * v81 + 2 * v23 * v84);
    auto v86 = 2 * incrot[1] * incrotSqr[2] * v43 * v6 * v7;
    auto v87 = -2 * incrot[1] * incrotSqr[2] * v46 * v9;
    auto v88 = v78 + v87 + v77 + v86 + v76;
    auto v89 = (-incrotSqr[1] * v8 * v9) + v54 + v53 + v52 -
               incrotSqr[1] * v43 * v6 * v7 + incrotSqr[1] * v8 * v51;
    auto v90 = -incrot[1] * incrot[2] * v8 * v51;
    auto v91 = incrot[1] * incrot[2] * v43 * v6 * v7;
    auto v92 = incrot[0] * v8 * v9;
    auto v93 = v14 + v92 + v48 + v91 + v45 + v90;
    auto v94 = v83 + v59 + v82 + v58;
    auto v95 = incrot[0] * incrot[2] * v8 * v51;
    auto v96 = -incrot[0] * incrot[2] * v43 * v6 * v7;
    auto v97 = -incrot[0] * incrot[2] * v8 * v9;
    auto v98 = v97 + v74 + v87 + v86 + v96 + v95;
    auto v99 = v14 + v92 + v64 + v63 + v91 + v90;
    auto v100 = beacon[1] * ((-4 * v24 * v94) + 2 * v23 * v98 + 4 * v22 * v99) +
                beacon[2] * ((-2 * v16 * v94) + 4 * v13 * v98 + 4 * v2 * v99) +
                beacon[0] * ((-4 * v28 * v94) + 4 * v27 * v98 + 2 * v26 * v99);
    auto v101 = 2 * incrotCu[2] * v43 * v6 * v7;
    auto v102 = -2 * incrotCu[2] * v46 * v9;
    auto v103 = 2 * incrot[2] * v8 * v9;
    auto v104 = v103 + v102 + v83 + v101 + v82;
    auto v105 = incrot[1] * incrot[2] * v8 * v51;
    auto v106 = -incrot[1] * incrot[2] * v43 * v6 * v7;
    auto v107 = -incrot[1] * incrot[2] * v8 * v9;
    auto v108 = v107 + v92 + v64 + v63 + v106 + v105;
    auto v109 = v37 + v54 + v80 + incrotSqr[2] * v43 * v6 * v7 + v52 -
                incrotSqr[2] * v8 * v51;
    auto v110 = v34 + incrot[2] * v5 * v6 * v7;
    auto v111 = v14 - incrot[0] * v5 * v6 * v7;
    auto v112 = 1 - 2 * (v17 + v37);
    auto v113 = v23 * v112 + 4 * v24 * v111 + 4 * v22 * v110;
    auto v114 = 2 * v13 * v112 + 2 * v16 * v111 + 4 * v2 * v110;
    auto v115 = 2 * v27 * v112 + 4 * v28 * v111 + 2 * v26 * v110;
    auto v116 = beacon[0] * v115 + beacon[2] * v114 + beacon[1] * v113 + pos[1];
    auto v117 = v49 + v64 + v47 + v63 + v44;
    auto v118 = v17 + v54 + v80 + v52 + incrotSqr[0] * v43 * v6 * v7 -
                incrotSqr[0] * v8 * v51;
    auto v119 = v97 + v74 + v73 + v96 + v71 + v95;
    auto v120 = v87 + v73 + v86 + v71;
    auto v121 = v60 + v34 + v83 + v82 + v57 + v56;
    auto v122 = v107 + v92 + v48 + v106 + v45 + v105;
    auto v123 = v103 + v102 + v59 + v101 + v58;
    auto v124 = v10 + v74 + v87 + v86 + v72 + v70;
    auto v125 = (-incrotSqr[2] * v8 * v9) + v54 + v53 -
                incrotSqr[2] * v43 * v6 * v7 + v52 + incrotSqr[2] * v8 * v51;
    Eigen::Matrix<double, 2, 9> result;
    result << v32, 0, -v33 * v42 * fl,
        (beacon[1] * (2 * v23 * v75 + 4 * v24 * v69 - 4 * v22 * v65) +
         beacon[2] * (4 * v13 * v75 + 2 * v16 * v69 - 4 * v2 * v65) +
         beacon[0] * (4 * v27 * v75 + 4 * v28 * v69 - 2 * v26 * v65)) *
                v31 * fl -
            v62 * v33 * v42 * fl,
        (beacon[1] * (2 * v23 * v93 + 4 * v24 * v89 - 4 * v22 * v88) +
         beacon[2] * (4 * v13 * v93 + 2 * v16 * v89 - 4 * v2 * v88) +
         beacon[0] * (4 * v27 * v93 + 4 * v28 * v89 - 2 * v26 * v88)) *
                v31 * fl -
            v85 * v33 * v42 * fl,
        (beacon[1] * (2 * v23 * v109 + 4 * v24 * v108 - 4 * v22 * v104) +
         beacon[2] * (4 * v13 * v109 + 2 * v16 * v108 - 4 * v2 * v104) +
         beacon[0] * (4 * v27 * v109 + 4 * v28 * v108 - 2 * v26 * v104)) *
                v31 * fl -
            v100 * v33 * v42 * fl,
        v39 * v31 * fl - v29 * v33 * v42 * fl,
        v41 * v31 * fl - v25 * v33 * v42 * fl,
        v40 * v31 * fl - v20 * v33 * v42 * fl, /* end of row */ 0, v32,
        -v33 * v116 * fl,
        (beacon[0] * (2 * v26 * v119 + 4 * v28 * v118 - 4 * v27 * v117) +
         beacon[2] * (4 * v2 * v119 + 2 * v16 * v118 - 4 * v13 * v117) +
         beacon[1] * (4 * v22 * v119 + 4 * v24 * v118 - 2 * v23 * v117)) *
                v31 * fl -
            v62 * v33 * v116 * fl,
        (beacon[0] * (2 * v26 * v122 + 4 * v28 * v121 - 4 * v27 * v120) +
         beacon[2] * (4 * v2 * v122 + 2 * v16 * v121 - 4 * v13 * v120) +
         beacon[1] * (4 * v22 * v122 + 4 * v24 * v121 - 2 * v23 * v120)) *
                v31 * fl -
            v85 * v33 * v116 * fl,
        (beacon[0] * (2 * v26 * v125 + 4 * v28 * v124 - 4 * v27 * v123) +
         beacon[2] * (4 * v2 * v125 + 2 * v16 * v124 - 4 * v13 * v123) +
         beacon[1] * (4 * v22 * v125 + 4 * v24 * v124 - 2 * v23 * v123)) *
                v31 * fl -
            v100 * v33 * v116 * fl,
        v115 * v31 * fl - v29 * v33 * v116 * fl,
        v113 * v31 * fl - v25 * v33 * v116 * fl,
        v114 * v31 * fl - v20 * v33 * v116 * fl;
    return result;
}

#endif // INCLUDED_VideoJacobian_h_GUID_F0FB11A4_0786_4285_B197_D0A3AFFCBFEC
