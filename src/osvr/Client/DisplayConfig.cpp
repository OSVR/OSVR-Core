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
#include <osvr/Client/DisplayConfig.h>
#include "DisplayDescriptorSchema1.h"

// Library/third-party includes
#include <osvr/Util/EigenExtras.h>

// Standard includes
// - none

namespace osvr {
namespace client {
    DisplayConfigPtr DisplayConfigFactory::create(OSVR_ClientContext ctx) {
        DisplayConfigPtr cfg(new DisplayConfig(ctx));
        auto const descriptorString = ctx->getStringParameter("/display");
        auto desc = display_schema_1::DisplayDescriptor(descriptorString);
        cfg->m_viewers.emplace_back(Viewer(ctx, "/me/head"));
        auto &viewer = cfg->m_viewers.front();
        auto eyesDesc = desc.getEyes();
        if (eyesDesc.size() == 2) {
            // stereo
            Eigen::Vector3d offset =
                desc.getIPDMeters() / 2. * Eigen::Vector3d::UnitX();
        } else {
            // if (eyesDesc.size() == 1)
            // mono
        }
        return cfg;
    }
    DisplayConfig::DisplayConfig(OSVR_ClientContext ctx) {}
} // namespace client
} // namespace osvr
