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

#ifndef INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C
#define INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C

// Internal Includes
#include <osvr/ClientKit/DisplayC.h>
#include <osvr/ClientKit/CppAPI.h>
#include <osvr/ClientKit/Context.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

/// @name Overloads taking output parameters by reference
/// @{
inline OSVR_ReturnCode osvrClientGetNumViewers(OSVR_DisplayConfig disp,
                                               OSVR_ViewerCount &viewers) {
    return osvrClientGetNumViewers(disp, &viewers);
}

inline OSVR_ReturnCode osvrClientGetNumEyesForViewer(OSVR_DisplayConfig disp,
                                                     OSVR_ViewerCount viewer,
                                                     OSVR_EyeCount &eyes) {
    return osvrClientGetNumEyesForViewer(disp, viewer, &eyes);
}

inline OSVR_ReturnCode
osvrClientGetNumSurfacesForViewerEye(OSVR_DisplayConfig disp,
                                     OSVR_ViewerCount viewer, OSVR_EyeCount eye,
                                     OSVR_SurfaceCount &surfaces) {
    return osvrClientGetNumSurfacesForViewerEye(disp, viewer, eye, &surfaces);
}

inline OSVR_ReturnCode osvrClientGetRelativeViewportForViewerEyeSurface(
    OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye,
    OSVR_SurfaceCount surface, OSVR_ViewportDimension &left,
    OSVR_ViewportDimension &bottom, OSVR_ViewportDimension &width,
    OSVR_ViewportDimension &height) {
    return osvrClientGetRelativeViewportForViewerEyeSurface(
        disp, viewer, eye, surface, &left, &bottom, &width, &height);
}

/// @todo ensure we have wrappers for all the functions.
/// @}

namespace osvr {
namespace clientkit {
/// Define OSVR_CLIENTKIT_CUSTOM_DISPLAY_ERROR_HANDLER and provide your own
/// implementation of void osvr::clientkit::handleDisplayError(const char msg[])
/// if you don't want exceptions thrown.
#ifndef OSVR_CLIENTKIT_CUSTOM_DISPLAY_ERROR_HANDLER
    static inline void handleDisplayError(const char msg[]) {
        throw std::runtime_error(msg);
    }
#endif

    /// @addtogroup ClientKitCPP
    /// @{
    typedef shared_ptr<OSVR_DisplayConfigObject> UnderlyingDisplayConfigPtr;

    /// @brief Get a shared_ptr to a DisplayConfig (with appropriate deleter
    /// pre-loaded)
    inline UnderlyingDisplayConfigPtr getDisplay(ClientContext &ctx) {
        OSVR_DisplayConfig cfg = NULL;
        osvrClientGetDisplay(ctx.get(), &cfg);
        UnderlyingDisplayConfigPtr ret(cfg, &::osvrClientFreeDisplay);
        return ret;
    }

    struct RelativeViewport {
        OSVR_ViewportDimension left;
        OSVR_ViewportDimension bottom;
        OSVR_ViewportDimension width;
        OSVR_ViewportDimension height;
    };

    struct ProjectionClippingPlanes {
        double left;
        double right;
        double top;
        double bottom;
    };

    struct DisplayDimensions {
        OSVR_DisplayDimension width;
        OSVR_DisplayDimension height;
    };

    /// @brief Wrapper for a viewer, eye, and surface bound to a display config.
    /// DOES NOT provide lifetime management for the associated display config!
    class Surface {
      public:
        Surface(OSVR_DisplayConfig disp, OSVR_ViewerCount viewer,
                OSVR_EyeCount eye, OSVR_SurfaceCount surface)
            : m_disp(disp), m_viewer(viewer), m_eye(eye), m_surface(surface) {}

        /// @brief Gets the video-input-relative viewport corresponding to this
        /// surface.
        ///
        /// @sa osvrClientGetRelativeViewportForViewerEyeSurface()
        RelativeViewport getRelativeViewport() const {
            RelativeViewport viewport;
            OSVR_ReturnCode ret =
                osvrClientGetRelativeViewportForViewerEyeSurface(
                    m_disp, m_viewer, m_eye, m_surface, viewport.left,
                    viewport.bottom, viewport.width, viewport.height);
            if (OSVR_RETURN_SUCCESS != ret) {
                handleDisplayError(
                    "Could not access relative viewport for surface!");
            }
            return viewport;
        }

