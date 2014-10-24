/** @file
    @brief Implementation

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
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
#include <ogvr/PluginKit/PluginRegistrationC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

static int sampleHardwareUserdata = 1;

static OGVR_PluginReturnCode
pollForHardwareChange(OGVRPluginHardwarePollContext /*pollContext*/,
                      void *userData) {
    int &data = *static_cast<int *>(userData);
    std::cout << "Got a poll for hardware change, with user data " << data
              << std::endl;
    return OGVR_PLUGIN_SUCCESS;
}

OGVR_PLUGIN(org_opengoggles_example_DummyHardwarePoll) {
    /// Register a polling callback, with some dummy userdata.
    ogvrPluginRegisterHardwarePollCallback(ctx, &pollForHardwareChange,
                                           &sampleHardwareUserdata);

    return OGVR_PLUGIN_SUCCESS;
}
