/** @file
    @brief Header with integer types for Viewer, Eye, and Surface
   counts/indices, as well as viewport information.

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

#ifndef INCLUDED_RenderingTypesC_h_GUID_6689A6CA_76AC_48AC_A0D0_2902BC95AC35
#define INCLUDED_RenderingTypesC_h_GUID_6689A6CA_76AC_48AC_A0D0_2902BC95AC35

/* Internal Includes */
#include <osvr/Util/StdInt.h>
#include <osvr/Util/APIBaseC.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @addtogroup PluginKit
@{
*/

/** @brief The integer type specifying a number of viewers in a system. */
typedef uint32_t OSVR_ViewerCount;

/** @brief The integer type specifying the number of eyes of a viewer. */
typedef uint8_t OSVR_EyeCount;

/** @brief The integer type specifying the number of surfaces seen by a viewer's
    eye. */
typedef uint32_t OSVR_SurfaceCount;

/** @brief The integer type used in specification of size or location of a
    viewport.  */
typedef int32_t OSVR_ViewportDimension;

/** @} */

OSVR_EXTERN_C_END

#endif