        /// @brief Gets the projection matrix.
        ///
        /// @sa osvrClientGetViewerEyeSurfaceProjectionMatrixd
        void getProjectionMatrix(double near, double far,
                                 OSVR_MatrixConventions flags,
                                 double matrix[OSVR_MATRIX_SIZE]) const {
            OSVR_ReturnCode ret =
                osvrClientGetViewerEyeSurfaceProjectionMatrixd(
                    m_disp, m_viewer, m_eye, m_surface, near, far, flags,
                    matrix);
            if (OSVR_RETURN_SUCCESS != ret) {
                handleDisplayError(
                    "Could not access projection matrix for surface!");
            }
        }

        /// @overload
        void getProjectionMatrix(float near, float far,
                                 OSVR_MatrixConventions flags,
                                 float matrix[OSVR_MATRIX_SIZE]) const {
            OSVR_ReturnCode ret =
                osvrClientGetViewerEyeSurfaceProjectionMatrixf(
                    m_disp, m_viewer, m_eye, m_surface, near, far, flags,
                    matrix);
            if (OSVR_RETURN_SUCCESS != ret) {
                handleDisplayError(
                    "Could not access projection matrix for surface!");
            }
        }

        /// @brief Gets the clipping planes.
        ///
        /// @sa osvrClientGetViewerEyeSurfaceProjectionClippingPlanes
        ProjectionClippingPlanes getProjectionClippingPlanes() const {
            ProjectionClippingPlanes clippingPlanes;
            OSVR_ReturnCode ret =
                osvrClientGetViewerEyeSurfaceProjectionClippingPlanes(
                    m_disp, m_viewer, m_eye, m_surface, &clippingPlanes.left,
                    &clippingPlanes.right, &clippingPlanes.bottom, &clippingPlanes.top);
            if (OSVR_RETURN_SUCCESS != ret) {
                handleDisplayError(
                    "Could not access projection clipping planes for surface!");
            }
            return clippingPlanes;
        }

        /// @brief Determines if the description of the surface requests any
        /// distortion to be performed.
        ///
        /// @sa osvrClientDoesViewerEyeSurfaceWantDistortion()
        bool isDistortionRequested() const {
            OSVR_CBool distort;
            OSVR_ReturnCode ret = osvrClientDoesViewerEyeSurfaceWantDistortion(
                m_disp, m_viewer, m_eye, m_surface, &distort);
            if (OSVR_RETURN_SUCCESS != ret) {
                handleDisplayError(
                    "Could not get distortion request for surface!");
            }
            return (distort == OSVR_TRUE);
        }

        /// @brief Get the priority/availability of radial distortion parameters
        /// for this surface. Negative values (canonically,
        /// OSVR_DISTORTION_PRIORITY_UNAVAILABLE) indicate that this distortion
        /// strategy is not available/parameterized for the surface in question.
        /// Larger values indicate more highly-preferred distortion strategies.
        ///
        /// @sa osvrClientGetViewerEyeSurfaceRadialDistortionPriority()
        OSVR_DistortionPriority getRadialDistortionPriority() {
            OSVR_DistortionPriority prio;
            OSVR_ReturnCode ret =
                osvrClientGetViewerEyeSurfaceRadialDistortionPriority(
                    m_disp, m_viewer, m_eye, m_surface, &prio);

            return prio;
        }

        /// @brief Get the radial distortion parameters.
        ///
        /// Will only succeed if getRadialDistortionPriority() is non-negative.
        ///
        /// @sa osvrClientGetViewerEyeSurfaceRadialDistortionPriority()
        OSVR_RadialDistortionParameters getRadialDistortion() {
            OSVR_RadialDistortionParameters params;
            OSVR_ReturnCode ret = osvrClientGetViewerEyeSurfaceRadialDistortion(
                m_disp, m_viewer, m_eye, m_surface, &params);
            if (OSVR_RETURN_SUCCESS != ret) {
                handleDisplayError(
                    "Could not get radial distortion params for surface!");
            }
            return params;
        }
        /// @name Identification getters
        /// @{
        OSVR_DisplayConfig getDisplayConfig() const { return m_disp; }
        OSVR_ViewerCount getViewerID() const { return m_viewer; }
        OSVR_EyeCount getEyeID() const { return m_eye; }
        OSVR_SurfaceCount getSurfaceID() const { return m_surface; }
        /// @}
      private:
        OSVR_DisplayConfig m_disp;
        OSVR_ViewerCount m_viewer;
        OSVR_EyeCount m_eye;
        OSVR_SurfaceCount m_surface;
    };

