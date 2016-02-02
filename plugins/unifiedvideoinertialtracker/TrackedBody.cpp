/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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
#include "TrackedBody.h"
#include "TrackedBodyIMU.h"
#include "TrackedBodyTarget.h"
#include "TrackingSystem.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    struct TrackedBody::ImplData {};
    TrackedBody::TrackedBody(TrackingSystem &system, BodyId id)
        : m_system(system), m_id(id), m_data(new ImplData) {}

    TrackedBody::~TrackedBody() {}

    TrackedBodyIMU *TrackedBody::createIntegratedIMU() {
        /// @todo
        return nullptr;
    }

    TrackedBodyTarget *
    TrackedBody::createTarget(Eigen::Isometry3d const &targetToBody,
                              TargetSetupData const &setupData) {
        if (m_target) {
            // already have a target!
            /// @todo handle multiple targets!
            return nullptr;
        }
        /// The target will always be target 0...
        m_target.reset(
            new TrackedBodyTarget(*this, targetToBody, setupData, TargetId{0}));
        return m_target.get();
    }

    ConfigParams const &TrackedBody::getParams() const {
        return m_system.getParams();
    }

    BodyId TrackedBody::getId() const {
#if 0
        return m_system.getIdForBody(*this);
#endif
        return m_id;
    }

} // namespace vbtracker
} // namespace osvr
