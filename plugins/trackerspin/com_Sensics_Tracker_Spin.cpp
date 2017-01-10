/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/ClientKit/InterfaceStateC.h>
#include <quat/quat.h>

// Generated JSON header file
#include "com_Sensics_Tracker_Spin_json.h"

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>

// Standard includes
#include <iostream>
#include <memory>
#include <vector>

// Anonymous namespace to avoid symbol collision
namespace {

static const auto DRIVER_NAME = "TrackerSpin";
static double inline getDefaultReportRate() { return 200.0; }
static double inline getDefaultXSpin() { return 0.0; }
static double inline getDefaultYSpin() { return 1.0; }
static double inline getDefaultZSpin() { return 0.0; }
static double inline getDefaultRotationRate() { return 0.1; }

class TrackerSpinDevice {
  public:
    TrackerSpinDevice(OSVR_PluginRegContext ctx, std::string const &name,
                      double &axisX, double &axisY, double &axisZ,
                      double &updateRate, double &rotationRate)
        : x(axisX), y(axisY), z(axisZ), reportRate(updateRate),
          spinRate(rotationRate) {

        if (spinRate < 0.) {
            x *= -1.;
            y *= -1.;
            z *= -1.;
        }

        double dt;
        if (spinRate == 0.) {
            dt = 1.0;
        } else {
            dt = 0.9 * (0.5 / spinRate);
        }
        q_from_axis_angle(vel_quat, x, y, z, dt * spinRate * Q_PI);
        vel_quat_dt = dt;
        osvrTimeValueGetNow(&timestamp);
        osvrTimeValueGetNow(&start);

        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Create the device token with the options
        m_dev.initAsync(ctx, name, opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_Sensics_Tracker_Spin_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {

        osvrTimeValueGetNow(&currentTime);
        /// time to report
        if (osvrTimeValueDurationSeconds(&currentTime, &timestamp) >=
            1.0 / reportRate) {
            timestamp.seconds = currentTime.seconds;
            timestamp.microseconds = currentTime.microseconds;
            double duration =
                osvrTimeValueDurationSeconds(&currentTime, &start);
            q_from_axis_angle(d_quat, x, y, z, duration * spinRate * 2 * Q_PI);
            OSVR_OrientationState state;
            osvrQuatSetW(&state, d_quat[3]);
            osvrQuatSetX(&state, d_quat[0]);
            osvrQuatSetY(&state, d_quat[1]);
            osvrQuatSetZ(&state, d_quat[2]);
            osvrDeviceTrackerSendOrientationTimestamped(m_dev, m_tracker,
                                                        &state, 0, &timestamp);
        }

        OSVR_OrientationState orientState;

        return OSVR_RETURN_SUCCESS;
    }

  private:
    OSVR_TrackerDeviceInterface m_tracker;
    osvr::pluginkit::DeviceToken m_dev;
    double x, y, z, spinRate, reportRate, vel_quat_dt;
    q_type vel_quat, d_quat;
    OSVR_TimeValue timestamp, currentTime, start;
};

class TrackerSpinCreate {
  public:
    TrackerSpinCreate() {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        Json::Value root;
        {
            Json::Reader reader;
            if (!reader.parse(params, root)) {
                std::cerr << "Couldn't parse JSON for tracker blendor!"
                          << std::endl;
                return OSVR_RETURN_FAILURE;
            }
        }

        auto updateRate =
            root.get("report_rate", getDefaultReportRate()).asDouble();
        auto xAxisSpin =
            root.get("x_of_axis_to_spin_around", getDefaultXSpin()).asDouble();
        auto yAxisSpin =
            root.get("y_of_axis_to_spin_around", getDefaultYSpin()).asDouble();
        auto zAxisSpin =
            root.get("z_of_axis_to_spin_around", getDefaultZSpin()).asDouble();
        auto spinRate = root.get("rotation_rate_around_axis_in_Hz",
                                 getDefaultRotationRate())
                            .asDouble();
        // optional
        auto deviceName = root.get("name", DRIVER_NAME).asString();

        osvr::pluginkit::PluginContext context(ctx);

        /// @todo make the token own this instead once there is API for that.
        context.registerObjectForDeletion(
            new TrackerSpinDevice(ctx, deviceName, xAxisSpin, yAxisSpin,
                                  zAxisSpin, updateRate, spinRate));
        return OSVR_RETURN_SUCCESS;
    }
};
} // namespace

OSVR_PLUGIN(com_Sensics_Tracker_Spin) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerDriverInstantiationCallback(DRIVER_NAME,
                                                TrackerSpinCreate());

    return OSVR_RETURN_SUCCESS;
}