    /// @brief Wrapper for a viewer and eye bound to a display config.
    /// DOES NOT provide lifetime management for the associated display config!
    class Eye {
      public:
        Eye(OSVR_DisplayConfig disp, OSVR_ViewerCount viewer, OSVR_EyeCount eye)
            : m_disp(disp), m_viewer(viewer), m_eye(eye) {
            if (!m_disp) {
                handleDisplayError("Can't interact with an eye using a "
                                   "null/invalid display config");
            }
        }

        /// @name Child-related methods
        /// @{
        OSVR_SurfaceCount getNumSurfaces() const {
            OSVR_SurfaceCount surfaces;
            OSVR_ReturnCode ret = osvrClientGetNumSurfacesForViewerEye(
                m_disp, m_viewer, m_eye, &surfaces);
            if (ret != OSVR_RETURN_SUCCESS) {
                handleDisplayError("Couldn't get number of surfaces in this "
                                   "display for the given viewer and eye!");
            }
            return surfaces;
        }

        Surface getSurface(OSVR_SurfaceCount surface) const {
            return Surface(m_disp, m_viewer, m_eye, surface);
        }
        /// @}

        /// @brief Attempt to get the eye pose.
        ///
        /// @return false if there was an error in the input parameters or if no
        /// pose is yet available
        bool getPose(OSVR_Pose3 &pose) {
            OSVR_ReturnCode ret =
                osvrClientGetViewerEyePose(m_disp, m_viewer, m_eye, &pose);
            return (ret == OSVR_RETURN_SUCCESS);
        }

        /// @brief Attempt to get the view matrix.
        ///
        /// @return false if there was an error in the input parameters or if no
        /// pose (and thus view) is yet available.
        bool getViewMatrix(OSVR_MatrixConventions flags,
                           double mat[OSVR_MATRIX_SIZE]) {
            OSVR_ReturnCode ret = osvrClientGetViewerEyeViewMatrixd(
                m_disp, m_viewer, m_eye, flags, mat);
            return (ret == OSVR_RETURN_SUCCESS);
        }

        /// @overload
        bool getViewMatrix(OSVR_MatrixConventions flags,
                           float mat[OSVR_MATRIX_SIZE]) {
            OSVR_ReturnCode ret = osvrClientGetViewerEyeViewMatrixf(
                m_disp, m_viewer, m_eye, flags, mat);
            return (ret == OSVR_RETURN_SUCCESS);
        }

        /// @name Iteration methods
        /// @{
        template <typename F>
        void forEachSurface(F OSVR_CLIENTKIT_FUNCTOR_REF functor) {
            /// For each display surface seen by the given eye of the given
            /// viewer...
            OSVR_SurfaceCount surfaces = getNumSurfaces();
            for (OSVR_SurfaceCount surface = 0; surface < surfaces; ++surface) {
                functor(getSurface(surface));
            }
        }
        /// @}

        /// @name Identification getters
        /// @{
        OSVR_DisplayConfig getDisplayConfig() const { return m_disp; }
        OSVR_ViewerCount getViewerID() const { return m_viewer; }
        OSVR_EyeCount getEyeID() const { return m_eye; }
        /// @}
      private:
        OSVR_DisplayConfig m_disp;
        OSVR_ViewerCount m_viewer;
        OSVR_EyeCount m_eye;
    };

    /// @brief Wrapper for a viewer bound to a display config.
    /// DOES NOT provide lifetime management for the associated display config!
    class Viewer {
      public:
        Viewer(OSVR_DisplayConfig disp, OSVR_ViewerCount viewer)
            : m_disp(disp), m_viewer(viewer) {
            if (!m_disp) {
                handleDisplayError("Can't interact with a viewer using a "
                                   "null/invalid display config");
            }
        }

