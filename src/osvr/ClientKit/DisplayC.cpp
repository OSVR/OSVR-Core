/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/ClientKit/DisplayC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Client/DisplayConfig.h>
#include <osvr/Util/MacroToolsC.h>
#include <osvr/Util/EigenExtras.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/MatrixConventions.h>
#include <osvr/Util/MatrixEigenAssign.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <utility>

struct OSVR_DisplayConfigObject {
    OSVR_DisplayConfigObject(OSVR_ClientContext context)
        : ctx(context),
          cfg(osvr::client::DisplayConfigFactory::create(context)) {
        OSVR_DEV_VERBOSE("Created an OSVR_DisplayConfigObject!");
    }
    ~OSVR_DisplayConfigObject() {
        OSVR_DEV_VERBOSE("OSVR_DisplayConfigObject destructor");
    }
    OSVR_ClientContext ctx;
    osvr::client::DisplayConfigPtr cfg;
};

#define OSVR_VALIDATE_OUTPUT_PTR(X, DESC)                                      \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (nullptr == X) {                                                        \
        OSVR_DEV_VERBOSE("Passed a null pointer for output parameter " #X      \
                         ", " DESC "!");                                       \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

OSVR_ReturnCode osvrClientGetDisplay(OSVR_ClientContext ctx,
                                     OSVR_DisplayConfig *disp) {
    OSVR_VALIDATE_OUTPUT_PTR(disp, "display config");
    if (ctx == nullptr) {
        OSVR_DEV_VERBOSE("Passed a null client context!");
        *disp = nullptr;
        return OSVR_RETURN_FAILURE;
    }
    std::shared_ptr<OSVR_DisplayConfigObject> config;
    try {
        config = std::make_shared<OSVR_DisplayConfigObject>(ctx);
    } catch (std::exception &e) {
        OSVR_DEV_VERBOSE(
            "Error creating display config: constructor threw exception :"
            << e.what());
        return OSVR_RETURN_FAILURE;
    }
    if (!config) {
        OSVR_DEV_VERBOSE(
            "Error creating display config - null config returned");
        return OSVR_RETURN_FAILURE;
    }
    if (!config->cfg) {
        OSVR_DEV_VERBOSE("Error creating display config - null internal config "
                         "object returned");
        return OSVR_RETURN_FAILURE;
    }
    ctx->acquireObject(config);
    *disp = config.get();
    return OSVR_RETURN_SUCCESS;
}

#define OSVR_VALIDATE_DISPLAY_CONFIG                                           \
    OSVR_UTIL_MULTILINE_BEGIN                                                  \
    if (nullptr == disp) {                                                     \
        OSVR_DEV_VERBOSE("Passed a null display config!");                     \
        return OSVR_RETURN_FAILURE;                                            \
    }                                                                          \
    OSVR_UTIL_MULTILINE_END

/// @todo make these an "always" check? instead of an assert

/// @todo actually check the config for number of viewers
/// (viewer < disp->cfg->size())
#define OSVR_VALIDATE_VIEWER_ID                                                \
    BOOST_ASSERT_MSG(viewer == 0, "Must pass a valid viewer ID.")

#define OSVR_VALIDATE_EYE_ID                                                   \
    BOOST_ASSERT_MSG(eye < disp->cfg->getNumViewerEyes(viewer),                \
                     "Must pass a valid eye ID.")

/// @todo actually check the config for number of surfaces
#define OSVR_VALIDATE_SURFACE_ID                                               \
    BOOST_ASSERT_MSG(surface == 0, "Must pass a valid surface ID.")

#define OSVR_VALIDATE_DISPLAY_INPUT_ID                                         \
    BOOST_ASSERT_MSG(displayInputIndex < disp->cfg->getNumDisplayInputs(),     \
                     "Must pass a valid display input index.")

OSVR_ReturnCode osvrClientFreeDisplay(OSVR_DisplayConfig disp) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_ClientContext ctx = disp->ctx;
    BOOST_ASSERT_MSG(
        ctx != nullptr,
        "Should never get a display config object with a null context in it.");
    if (nullptr == ctx) {
        return OSVR_RETURN_FAILURE;
    }
    auto freed = ctx->releaseObject(disp);
    return freed ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientCheckDisplayStartup(OSVR_DisplayConfig disp) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    return disp->cfg->isStartupComplete() ? OSVR_RETURN_SUCCESS
                                          : OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode
