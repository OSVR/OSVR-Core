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
#include "PluginSpecificRegistrationContextImpl.h"
#include <osvr/PluginHost/PluginSpecificRegistrationContext.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace pluginhost {

    PluginSpecificRegistrationContext::~PluginSpecificRegistrationContext() {}

    PluginRegPtr
    PluginSpecificRegistrationContext::create(std::string const &name) {
        PluginRegPtr pluginReg =
            make_shared<PluginSpecificRegistrationContextImpl>(name);
        return pluginReg;
    }

    PluginSpecificRegistrationContext &
    PluginSpecificRegistrationContext::get(OSVR_PluginRegContext ctx) {
        return *static_cast<PluginSpecificRegistrationContext *>(ctx);
    }

    OSVR_PluginRegContext
    PluginSpecificRegistrationContext::extractOpaquePointer() {
        return static_cast<OSVR_PluginRegContext>(this);
    }

    const std::string &PluginSpecificRegistrationContext::getName() const {
        return m_name;
    }
    PluginSpecificRegistrationContext::PluginSpecificRegistrationContext(
        std::string const &name)
        : m_name(name) {}

} // namespace pluginhost
} // namespace osvr