        /// @name Child-related methods
        /// @{
        OSVR_EyeCount getNumEyes() const {
            OSVR_EyeCount eyes;
            OSVR_ReturnCode ret =
                osvrClientGetNumEyesForViewer(m_disp, m_viewer, &eyes);
            if (ret != OSVR_RETURN_SUCCESS) {
                handleDisplayError("Couldn't get number of eyes in this "
                                   "display for the given viewer!");
            }
            return eyes;
        }

        Eye getEye(OSVR_EyeCount eye) const {
            return Eye(m_disp, m_viewer, eye);
        }
        Surface getSurface(OSVR_EyeCount eye, OSVR_SurfaceCount surface) const {
            return Surface(m_disp, m_viewer, eye, surface);
        }
        /// @}

        /// @brief Attempt to get the viewer pose.
        ///
        /// Note that there may not necessarily be any surfaces rendered from
        /// this pose (it's the unused "center" eye in a stereo configuration)
        /// so only use this if it makes integration into your engine or
        /// existing applications (not originally designed for stereo) easier.
        ///
        /// @return false if there was an error in the input parameters or if no
        /// pose is yet available
        bool getPose(OSVR_Pose3 &pose) {
            OSVR_ReturnCode ret =
                osvrClientGetViewerPose(m_disp, m_viewer, &pose);
            return (ret == OSVR_RETURN_SUCCESS);
        }

        /// @name Iteration methods
        /// @{
        template <typename F>
        void forEachEye(F OSVR_CLIENTKIT_FUNCTOR_REF functor) {
            /// For each eye of the given viewer...
            OSVR_EyeCount eyes = getNumEyes();
            for (OSVR_EyeCount eye = 0; eye < eyes; ++eye) {
                functor(getEye(eye));
            }
        }
        template <typename F> void forEachSurface(F functor) {
            /// For each eye of the given viewer...
            OSVR_EyeCount eyes = getNumEyes();
            for (OSVR_EyeCount eye = 0; eye < eyes; ++eye) {
                getEye(eye).forEachSurface(functor);
            }
        }
        ///}

        /// @name Identification getters
        /// @{
        OSVR_DisplayConfig getDisplayConfig() const { return m_disp; }
        OSVR_ViewerCount getViewerID() const { return m_viewer; }
        /// @}
      private:
        OSVR_DisplayConfig m_disp;
        OSVR_ViewerCount m_viewer;
    };

    /// @brief Class wrapping OSVR_DisplayConfig objects, optionally managing
    /// shared ownership.
    class DisplayConfig {
      public:
        /// @brief Empty constructor - constructs a config where valid() is
        /// false
        DisplayConfig() : m_disp(NULL) {}

        /// @brief Retrieve a display config, owning it and managing its
        /// lifetime
        explicit DisplayConfig(ClientContext &ctx)
            : m_disp(NULL), m_owningDisp(osvr::clientkit::getDisplay(ctx)) {
            if (m_owningDisp) {
                m_disp = m_owningDisp.get();
            }
        }

        /// @brief Construct from a shared pointer to the underlying display
        /// config, taking shared ownership.
        explicit DisplayConfig(UnderlyingDisplayConfigPtr const &ptr)
            : m_disp(NULL), m_owningDisp(ptr) {
            if (m_owningDisp) {
                m_disp = m_owningDisp.get();
            }
        }

        /// @brief Construct from a (presumably-externally-managed) raw display
        /// config, not affecting lifetime.
        explicit DisplayConfig(OSVR_DisplayConfig disp) : m_disp(disp) {}

        /// @brief Checks the validity of the contained pointer.
        bool valid() const { return m_disp != NULL; }

        DisplayConfig &ensureValid() {
            if (!valid()) {
                handleDisplayError(
                    "Can't operate on an invalid DisplayConfig!");
            }
            return *this;
        }
        DisplayConfig const &ensureValid() const {
            if (!valid()) {
                handleDisplayError(
                    "Can't operate on an invalid DisplayConfig!");
            }
            return *this;
        }

