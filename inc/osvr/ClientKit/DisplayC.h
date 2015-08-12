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
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/Matrix44C.h>
#include <osvr/Util/Pose3C.h>

/* Library/third-party includes */
/* none */

/* Standard includes */
/* none */

OSVR_EXTERN_C_BEGIN

/** @brief Opaque type of a display configuration. */
typedef struct OSVR_DisplayConfigObject *OSVR_DisplayConfig;

typedef int32_t OSVR_ViewportDimension;

/** @brief Allocates a display configuration object.
    @return OSVR_RETURN_FAILURE if invalid parameters were passed or some other
    error occured, in which case the output argument is unmodified.
  */
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetDisplay(OSVR_ClientContext ctx, OSVR_DisplayConfig *disp);

/** @brief Frees a display configuration object. The corresponding context must
    still be open.

    If you fail to call this, it will be automatically called as part of
    clean-up when the corresponding context is closed.

    @return OSVR_RETURN_FAILURE if a null config was passed, or if the given
    display object was already freed.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientFreeDisplay(OSVR_DisplayConfig disp);

/** @brief A display config can have one (or theoretically more) viewers */
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetNumViewers(OSVR_DisplayConfig disp, OSVR_ViewerCount *viewers);

/** @brief Get the center of projection/"eye point" for a viewer in a display
    config.

    Note that there may not necessarily be any surfaces rendered from this pose
    (it's the unused "center" eye in a stereo configuration) so only use this if
    it makes integration into your engine or existing applications (not
    originally designed for stereo) easier.

    @return OSVR_RETURN_FAILURE if invalid parameters were passed or no pose was
    yet available, in which case the pose argument is unmodified.
  */
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetViewerPose(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_Pose3 *pose);

/** @brief Each viewer in a display config can have one or more "eyes" which
    have a substantially similar pose

    @return OSVR_RETURN_FAILURE if invalid parameters were passed.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetNumEyesForViewer(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount *eyes);

/** @brief Get the center of projection/"eye point" for the given eye of a
    viewer in a display config

    @return OSVR_RETURN_FAILURE if invalid parameters were passed or no pose was
    yet available, in which case the pose argument is unmodified.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetViewerEyePose(OSVR_DisplayConfig disp, OSVR_ViewerCount viewer,
                           OSVR_EyeCount eye, OSVR_Pose3 *pose);

/** @brief Each eye of each viewer in a display config has one or more surfaces
 * (aka "screens") on which content should be rendered. */
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetNumSurfacesForViewerEye(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount *surfaces);

/** @brief Get the dimensions/location of the viewport **within the display
    input** for a surface seen by an eye of a viewer
    in a display config. (This does not include other video inputs that may be
    on a single virtual desktop, etc. and does not necessarily indicate that a
   viewport in the sense of glViewport must be created with these parameters,
   though the output order matches for convenience.)

    @param disp Display config object
    @param viewer Viewer ID
    @param eye Eye ID
    @param surface Surface ID
    @param left Output: Distance in pixels from the left of the video input to
    the left of the viewport.
    @param bottom Output: Distance in pixels from the bottom of the video input
    to the bottom of the viewport.
    @param width Output: Width of viewport in pixels.
    @param height Output: Height of viewport in pixels.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetRelativeViewportForViewerEyeSurface(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_ViewportDimension *left,
    OSVR_ViewportDimension *bottom, OSVR_ViewportDimension *width,
    OSVR_ViewportDimension *height);

/** @brief Get the projection matrix for a surface seen by an eye of a viewer
   in a display config.  Currently returns a matrix that transforms column
   vectors from a right-handed coordinate system to a volume with signed Z (that
   is, near plane mapped to -1 and far to 1) - that is, the convention used by
   the OpenGL fixed-function pipeline. A parameterization is forthcoming.

    @param disp Display config object
    @param viewer Viewer ID
    @param eye Eye ID
    @param surface Surface ID
    @param near Distance to near clipping plane - must be nonzero, typically
    positive.
    @param far Distance to far clipping plane - must be nonzero, typically
    positive and greater than near.
    @param matrix Output projection matrix, row-major storage order.
*/
OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode
osvrClientGetProjectionForViewerEyeSurface(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, double near, double far, OSVR_Matrix44 *matrix);

/** @todo for a surface: get needs distortion, get distortion shader, get distortion shader parameters */

OSVR_EXTERN_C_END

#endif
