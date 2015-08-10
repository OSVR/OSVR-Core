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

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <utility>

namespace {
class InterfaceOwner {
  public:
    /// @brief Empty constructor
    InterfaceOwner() : InterfaceOwner(nullptr) {}

    /// @brief Constructor
    InterfaceOwner(OSVR_ClientContext ctx, OSVR_ClientInterface iface = nullptr)
        : m_ctx(ctx), m_iface(iface) {}

    /// @brief Constructor
    InterfaceOwner(OSVR_ClientContext ctx, const char path[])
        : m_ctx(ctx), m_iface(nullptr) {
        osvrClientGetInterface(m_ctx, path, &m_iface);
    }

    /// @brief non-copyable
    InterfaceOwner(InterfaceOwner const &) = delete;

    /// @brief non-assignable
    InterfaceOwner &operator=(InterfaceOwner const &) = delete;

    /// @brief move constructible
    InterfaceOwner(InterfaceOwner &&other)
        : m_ctx(other.m_ctx), m_iface(nullptr) {
        std::swap(m_iface, other.m_iface);
    }

    /// @brief move-assignable
    InterfaceOwner &operator=(InterfaceOwner &&other) {
        if (&other == this) {
            return *this;
        }
        m_reset();
        std::swap(other.m_ctx, m_ctx);
        std::swap(other.m_iface, m_iface);
        return *this;
    }

    /// @brief Destructor - frees interface.
    ~InterfaceOwner() { m_reset(); }

  private:
    void m_reset() {
        if (m_ctx && m_iface) {
            osvrClientFreeInterface(m_ctx, m_iface);
            m_ctx = nullptr;
            m_iface = nullptr;
        }
    }
    OSVR_ClientContext m_ctx;
    OSVR_ClientInterface m_iface;
};
} // namespace

struct OSVR_DisplayConfigObject {
    OSVR_DisplayConfigObject(OSVR_ClientContext context)
        : ctx(context), head(ctx, "/me/head") {
        OSVR_DEV_VERBOSE("Created an OSVR_DisplayConfigObject!");
    }
    ~OSVR_DisplayConfigObject() {
        OSVR_DEV_VERBOSE("OSVR_DisplayConfigObject destructor");
    }
    OSVR_ClientContext ctx;
    InterfaceOwner head;
};

OSVR_ReturnCode osvrClientGetDisplay(OSVR_ClientContext ctx,
                                     OSVR_DisplayConfig *disp) {
    if (disp == nullptr) {
        OSVR_DEV_VERBOSE("Passed a null pointer to a display config!");
        return OSVR_RETURN_FAILURE;
    }
    if (ctx == nullptr) {
        OSVR_DEV_VERBOSE("Passed a null client context!");
        *disp = nullptr;
        return OSVR_RETURN_FAILURE;
    }

    auto config = std::make_shared<OSVR_DisplayConfigObject>(ctx);
    if (!config) {
    }
    ctx->acquireObject(config);
    *disp = config.get();
    return OSVR_RETURN_SUCCESS;
}

#define OSVR_VALIDATE_DISPLAY_CONFIG(X)                                        \
    if (X == nullptr) {                                                        \
        OSVR_DEV_VERBOSE("Passed a null display config!");                     \
        return OSVR_RETURN_FAILURE;                                            \
    }

OSVR_ReturnCode osvrClientFreeDisplay(OSVR_DisplayConfig disp) {
    OSVR_VALIDATE_DISPLAY_CONFIG(disp);
    OSVR_ClientContext ctx = disp->ctx;
    BOOST_ASSERT_MSG(
        ctx != nullptr,
        "Should never get a display config object with a null context in it.");
    auto freed = ctx->releaseObject(disp);
    return freed ? OSVR_RETURN_SUCCESS : OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientGetNumViewers(OSVR_DisplayConfig disp,
                                        OSVR_ViewerCount *viewers) {
    OSVR_VALIDATE_DISPLAY_CONFIG(disp);
    if (nullptr == viewers) {
        OSVR_DEV_VERBOSE("Passed a null pointer to viewer count!");
        return OSVR_RETURN_FAILURE;
    }
    /// @todo might not always be exactly 1 viewer
    *viewers = 1;
}

OSVR_ReturnCode osvrClientGetNumEyesForViewer(OSVR_DisplayConfig disp,
                                              OSVR_ViewerCount viewer,
                                              OSVR_EyeCount *eyes) {
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode
osvrClientGetViewerEyePose(OSVR_DisplayConfig disp, OSVR_ViewerCount viewer,
OSVR_EyeCount eye, OSVR_Pose3 *pose) {
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode
osvrClientGetNumSurfacesForViewerEye(OSVR_DisplayConfig disp,
                                     OSVR_ViewerCount viewer, OSVR_EyeCount eye,
                                     OSVR_SurfaceCount *surfaces) {
    return OSVR_RETURN_FAILURE;
}