        /// @brief Checks if the display is fully configured and ready,
        /// including first pose.
        /// @sa osvrClientCheckDisplayStartup()
        bool checkStartup() const {
            ensureValid();
            return osvrClientCheckDisplayStartup(m_disp) == OSVR_RETURN_SUCCESS;
        }

        /// @brief Returns number of inputs for this display.
        ///
        /// @sa osvrClientGetNumDisplayInputs()
        OSVR_DisplayInputCount getNumDisplayInputs() const {
            ensureValid();
            OSVR_DisplayInputCount inputs;
            OSVR_ReturnCode ret = osvrClientGetNumDisplayInputs(m_disp, &inputs);
            if (ret != OSVR_RETURN_SUCCESS) {
                handleDisplayError("Couldn't get number of display inputs!");
            }
            return inputs;
        }

        /// @brief Returns display dimensions for a display input.
        ///
        /// @sa osvrClientGetDisplayDimensions()
        osvr::clientkit::DisplayDimensions getDisplayDimensions(OSVR_DisplayInputCount displayInputIndex) const {
            ensureValid();
            DisplayDimensions dimensions;
            OSVR_ReturnCode ret = osvrClientGetDisplayDimensions(m_disp, displayInputIndex, &dimensions.width, &dimensions.height);
            if (ret != OSVR_RETURN_SUCCESS) {
                handleDisplayError("Couldn't get dimensions of this "
                                   "display!");
            }
            return dimensions;
        }

        /// @name Child-related methods
        /// @{
        OSVR_ViewerCount getNumViewers() const {
            ensureValid();
            OSVR_ViewerCount viewers;
            OSVR_ReturnCode ret = osvrClientGetNumViewers(m_disp, &viewers);
            if (ret != OSVR_RETURN_SUCCESS) {
                handleDisplayError("Couldn't get number of viewers in this "
                                   "display!");
            }
            return viewers;
        }

        Viewer getViewer(OSVR_ViewerCount viewer) const {
            ensureValid();
            return Viewer(m_disp, viewer);
        }

        Eye getEye(OSVR_ViewerCount viewer, OSVR_EyeCount eye) const {
            ensureValid();
            return Eye(m_disp, viewer, eye);
        }

        Surface getSurface(OSVR_ViewerCount viewer, OSVR_EyeCount eye,
                           OSVR_SurfaceCount surface) const {
            ensureValid();
            return Surface(m_disp, viewer, eye, surface);
        }

        /// @}

        /// @name Iteration methods
        /// @{
        template <typename F>
        void forEachViewer(F OSVR_CLIENTKIT_FUNCTOR_REF functor) {
            /// For each viewer...
            OSVR_ViewerCount viewers = getNumViewers();
            for (OSVR_ViewerCount viewer = 0; viewer < viewers; ++viewer) {
                functor(getViewer(viewer));
            }
        }

        template <typename F>
        void forEachEye(F OSVR_CLIENTKIT_FUNCTOR_REF functor) {
            /// For each viewer...
            OSVR_ViewerCount viewers = getNumViewers();
            for (OSVR_ViewerCount viewer = 0; viewer < viewers; ++viewer) {
                getViewer(viewer).forEachEye(functor);
            };
        }

        template <typename F>
        void forEachSurface(F OSVR_CLIENTKIT_FUNCTOR_REF functor) {
            /// For each viewer...
            OSVR_ViewerCount viewers = getNumViewers();
            for (OSVR_ViewerCount viewer = 0; viewer < viewers; ++viewer) {
                getViewer(viewer).forEachSurface(functor);
            };
        }

        ///}

        /// @name Identification getters
        /// @{

        /// @brief Gets the raw OSVR_DisplayConfig
        OSVR_DisplayConfig getDisplayConfig() const { return m_disp; }

        /// @brief Gets a copy of the owning pointer, if we hold shared
        /// ownership over this DisplayConfig.
        UnderlyingDisplayConfigPtr getOwningPtr() const { return m_owningDisp; }

        /// @}
      private:
        OSVR_DisplayConfig m_disp;
        UnderlyingDisplayConfigPtr m_owningDisp;
    };

