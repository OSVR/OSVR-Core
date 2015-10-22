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
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/Util/EigenFilters.h>
#include <osvr/Util/EigenInterop.h>

// Generated JSON header file
#include "org_osvr_filter_oneeuro_json.h"

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>

// Standard includes
#include <iostream>
#include <memory>
#include <vector>

// Anonymous namespace to avoid symbol collision
namespace {

static const auto DRIVER_NAME = "OneEuroFilter";

using osvr::util::filters::one_euro::Params;
namespace filters = osvr::util::filters;

/// Default parameters for a position one-euro filter in meters.
inline Params getPositionDefaults() { return Params{1.15, 0.5, 1.2}; }
/// Default parameters for an orientation one-euro filter using quaternions.
inline Params getOrientationDefaults() { return Params{1.5, 0.5, 1.2}; }

class OneEuroFilterDevice {
  public:
    OneEuroFilterDevice(OSVR_PluginRegContext ctx, std::string const &name,
                        std::string const &input, Params const &posParams,
                        Params const &oriParams)
        : m_posParams(posParams), m_oriParams(oriParams) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceTrackerConfigure(opts, &m_trackerOut);

        /// Create the device token with the options
        OSVR_DeviceToken dev;
        if (OSVR_RETURN_FAILURE ==
            osvrAnalysisSyncInit(ctx, name.c_str(), opts, &dev, &m_clientCtx)) {
            throw std::runtime_error("Could not initialize analysis plugin!");
        }
        m_dev = osvr::pluginkit::DeviceToken(dev);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(org_osvr_filter_oneeuro_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);

        /// Create our client interface and register a callback.
        if (OSVR_RETURN_FAILURE == osvrClientGetInterface(m_clientCtx,
                                                          input.c_str(),
                                                          &m_clientInterface)) {
            throw std::runtime_error(
                "Could not get client interface for analysis plugin!");
        }
        osvrRegisterPoseCallback(m_clientInterface,
                                 &OneEuroFilterDevice::poseCallback, this);

        std::cout << "OneEuroFilterDevice constructor finished." << std::endl;
    }

    ~OneEuroFilterDevice() {
        /// Free the client interface so we don't end up getting called after
        /// destruction.
        osvrClientFreeInterface(m_clientCtx, m_clientInterface);
    }

    static void poseCallback(void *userdata, const OSVR_TimeValue *timestamp,
                             const OSVR_PoseReport *report) {
        auto &self = *static_cast<OneEuroFilterDevice *>(userdata);
        self.handleData(*timestamp, *report);
    }

    /// Processes a tracker report.
    void handleData(OSVR_TimeValue const &timestamp,
                    OSVR_PoseReport const &report) {
        ensureSensorId(report.sensor);
        auto &sensorData = *m_sensors[report.sensor];
        double dt =
            (timestamp.microseconds - sensorData.lastReport.microseconds) /
                1000000.0 +
            (timestamp.seconds - sensorData.lastReport.seconds);
        if (dt <= 0) {
            dt = 1; // in case of weirdness, avoid divide by zero.
        }
        OSVR_PoseState filteredPose;
        using osvr::util::vecMap;
        using osvr::util::toQuat;
        using osvr::util::fromQuat;

        /// Perform filtration
        vecMap(filteredPose.translation) = sensorData.positionFilter.filter(
            dt, vecMap(report.pose.translation));
        auto filteredQuat = sensorData.orientationFilter.filter(
            dt, fromQuat(report.pose.rotation));
        toQuat(filteredQuat, filteredPose.rotation);

        /// Update last report time.
        sensorData.lastReport = timestamp;

        /// Send report
        osvrDeviceTrackerSendPoseTimestamped(m_dev, m_trackerOut, &filteredPose,
                                             report.sensor, &timestamp);
    }

    OSVR_ReturnCode update() {
        // Nothing to do here - everything happens in a callback.
        return OSVR_RETURN_SUCCESS;
    }

    void ensureSensorId(OSVR_ChannelCount sensor) {
        /// Make sure there's enough entries in the vector.
        if (m_sensors.size() <= sensor) {
            std::cout << "Resizing to handle sensor #" << sensor << "\n";
            m_sensors.resize(sensor + 1);
        }
        /// Make sure the desired entry isn't a null pointer.
        if (!m_sensors[sensor]) {
            std::cout << "Creating sensor data object for sensor #" << sensor
                      << std::endl;
            m_sensors[sensor] = makeSensorData();
        }
    }

  private:
    const Params m_posParams;
    const Params m_oriParams;

    OSVR_TrackerDeviceInterface m_trackerOut;
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_ClientContext m_clientCtx;
    OSVR_ClientInterface m_clientInterface;

    struct SensorData {
        SensorData(Params const &posParams, Params const &oriParams)
            : positionFilter(posParams), orientationFilter(oriParams),
              lastReport(osvr::util::time::getNow()) {}

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        filters::OneEuroFilter<Eigen::Vector3d> positionFilter;
        filters::OneEuroFilter<Eigen::Quaterniond> orientationFilter;
        osvr::util::time::TimeValue lastReport;
    };
    using SensorDataPtr = std::unique_ptr<SensorData>;

    SensorDataPtr makeSensorData() const {
        SensorDataPtr ret(new SensorData(m_posParams, m_oriParams));
        return ret;
    }
    std::vector<SensorDataPtr> m_sensors;
};

class AnalysisPluginInstantiation {
  public:
    AnalysisPluginInstantiation() {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        Json::Value root;
        {
            Json::Reader reader;
            if (!reader.parse(params, root)) {
                std::cerr << "Couldn't parse JSON for one euro filter!"
                          << std::endl;
                return OSVR_RETURN_FAILURE;
            }
        }

        // Get filter parameters for position
        auto posParams = getPositionDefaults();
        if (root.isMember("position")) {
            parseOneEuroParams(posParams, root["position"]);
        }

        // Get filter parameters for orientation
        auto oriParams = getOrientationDefaults();
        if (root.isMember("orientation")) {
            parseOneEuroParams(oriParams, root["orientation"]);
        }

        // required
        auto input = root["input"].asString();

        // optional
        auto deviceName = root.get("name", DRIVER_NAME).asString();

        osvr::pluginkit::PluginContext context(ctx);

        /// @todo make the token own this instead once there is API for that.
        context.registerObjectForDeletion(new OneEuroFilterDevice(
            ctx, deviceName, input, posParams, oriParams));
        return OSVR_RETURN_SUCCESS;
    }

    static void parseOneEuroParams(Params &p, Json::Value const &json) {
        // In all cases, using the existing value as default value.
        p.minCutoff = json.get("minCutoff", p.minCutoff).asDouble();
        p.beta = json.get("beta", p.beta).asDouble();
        p.derivativeCutoff =
            json.get("derivativeCutoff", p.derivativeCutoff).asDouble();
    }
};
} // namespace

OSVR_PLUGIN(org_osvr_filter_oneeuro) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerDriverInstantiationCallback(DRIVER_NAME,
                                                AnalysisPluginInstantiation());

    return OSVR_RETURN_SUCCESS;
}
