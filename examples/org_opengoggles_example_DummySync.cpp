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
#include <ogvr/PluginKit/PluginInterfaceC.h>
#include <ogvr/Util/GenericDeleter.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

class DummySyncDevice {
  public:
    DummySyncDevice(OGVRDevice d) : m_dev(d) {
        std::cout << "Constructing dummy synchronous device" << std::endl;
    }

    /// Trampoline: C-compatible callback bouncing into a member function.
    /// Also something we can wrap.
    static OGVRPluginReturnCode update(void *userData) {
        return static_cast<DummySyncDevice *>(userData)->m_update();
    }
    ~DummySyncDevice() {
        std::cout << "Destroying dummy synchronous device" << std::endl;
    }

  private:
    OGVRPluginReturnCode m_update() {
        // get some data
        char *mydata = NULL;
        ogvrDeviceSendData(m_dev, mydata, 0);
        return OGVR_PLUGIN_SUCCESS;
    }
    OGVRDevice m_dev;
};

OGVR_PLUGIN(org_opengoggles_example_DummySync) {
    /// Create a synchronous (in the mainloop) device
    OGVRDevice d;
    ogvrDeviceSyncInit(ctx, "My Sync Device",
                       &d); // Puts a token in d that knows it's a sync
                            // device so ogvrDeviceSendData knows that it
                            // doesn't need to acquire a lock.
    DummySyncDevice *mySync = new DummySyncDevice(d);
    ogvrPluginRegisterDataWithDeleteCallback(
        ctx, &ogvr::detail::generic_deleter<DummySyncDevice>,
        static_cast<void *>(mySync));
    ogvrDeviceSyncRegisterUpdateCallback(d, &DummySyncDevice::update,
                                         static_cast<void *>(mySync));
    return OGVR_PLUGIN_SUCCESS;
}