osvrClientGetNumDisplayInputs(OSVR_DisplayConfig disp,
                              OSVR_DisplayInputCount *numDisplayInputs) {

    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(numDisplayInputs, "display inputs count");
    *numDisplayInputs = disp->cfg->getNumDisplayInputs();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetDisplayDimensions(
    OSVR_DisplayConfig disp, OSVR_DisplayInputCount displayInputIndex,
    OSVR_DisplayDimension *width, OSVR_DisplayDimension *height) {

    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_DISPLAY_INPUT_ID;
    OSVR_VALIDATE_OUTPUT_PTR(width, "width");
    OSVR_VALIDATE_OUTPUT_PTR(height, "height");
    *width = disp->cfg->getDisplayInput(displayInputIndex).getDisplayWidth();
    *height = disp->cfg->getDisplayInput(displayInputIndex).getDisplayHeight();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetNumViewers(OSVR_DisplayConfig disp,
                                        OSVR_ViewerCount *viewers) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_OUTPUT_PTR(viewers, "viewer count");
    *viewers = disp->cfg->getNumViewers();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetViewerPose(OSVR_DisplayConfig disp,
                                        OSVR_ViewerCount viewer,
                                        OSVR_Pose3 *pose) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_OUTPUT_PTR(pose, "viewer pose");
    try {
        *pose = disp->cfg->getViewer(viewer).getPose();
        return OSVR_RETURN_SUCCESS;
    } catch (osvr::client::NoPoseYet &) {
        OSVR_DEV_VERBOSE("Error getting viewer pose: no pose yet available");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {

        OSVR_DEV_VERBOSE("Error getting viewer pose - exception: " << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientGetNumEyesForViewer(OSVR_DisplayConfig disp,
                                              OSVR_ViewerCount viewer,
                                              OSVR_EyeCount *eyes) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_OUTPUT_PTR(eyes, "eye count");
    *eyes = disp->cfg->getNumViewerEyes(viewer);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetViewerEyePose(OSVR_DisplayConfig disp,
                                           OSVR_ViewerCount viewer,
                                           OSVR_EyeCount eye,
                                           OSVR_Pose3 *pose) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(pose, "eye pose");
    try {
        *pose = disp->cfg->getViewerEye(viewer, eye).getPose();
        return OSVR_RETURN_SUCCESS;
    } catch (osvr::client::NoPoseYet &) {
        OSVR_DEV_VERBOSE(
            "Error getting viewer eye pose: no pose yet available");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {

        OSVR_DEV_VERBOSE(
            "Error getting viewer eye pose - exception: " << e.what());
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_FAILURE;
}

template <typename Scalar>
static inline OSVR_ReturnCode getViewMatrixImpl(OSVR_DisplayConfig disp,
                                                OSVR_ViewerCount viewer,
                                                OSVR_EyeCount eye, Scalar *mat,
                                                OSVR_MatrixConventions flags) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(mat, "view matrix");
    try {
        osvr::util::matrixEigenAssign(
            disp->cfg->getViewerEye(viewer, eye).getView(), flags, mat);
        return OSVR_RETURN_SUCCESS;
    } catch (osvr::client::NoPoseYet &) {
        OSVR_DEV_VERBOSE(
            "Error getting viewer eye view matrix: no pose yet available");
        return OSVR_RETURN_FAILURE;
    } catch (std::exception &e) {

        OSVR_DEV_VERBOSE(
            "Error getting viewer eye view matrix - exception: " << e.what());
        return OSVR_RETURN_FAILURE;
    } catch (...) {
        OSVR_DEV_VERBOSE("Error getting viewer eye view matrix");
        return OSVR_RETURN_FAILURE;
    }
}

OSVR_ReturnCode osvrClientGetViewerEyeViewMatrixd(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_MatrixConventions flags, double *mat) {
    return getViewMatrixImpl(disp, viewer, eye, mat, flags);
}

OSVR_ReturnCode osvrClientGetViewerEyeViewMatrixf(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_MatrixConventions flags, float *mat) {
    return getViewMatrixImpl(disp, viewer, eye, mat, flags);
}

OSVR_ReturnCode
osvrClientGetNumSurfacesForViewerEye(OSVR_DisplayConfig disp,
                                     OSVR_ViewerCount viewer, OSVR_EyeCount eye,
                                     OSVR_SurfaceCount *surfaces) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(surfaces, "surface count");
    *surfaces = disp->cfg->getNumViewerEyeSurfaces(viewer, eye);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetRelativeViewportForViewerEyeSurface(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_ViewportDimension *left,
    OSVR_ViewportDimension *bottom, OSVR_ViewportDimension *width,
    OSVR_ViewportDimension *height) {

    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_SURFACE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(left, "viewport left bound");
    OSVR_VALIDATE_OUTPUT_PTR(bottom, "viewport bottom bound");
    OSVR_VALIDATE_OUTPUT_PTR(width, "viewport width");
    OSVR_VALIDATE_OUTPUT_PTR(height, "viewport height");
    auto viewport = disp->cfg->getViewerEyeSurface(viewer, eye, surface)
                        .getDisplayRelativeViewport();
    *left = viewport.left;
    *bottom = viewport.bottom;
    *width = viewport.width;
    *height = viewport.height;
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetViewerEyeSurfaceDisplayInputIndex(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_DisplayInputCount *displayInput) {

    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_SURFACE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(displayInput, "index of display input");

    *displayInput = disp->cfg->getViewerEyeSurface(viewer, eye, surface)
                        .getDisplayInputIdx();

    return OSVR_RETURN_SUCCESS;
}

template <typename Scalar>
static inline OSVR_ReturnCode
getProjectionMatrixImpl(OSVR_DisplayConfig disp, OSVR_ViewerCount viewer,
                        OSVR_EyeCount eye, OSVR_SurfaceCount surface,
                        Scalar near, Scalar far, OSVR_MatrixConventions flags,
                        Scalar *mat) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_SURFACE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(mat, "projection matrix");
    if (near == 0 || far == 0) {
        OSVR_DEV_VERBOSE("Can't specify a near or far distance as 0!");
        return OSVR_RETURN_FAILURE;
    }
    if (near < 0 || far < 0) {
        OSVR_DEV_VERBOSE("Can't specify a negative near or far distance!");
        return OSVR_RETURN_FAILURE;
    }
    if (near == far) {
        OSVR_DEV_VERBOSE("Can't specify equal near and far distances!");
        return OSVR_RETURN_FAILURE;
    }
    osvr::util::matrixEigenAssign(
        disp->cfg->getViewerEyeSurface(viewer, eye, surface)
            .getProjection(near, far, flags),
        flags, mat);

    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetViewerEyeSurfaceProjectionMatrixd(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, double near, double far,
    OSVR_MatrixConventions flags, double *matrix) {
    return getProjectionMatrixImpl(disp, viewer, eye, surface, near, far, flags,
                                   matrix);
}

OSVR_ReturnCode osvrClientGetViewerEyeSurfaceProjectionMatrixf(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, float near, float far,
    OSVR_MatrixConventions flags, float *matrix) {
    return getProjectionMatrixImpl(disp, viewer, eye, surface, near, far, flags,
                                   matrix);
}

OSVR_ReturnCode osvrClientGetViewerEyeSurfaceProjectionClippingPlanes(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, double *left, double *right, double *bottom,
    double *top) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(left, "left");
    OSVR_VALIDATE_OUTPUT_PTR(right, "right");
    OSVR_VALIDATE_OUTPUT_PTR(bottom, "bottom");
    OSVR_VALIDATE_OUTPUT_PTR(top, "top");
    auto rect = disp->cfg->getViewerEyeSurface(viewer, eye, surface).getRect();
    *left = rect[rect.LEFT];
    *right = rect[rect.RIGHT];
    *bottom = rect[rect.BOTTOM];
    *top = rect[rect.TOP];
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientDoesViewerEyeSurfaceWantDistortion(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_CBool *distortionRequested) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_SURFACE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(distortionRequested, "distortion request");
    *distortionRequested =
        disp->cfg->getViewerEyeSurface(viewer, eye, surface).wantDistortion()
            ? OSVR_TRUE
            : OSVR_FALSE;
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetViewerEyeSurfaceRadialDistortionPriority(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_DistortionPriority *priority) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_SURFACE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(priority, "distortion technique priority");
    *priority = disp->cfg->getViewerEyeSurface(viewer, eye, surface)
                    .getRadialDistortionPriority();
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetViewerEyeSurfaceRadialDistortion(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_RadialDistortionParameters *params) {
    OSVR_VALIDATE_DISPLAY_CONFIG;
    OSVR_VALIDATE_VIEWER_ID;
    OSVR_VALIDATE_EYE_ID;
    OSVR_VALIDATE_SURFACE_ID;
    OSVR_VALIDATE_OUTPUT_PTR(params, "radial distortion parameter structure");
    auto optParams = disp->cfg->getViewerEyeSurface(viewer, eye, surface)
                         .getRadialDistortionParams();
    if (optParams.is_initialized()) {
        *params = *optParams;
        return OSVR_RETURN_SUCCESS;
    }
    return OSVR_RETURN_FAILURE;
}
