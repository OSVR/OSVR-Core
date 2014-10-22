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
#include "GenericDeleter.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

class DummyAsyncDevice {
  public:
    DummyAsyncDevice(OGVRDevice d) : m_dev(d) {
        std::cout << "Constructing dummy asynchronous (threaded) device"
                  << std::endl;
    }
    /// Another trampoline.
    ///
    /// In this case, the core spawns a thread, with a loop calling this
    /// function
    /// as long as things are running. So this function waits for the next
    /// message from the device and passes it on.
    static OGVRPluginReturnCode wait(void *userData) {
        return static_cast<DummyAsyncDevice *>(userData)->m_wait();
    }
    ~DummyAsyncDevice() {
        std::cout << "Destroying dummy asynchronous (threaded) device"
                  << std::endl;
    }

  private:
    OGVRPluginReturnCode m_wait() {
        // block on waiting for data.
        // once we have enough, call
        char *mydata = NULL;
        ogvrDeviceSendData(m_dev, mydata, 0);
    }
    OGVRDevice m_dev;
};

OGVR_PLUGIN(org_opengoggles_example_DummyAsync) {
    /// Create an asynchronous (threaded) device
    OGVRDevice d;
    ogvrDeviceAsyncInit(ctx, "My Async Device",
                        &d); // Puts an object in d that knows it's a
                             // threaded device so ogvrDeviceSendData knows
                             // that it needs to get a connection lock first.
    DummyAsyncDevice *myAsync = new DummyAsyncDevice(d);
    ogvrPluginRegisterDataWithDeleteCallback(
        ctx, &generic_deleter<DummyAsyncDevice>, static_cast<void *>(myAsync));
    ogvrDeviceAsyncStartWaitLoop(ctx, &DummyAsyncDevice::wait,
                                 static_cast<void *>(myAsync));
    return OGVR_PLUGIN_SUCCESS;
}