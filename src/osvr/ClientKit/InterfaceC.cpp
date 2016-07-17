/** @file
    @brief Implementation

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/Tracing.h>
#include <osvr/Common/SystemComponent.h>
#include <osvr/Util/StringIds.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_ReturnCode osvrClientGetInterface(OSVR_ClientContext ctx,
                                       const char path[],
                                       OSVR_ClientInterface *iface) {
    if (nullptr == ctx) {
        /// Return failure if given a null context
        return OSVR_RETURN_FAILURE;
    }
    osvr::common::tracing::markGetInterface(path);
    osvr::common::ClientInterfacePtr ret = ctx->getInterface(path);
    if (ret) {
        *iface = ret.get();
        return OSVR_RETURN_SUCCESS;
    }
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientFreeInterface(OSVR_ClientContext ctx,
                                        OSVR_ClientInterface iface) {
    if (nullptr == ctx) {
        /// Return failure if given a null context
        return OSVR_RETURN_FAILURE;
    }
    if (nullptr == iface) {
        /// Return failure if given a null interface
        return OSVR_RETURN_FAILURE;
    }

    /// This call returns a smart pointer - going to let it go out of scope
    /// here to delete.
    osvr::common::ClientInterfacePtr ptr(ctx->releaseInterface(iface));
    if (!ptr) {
        /// Return failure if the context didn't have a record of this
        /// interface.
        return OSVR_RETURN_FAILURE;
    }
    return OSVR_RETURN_SUCCESS;
}

OSVR_CLIENTKIT_EXPORT OSVR_ReturnCode osvrClientGetGestureNameLength(
    OSVR_ClientContext ctx, uint32_t id, size_t *len) {

    if (ctx == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    if (len == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    /// @todo Shouldn't be hardcoded as the only registered string map - this
    /// should be accessible as "interface-class-level" data for Gesture.
    std::string entryName =
        ctx->getSystemComponent()->getGestureMap()->corrMap.getStringFromId(
            osvr::util::StringID(id));
    *len = entryName.empty() ? 0 : (entryName.size() + 1);
    return OSVR_RETURN_SUCCESS;
}

OSVR_ReturnCode osvrClientGetGestureNameFromID(OSVR_ClientContext ctx,
                                               uint32_t id, char *buf,
                                               size_t len) {

    if (ctx == nullptr) {
        return OSVR_RETURN_FAILURE;
    }
    if (buf == nullptr) {
        return OSVR_RETURN_FAILURE;
    }

    /// @todo Shouldn't be hardcoded as the only registered string map - this
    /// should be accessible as "interface-class-level" data for Gesture.
    std::string entryName =
        ctx->getSystemComponent()->getGestureMap()->corrMap.getStringFromId(
            osvr::util::StringID(id));

    if (entryName.size() + 1 > len) {
        /// buffer too small.
        return OSVR_RETURN_FAILURE;
    }

    /// @todo refactor to eliminate duplication
    entryName.copy(buf, entryName.size());
    buf[entryName.size()] = '\0';
    return OSVR_RETURN_SUCCESS;
}
