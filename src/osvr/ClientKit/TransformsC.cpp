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
#include <osvr/ClientKit/TransformsC.h>
#include <osvr/ClientKit/InterfaceStateC.h>
#include <osvr/ClientKit/InterfaceStateC.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/Transform.h>
#include <osvr/Client/InternalInterfaceOwner.h>
#include <osvr/Util/EigenInterop.h>

// Library/third-party includes
// - none

// Standard includes
#include <chrono>

OSVR_ReturnCode osvrClientSetRoomRotationUsingHead(OSVR_ClientContext ctx) {
    if (nullptr == ctx) {
        /// Return failure if given a null context
        return OSVR_RETURN_FAILURE;
    }
    auto xform = ctx->getRoomToWorldTransform();

    auto iface = osvr::client::InternalInterfaceOwner{ctx, "/me/head"};
    OSVR_OrientationState state;
    OSVR_TimeValue t;
    /// Give ourselves 200 milliseconds to get a head tracker report.
    using std::chrono::system_clock;
    const auto deadline = system_clock::now() + std::chrono::milliseconds(200);
    do {
        if (OSVR_RETURN_SUCCESS ==
            osvrGetOrientationState(&(*iface), &t, &state)) {
            // OK, we've gotten state successfully: extract yaw, update
            // transform, and return success.
            auto q = osvr::util::eigen_interop::map(state);

            // see
            // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/index.htm
            auto yaw = std::atan2(2 * (q.y() * q.w() - q.x() * q.z()),
                                  1 - 2 * q.y() * q.y() - 2 * q.z() * q.z());

            Eigen::AngleAxisd correction(-yaw, Eigen::Vector3d::UnitY());
            xform.concatPost(Eigen::Isometry3d(correction).matrix());
            ctx->setRoomToWorldTransform(xform);
            return OSVR_RETURN_SUCCESS;
        }
        ctx->update();
    } while (system_clock::now() < deadline);
    return OSVR_RETURN_FAILURE;
}

OSVR_ReturnCode osvrClientClearRoomToWorldTransform(OSVR_ClientContext ctx) {
    if (nullptr == ctx) {
        /// Return failure if given a null context
        return OSVR_RETURN_FAILURE;
    }
    osvr::common::Transform nullTransform;
    ctx->setRoomToWorldTransform(nullTransform);
    return OSVR_RETURN_SUCCESS;
}
