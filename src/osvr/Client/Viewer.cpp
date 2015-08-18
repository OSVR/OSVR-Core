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
#include <osvr/Client/Viewer.h>
#include <osvr/Common/ReportTypes.h>
#include <osvr/Common/ClientInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    Viewer::Viewer(OSVR_ClientContext ctx, const char path[])
        : m_head(ctx, path) {}

    OSVR_Pose3 Viewer::getPose() const {
        OSVR_TimeValue timestamp;
        OSVR_Pose3 pose;
        bool hasState = m_head->getState<OSVR_PoseReport>(timestamp, pose);
        if (!hasState) {
            throw NoPoseYet();
        }
        return pose;
    }

    bool Viewer::hasPose() const {
        return m_head->hasStateForReportType<OSVR_PoseReport>();
    }

} // namespace client
} // namespace osvr
