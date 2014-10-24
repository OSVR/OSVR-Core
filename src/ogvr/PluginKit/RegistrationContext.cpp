/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>

*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <ogvr/PluginKit/RegistrationContext.h>
#include "PluginSpecificRegistrationContext.h"
#include "ResetPointerList.h"
#include <ogvr/Util/Verbosity.h>

// Library/third-party includes
#include <libfunctionality/LoadPlugin.h>

// Standard includes
#include <algorithm>

namespace ogvr {
RegistrationContext::RegistrationContext() {}

RegistrationContext::~RegistrationContext() {
    // Reset the plugins in reverse order.
    detail::resetPointerListReverseOrder(m_regList);
}
void RegistrationContext::loadPlugin(std::string const &pluginName) {
    PluginRegPtr pluginReg(new PluginSpecificRegistrationContext(pluginName));
    OGVR_DEV_VERBOSE("Plugin context created, loading plugin");
    libfunc::PluginHandle plugin =
        libfunc::loadPluginByName(pluginName, pluginReg.get());
    OGVR_DEV_VERBOSE("Plugin loaded, assuming ownership of plugin handle and "
                     "storing context");
    pluginReg->takePluginHandle(std::move(plugin));
    m_regList.push_back(pluginReg);
    OGVR_DEV_VERBOSE("Completed RegistrationContext::loadPlugin");
}

} // end of namespace ogvr