    /// @brief Equality operator for clientkit::Surface
    /// @relates clientkit::Surface
    inline bool operator==(Surface const &lhs, Surface const &rhs) {
        return (lhs.getSurfaceID() == rhs.getSurfaceID()) &&
               (lhs.getEyeID() == rhs.getEyeID()) &&
               (lhs.getViewerID() == rhs.getViewerID()) &&
               (lhs.getDisplayConfig() == rhs.getDisplayConfig());
    }

    /// @brief Inequality operator for clientkit::Surface
    /// @relates clientkit::Surface
    inline bool operator!=(Surface const &lhs, Surface const &rhs) {
        return (lhs.getSurfaceID() != rhs.getSurfaceID()) ||
               (lhs.getEyeID() != rhs.getEyeID()) ||
               (lhs.getViewerID() != rhs.getViewerID()) ||
               (lhs.getDisplayConfig() != rhs.getDisplayConfig());
    }

    /// @brief Equality operator for clientkit::Eye
    /// @relates clientkit::Eye
    inline bool operator==(Eye const &lhs, Eye const &rhs) {
        return (lhs.getEyeID() == rhs.getEyeID()) &&
               (lhs.getViewerID() == rhs.getViewerID()) &&
               (lhs.getDisplayConfig() == rhs.getDisplayConfig());
    }

    /// @brief Inequality operator for clientkit::Eye
    /// @relates clientkit::Eye
    inline bool operator!=(Eye const &lhs, Eye const &rhs) {
        return (lhs.getEyeID() != rhs.getEyeID()) ||
               (lhs.getViewerID() != rhs.getViewerID()) ||
               (lhs.getDisplayConfig() != rhs.getDisplayConfig());
    }

    /// @brief Equality operator for clientkit::Viewer
    /// @relates clientkit::Viewer
    inline bool operator==(Viewer const &lhs, Viewer const &rhs) {
        return (lhs.getViewerID() == rhs.getViewerID()) &&
               (lhs.getDisplayConfig() == rhs.getDisplayConfig());
    }

    /// @brief Inequality operator for clientkit::Viewer
    /// @relates clientkit::Viewer
    inline bool operator!=(Viewer const &lhs, Viewer const &rhs) {
        return (lhs.getViewerID() != rhs.getViewerID()) ||
               (lhs.getDisplayConfig() != rhs.getDisplayConfig());
    }

    /// @brief Equality operator for clientkit::DisplayConfig - does not
    /// consider the ownership status in the comparison.
    /// @relates clientkit::DisplayConfig
    inline bool operator==(DisplayConfig const &lhs, DisplayConfig const &rhs) {
        return lhs.getDisplayConfig() == rhs.getDisplayConfig();
    }

    /// @brief Equality operator for clientkit::DisplayConfig
    /// @relates clientkit::DisplayConfig
    inline bool operator==(DisplayConfig const &lhs, OSVR_DisplayConfig rhs) {
        return lhs.getDisplayConfig() == rhs;
    }

    /// @brief Equality operator for clientkit::DisplayConfig
    /// @relates clientkit::DisplayConfig
    inline bool operator==(OSVR_DisplayConfig lhs, DisplayConfig const &rhs) {
        return lhs == rhs.getDisplayConfig();
    }

    /// @brief Inequality operator for clientkit::DisplayConfig - does not
    /// consider the ownership status in the comparison.
    /// @relates clientkit::DisplayConfig
    inline bool operator!=(DisplayConfig const &lhs, DisplayConfig const &rhs) {
        return lhs.getDisplayConfig() != rhs.getDisplayConfig();
    }

    /// @brief Inequality operator for clientkit::DisplayConfig
    /// @relates clientkit::DisplayConfig
    inline bool operator!=(DisplayConfig const &lhs, OSVR_DisplayConfig rhs) {
        return lhs.getDisplayConfig() != rhs;
    }

    /// @brief Inequality operator for clientkit::DisplayConfig
    /// @relates clientkit::DisplayConfig
    inline bool operator!=(OSVR_DisplayConfig lhs, DisplayConfig const &rhs) {
        return lhs != rhs.getDisplayConfig();
    }

    /// @}
    // end of group

} // namespace clientkit
} // namespace osvr
#endif // INCLUDED_Display_h_GUID_E15D0209_6784_4CBF_4F89_788E7A4B440C
