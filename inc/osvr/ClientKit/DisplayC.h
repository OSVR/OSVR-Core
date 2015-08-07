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

#ifndef INCLUDED_DisplayC_h_GUID_8658EDC9_32A2_49A2_5F5C_10F67852AE74
#define INCLUDED_DisplayC_h_GUID_8658EDC9_32A2_49A2_5F5C_10F67852AE74

/* Internal Includes */
#include <osvr/ClientKit/Export.h>
#include <osvr/Util/APIBaseC.h>
#include <osvr/Util/ReturnCodesC.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/Matrix44C.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @brief Opaque type of a display configuration. */
typedef struct OSVR_DisplayConfigObject *OSVR_DisplayConfig;

typedef uint32_t OSVR_ViewerCount;
typedef uint8_t OSVR_EyeCount;
typedef uint32_t OSVR_SurfaceCount;

/** @brief Allocates a display configuration object. */
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetDisplay(OSVR_ClientContext ctx, OSVR_DisplayConfig *disp);

/** @brief Frees a display configuration object. The corresponding context must
 * still be open. If you fail to call this, it will be automatically cleaned up
 * when the corresponding context is closed.*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientFreeDisplay(OSVR_DisplayConfig disp);

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetNumViewers(OSVR_DisplayConfig disp, OSVR_ViewerCount *viewers);

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetNumEyesForViewer(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount *eyes);

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetNumSurfacesForViewerEye(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount *surfaces);

OSVR_EXTERN_C_END

#endif
