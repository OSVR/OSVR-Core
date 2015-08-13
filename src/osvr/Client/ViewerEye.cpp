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
#include <osvr/Client/ViewerEye.h>
#include <osvr/Common/ReportTypes.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/ProjectionMatrix.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    OSVR_Pose3 ViewerEye::getPose() const {
        OSVR_TimeValue timestamp;
        OSVR_Pose3 pose;
        bool hasState = m_pose->getState<OSVR_PoseReport>(timestamp, pose);
        if (!hasState) {
            throw NoPoseYet();
        }
        Eigen::Isometry3d translatedPose =
            util::fromPose(pose) * Eigen::Translation3d(m_offset);
        util::toPose(translatedPose, pose);
        return pose;
    }

    Eigen::Matrix4d ViewerEye::getProjection(double near, double far) const {
        util::Rectd rect(m_unitBounds);
        // Scale the in-plane positions based on the near plane to put
        // the virtual viewing window on the near plane with the eye at the
        // origin.
        rect *= near;

        /// @todo Incorporate any rotation of the lenses around the eye Y axis
        // (such as the Oculus DK2) by shifting the boundaries.

        // Incorporate pitch_tilt (degrees, positive is downwards)
        // We assume that this results in a shearing of the image that leaves
        // the plane of the screen the same.
        if (m_pitchTilt != 0) {
            /// @todo
        }

        /// @todo Incorporate distortion (find the new boundaries so that
        // we will have something to put into every pixel once the
        // predistortion has been applied around the center of projection).

        /// @todo Incorporate extra space to enable us to shift the image
        /// after it has been rendered based on a new tracker report.

        // See if we need to rotate 180 degrees about Z.
        //
        // If so, do so by swapping each pair of edges, producing an upside-down
        // projection. We do this after all of the above shifts, producing an
        // image that matches but which is upside down.
        //
        // NOTE: This would adjust the distortion center of projection, but it
        // is assumed that we're doing this to make scan-out circuitry behave
        // rather than to change where the actual pixel location of the center
        // of projection is.
        if (m_rot180) {
            std::swap(rect[util::Rectd::LEFT], rect[util::Rectd::RIGHT]);
            std::swap(rect[util::Rectd::TOP], rect[util::Rectd::BOTTOM]);
        }

#if 0
        // Make sure that things won't blow up in the math below.
        if ((nearClip <= 0) || (farClip <= 0) || (nearClip == farClip) ||
            (left == right) || (top == bottom)) {
            return false;
        }
#endif

        /// @todo Figure out interactions between the above shifts and
        /// distortions and make sure to do them in the right order, or to
        /// adjust as needed to make them consistent when they are composed.
        return util::createProjectionMatrix(rect, near, far);
    }

    ViewerEye::ViewerEye(Viewer &viewer, OSVR_ClientContext ctx,
                         Eigen::Vector3d const &offset, const char path[],
                         Viewport &&viewport, util::Rectd &&unitBounds,
                         bool rot180, double pitchTilt)
        : m_pose(ctx, path), m_offset(offset), m_parent(viewer),
          m_viewport(viewport), m_unitBounds(unitBounds), m_rot180(rot180),
          m_pitchTilt(pitchTilt) {}

} // namespace client
} // namespace osvr
