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
#include <osvr/PluginKit/PluginRegistrationC.h>
#include <osvr/Util/GenericDeleter.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {

class DummyDevice {
  public:
    DummyDevice() { std::cout << "Constructing dummy device" << std::endl; }

    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
};
} // namespace

OSVR_PLUGIN(org_opengoggles_example_NullDevice) {

    /// Create a "device" that actually does nothing.
    DummyDevice *myDevice = new DummyDevice();
    /// Must ask the core to tell us to delete it.
    osvrPluginRegisterDataWithDeleteCallback(
        ctx, &osvr::util::generic_deleter<DummyDevice>,
        static_cast<void *>(myDevice));

    return OSVR_RETURN_SUCCESS;
}
