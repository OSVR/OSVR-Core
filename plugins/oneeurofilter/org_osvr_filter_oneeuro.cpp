/** @file
    @brief "One-Euro" tracking filter analysis plugin

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "org_osvr_filter_oneeuro_json.h"

// Library/third-party includes
#include "vrpn_OneEuroFilter.h" // A one-euro filter implementation that is bundled with VRPN, but independent of it.

// Standard includes
#include <iostream>

// Anonymous namespace to avoid symbol collision
namespace {
	/// Parameters needed for both vector and 
	struct Params {
		double minCutoff;
		double beta;
		double derivativeCutoff;
	};

class OneEuroFilterDevice {
  public:
	  OneEuroFilterDevice(OSVR_PluginRegContext ctx) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

		osvrDeviceTrackerConfigure(opts, &m_trackerOut);
        /// Create the device token with the options
        m_dev.initSync(ctx, "MyExampleDevice", opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(org_osvr_filter_oneeuro_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        /// This dummy loop just wastes time, to pretend to be your plugin
        /// blocking to wait for the arrival of data. It should be completely
        /// removed from your plugin.
        volatile int j; // volatile to keep it from being optimized out.
        for (int i = 0; i < 10000; ++i) {
            j = i;
        }
        /// End time-waster loop.

        /// Make up some dummy data that changes to report.
        m_myVal = (m_myVal + 0.1);
        if (m_myVal > 10.0) {
            m_myVal = 0;
        }

        /// Report the value of channel 0
        osvrDeviceAnalogSetValue(m_dev, m_analog, m_myVal, 0);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
	OSVR_ClientContext m_clientCtx;
    OSVR_TrackerDeviceInterface m_trackerOut;
    double m_myVal;
};

class AnalysisPluginInstantiation {
  public:
	AnalysisPluginInstantiation() {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char * params) {

        std::cout << "PLUGIN: Got a hardware detection request" << std::endl;
        if (!m_found) {
            std::cout << "PLUGIN: We have detected our fake device! Doing "
                         "setup stuff!"
                      << std::endl;
            m_found = true;

            /// Create our device object
            osvr::pluginkit::registerObjectForDeletion(
                ctx, new AnalogExampleDevice(ctx));
        }
        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance)
    bool m_found;
};
} // namespace

OSVR_PLUGIN(org_osvr_filter_oneeuro) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerDriverInstantiationCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}
