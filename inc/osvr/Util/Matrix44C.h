/** @file
    @brief Header

    Must be c-safe!

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_Matrix44C_h_GUID_55257DBD_69C2_46A4_87DB_8E764F13C8B9
#define INCLUDED_Matrix44C_h_GUID_55257DBD_69C2_46A4_87DB_8E764F13C8B9

/* Internal Includes */
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/StdInt.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN
/** @addtogroup UtilMath
@{
*/

/** @brief A structure defining a 4x4 matrix. Not to be used for general pose
    data, that can be more descriptively described with OSVR_Pose3.
*/
typedef struct OSVR_Matrix44 {
    /** @brief Data */
    double data[4][4];
} OSVR_Matrix44;

/** @brief Set a matrix to identity */
OSVR_INLINE void osvrMatrix44SetIdentity(OSVR_Matrix44 *mat) {
    for (unsigned int i = 0; i < 4; ++i) {
        for (unsigned int j = 0; j < 4; ++j) {
            mat->data[i][j] = (i == j ? 1. : 0.);
        }
    }
}

OSVR_EXTERN_C_END

#ifdef __cplusplus
/** @brief Set a matrix to identity (overload taking a reference instead of a
    pointer)
*/
inline void osvrMatrix44SetIdentity(OSVR_Matrix44 &mat) {
    osvrMatrix44SetIdentity(&mat);
}

/** @brief Get an identity matrix */
inline OSVR_Matrix44 osvrMatrix44GetIdentity() {
    OSVR_Matrix44 ret = {0};
    for (unsigned int i = 0; i < 4; ++i) {
        ret.data[i][i] = 1.;
    }
    return ret;
}
#endif

/** @} */

#endif
