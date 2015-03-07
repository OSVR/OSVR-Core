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
#include <osvr/PluginKit/PluginKit.h>

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {

OSVR_MessageType dummyMessage;

class DummyDevice {
  public:
    DummyDevice(OSVR_PluginRegContext ctx) {
        std::cout << "Constructing dummy device" << std::endl;

        /// Create an asynchronous (threaded) device
        osvrDeviceAsyncInit(
            ctx, "MyAsyncDevice",
            &m_dev); // Puts an object in m_dev that knows it's a
        // threaded device so osvrDeviceSendData knows
        // that it needs to get a connection lock first.

        /// Sets the update callback
        osvrDeviceRegisterUpdateCallback(m_dev, &DummyDevice::update, this);
    }

    /// Trampoline: C-compatible callback bouncing into a member function.
    /// Future enhancements to the C++ wrappers will make this tiny function
    /// no longer necessary
    ///
    /// In this case, the core spawns a thread, with a loop calling this
    /// function as long as things are running. So this function waits for the
    /// next message from the device and passes it on.
    static OSVR_ReturnCode update(void *userData) {
        return static_cast<DummyDevice *>(userData)->m_update();
    }

    ~DummyDevice() { std::cout << "Destroying dummy device" << std::endl; }

  private:
    OSVR_ReturnCode m_update() {
        std::cout << "In DummyDevice::m_update" << std::endl;
        // block on waiting for data.
        // once we have enough, send it.
        const char mydata[] = "something";
        osvrDeviceSendData(m_dev, dummyMessage, mydata, sizeof(mydata));
        return OSVR_RETURN_SUCCESS;
    }
    OSVR_DeviceToken m_dev;
};

} // namespace

OSVR_PLUGIN(org_opengoggles_example_DummyAsync) {
    /// Register custom message type
    osvrDeviceRegisterMessageType(ctx, "DummyMessage", &dummyMessage);

    /// Create the device and register it for deletion.
    osvr::pluginkit::registerObjectForDeletion(ctx, new DummyDevice(ctx));

    return OSVR_RETURN_SUCCESS;
}
