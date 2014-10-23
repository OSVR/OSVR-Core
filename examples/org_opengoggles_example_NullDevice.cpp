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
#include <ogvr/Util/GenericDeleter.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

class DummyDevice {
  public:
    DummyDevice() { std::cout << "Constructing dummy device" << std::endl; }

    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
};

OGVR_PLUGIN(org_opengoggles_example_NullDevice) {

    /// Create a "device" that actually does nothing.
    DummyDevice *myDevice = new DummyDevice();
    /// Must ask the core to tell us to delete it.
    ogvrPluginRegisterDataWithDeleteCallback(
        ctx, &ogvr::detail::generic_deleter<DummyDevice>,
        static_cast<void *>(myDevice));

    return OGVR_PLUGIN_SUCCESS;
}
