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
#include <ogvr/PluginKit/PluginRegistrationC.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

// Forward declarations of things not yet implemented
typedef void *OGVRDevice;

static int sampleHardwareUserdata = 1;

static OGVRPluginReturnCode
pollForHardwareChange(OGVRPluginHardwarePollContext /*pollContext*/,
                      void *userData) {
    int &data = *static_cast<int *>(userData);
    std::cout << "Got a poll for hardware change, with user data " << data
              << std::endl;
    return OGVR_PLUGIN_SUCCESS;
}

namespace {
/// Generic deleter function - the kind of thing easily hidden in a header-only
/// C++ wrapper.
template <typename T> void generic_deleter(void *obj) {
    T *o = static_cast<T *>(obj);
    delete o;
}
} // end of anonymous namespace
class DummyDevice {
  public:
    DummyDevice() { std::cout << "Constructing dummy device" << std::endl; }

    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
};

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
        ogvrDeviceSendData(m_dev, mydata);
    }
    OGVRDevice m_dev;
};

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
        ogvrDeviceSendData(m_dev, mydata);
    }
    OGVRDevice m_dev;
};

OGVR_PLUGIN(org_opengoggles_example_DummyDevice) {

    /// Register a polling callback, with some dummy userdata.
    ogvrPluginRegisterHardwarePollCallback(ctx, &pollForHardwareChange,
                                           &sampleHardwareUserdata);

    /// Create a "device" that actually does nothing.
    DummyDevice *myDevice = new DummyDevice();
    /// Must ask the core to tell us to delete it.
    ogvrPluginRegisterDataWithDeleteCallback(ctx, &generic_deleter<DummyDevice>,
                                             static_cast<void *>(myDevice));

    {
        /// Create a synchronous (in the mainloop) device
        OGVRDevice d;
        ogvrDeviceSyncInit(ctx, "My Sync Device",
                           d); // Puts an object in d that knows it's a sync
                               // device so ogvrDeviceSendData knows that it
                               // doesn't need to acquire a lock.
        DummySyncDevice *mySync = new DummySyncDevice(d);
        ogvrPluginRegisterDataWithDeleteCallback(
            ctx, &generic_deleter<DummySyncDevice>,
            static_cast<void *>(mySync));
        ogvrDeviceSyncRegisterUpdateCallback(ctx, &DummySyncDevice::update,
                                             static_cast<void *>(mySync));
    }

    {
        /// Create an asynchronous (threaded) device
        OGVRDevice d;
        ogvrDeviceAsyncInit(ctx, "My Async Device",
                            d); // Puts an object in d that knows it's a
                                // threaded device so ogvrDeviceSendData knows
                                // that it needs to get a connection lock first.
        DummyAsyncDevice *myAsync = new DummyAsyncDevice(d);
        ogvrPluginRegisterDataWithDeleteCallback(
            ctx, &generic_deleter<DummyAsyncDevice>,
            static_cast<void *>(myAsync));
        ogvrDeviceAsyncStartWaitLoop(ctx, &DummyAsyncDevice::wait,
                                     static_cast<void *>(myAsync));
    }

    return OGVR_PLUGIN_SUCCESS;
}
