/** @file
    @brief Implementation of a dummy Hardware Detect Callback, using the full
    idiomatic C++ wrapper, beyond even DummyHardwareDetectCpp2

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
class HardwareDetection {
  public:
    HardwareDetection(int initialVal) : m_data(initialVal) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext /*ctx*/) {

        std::cout
            << "Got a detection request for hardware change, with dummy data "
            << m_data << std::endl;
        m_data++;
        std::cout << "Data now " << m_data << std::endl;
        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Some dummy example data.
    int m_data;
};
} // namespace

OSVR_PLUGIN(com_osvr_example_DummyHardwareDetectCpp2) {
    osvr::pluginkit::PluginContext context(ctx);
    /// Register a detection callback function object: here passing it result of
    /// "new".
    /// Auto-registers for destruction.
    context.registerHardwareDetectCallback(new HardwareDetection(0));

    /// Register a detection callback function object: here passing it a value.
    /// It will make a copy that it auto-registers for destruction.
    HardwareDetection myDetect(10);
    context.registerHardwareDetectCallback(myDetect);
    /// Registering twice to demonstrate that copying occurs.
    context.registerHardwareDetectCallback(myDetect);

    return OSVR_RETURN_SUCCESS;
}
