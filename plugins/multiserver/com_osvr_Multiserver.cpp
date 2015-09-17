/** @file
    @brief Implementation of the "multiserver" plugin that offers the stock VRPN
   devices.

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

#ifdef __ANDROID__
// Define this for verbose output during polling.
#define OSVR_MULTISERVER_VERBOSE
#endif

// Internal Includes
#include "VRPNMultiserver.h"
#include "DevicesWithParameters.h"
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/StringLiteralFileToString.h>
#include <osvr/VRPNServer/VRPNDeviceRegistration.h>

#include "com_osvr_Multiserver_OSVRHackerDevKit_json.h"
#include "com_osvr_Multiserver_OneEuroFilter_json.h"
#include "com_osvr_Multiserver_RazerHydra_json.h"

// Library/third-party includes
#include "hidapi/hidapi.h"
#include "vrpn_Connection.h"
#include "vrpn_Tracker_RazerHydra.h"
#include "vrpn_Tracker_OSVRHackerDevKit.h"
#include "vrpn_Tracker_Filter.h"
#include <boost/noncopyable.hpp>

#include <json/value.h>
#include <json/reader.h>

#ifdef OSVR_MULTISERVER_VERBOSE
#include <boost/format.hpp>
#endif

// Standard includes
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#ifdef OSVR_MULTISERVER_VERBOSE
#include <iostream>
#endif

class VRPNHardwareDetect : boost::noncopyable {
  public:
    VRPNHardwareDetect(VRPNMultiserverData &data) : m_data(data) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        bool gotDevice;
#ifdef OSVR_MULTISERVER_VERBOSE
        bool first = true;
#endif
        do {
            gotDevice = false;
            struct hid_device_info *enumData = hid_enumerate(0, 0);
            for (struct hid_device_info *dev = enumData; dev != nullptr;
                 dev = dev->next) {

                if (m_isPathHandled(dev->path)) {
                    continue;
                }

#ifdef OSVR_MULTISERVER_VERBOSE
                if (first) {
                    std::cout << "[OSVR Multiserver] HID Enumeration: "
                              << boost::format("0x%04x") % dev->vendor_id << ":"
                              << boost::format("0x%04x") % dev->product_id
                              << std::endl;
                }
#endif

                if (gotDevice) {
                    continue;
                }
                // Razer Hydra
                if (dev->vendor_id == 0x1532 && dev->product_id == 0x0300) {
                    gotDevice = true;
                    m_handlePath(dev->path);
                    auto hydraJsonString = osvr::util::makeString(
                        com_osvr_Multiserver_RazerHydra_json);
                    Json::Value hydraJson;
                    Json::Reader reader;
                    if (!reader.parse(hydraJsonString, hydraJson)) {
                        throw std::logic_error("Faulty JSON file for Hydra - "
                                               "should not be possible!");
                    }
                    /// Decorated name for Hydra
                    std::string name;
                    {
                        // Razer Hydra
                        osvr::vrpnserver::VRPNDeviceRegistration reg(ctx);
                        name =
                            reg.useDecoratedName(m_data.getName("RazerHydra"));
                        reg.registerDevice(new vrpn_Tracker_RazerHydra(
                            name.c_str(), reg.getVRPNConnection()));
                        reg.setDeviceDescriptor(hydraJsonString);
                    }
                    std::string localName = "*" + name;

                    {
                        // Copy semantic paths for corresponding filter: just
                        // want left/$target and right/$target
                        Json::Value filterJson;
                        if (!reader.parse(
                                osvr::util::makeString(
                                    com_osvr_Multiserver_OneEuroFilter_json),
                                filterJson)) {
                            throw std::logic_error("Faulty JSON file for One "
                                                   "Euro Filter - should not "
                                                   "be possible!");
                        }
                        auto &filterSem =
                            (filterJson["semantic"] = Json::objectValue);
                        auto &hydraSem = hydraJson["semantic"];
                        for (auto const &element : {"left", "right"}) {
                            filterSem[element] = Json::objectValue;
                            filterSem[element]["$target"] =
                                hydraSem[element]["$target"];
                        }
                        auto &filterAuto = (filterJson["automaticAliases"] =
                                                Json::objectValue);
                        filterAuto["$priority"] =
                            130; // enough to override a normal automatic route.
                        auto &hydraAuto = hydraJson["automaticAliases"];
                        for (auto const &element :
                             {"/me/hands/left", "/me/hands/right"}) {
                            filterAuto[element] = hydraAuto[element];
                        }

                        // Corresponding filter
                        osvr::vrpnserver::VRPNDeviceRegistration reg(ctx);
                        reg.registerDevice(new vrpn_Tracker_FilterOneEuro(
                            reg.useDecoratedName(
                                    m_data.getName("OneEuroFilter")).c_str(),
                            reg.getVRPNConnection(), localName.c_str(), 2, 1.15,
                            1.0, 1.2, 1.5, 5.0, 1.2));

                        reg.setDeviceDescriptor(filterJson.toStyledString());
                    }
                    continue;
                }

                // OSVR Hacker Dev Kit
                if ((dev->vendor_id == 0x1532 && dev->product_id == 0x0b00) ||
                    (dev->vendor_id == 0x03EB && dev->product_id == 0x2421)) {
                    gotDevice = true;
                    m_handlePath(dev->path);
                    osvr::vrpnserver::VRPNDeviceRegistration reg(ctx);
                    auto name = m_data.getName("OSVRHackerDevKit");
                    auto decName = reg.useDecoratedName(name);
                    reg.constructAndRegisterDevice<
                        vrpn_Tracker_OSVRHackerDevKit>(name);
                    reg.setDeviceDescriptor(osvr::util::makeString(
                        com_osvr_Multiserver_OSVRHackerDevKit_json));
                    {
                        osvr::vrpnserver::VRPNDeviceRegistration reg2(ctx);
                        reg2.registerDevice(
                            new vrpn_Tracker_DeadReckoning_Rotation(
                                reg2.useDecoratedName(m_data.getName(
                                    "OSVRHackerDevKitPrediction")),
                                reg2.getVRPNConnection(), "*" + decName, 1,
                                1.0 / 60.0, false));
                        reg2.setDeviceDescriptor(osvr::util::makeString(
                            com_osvr_Multiserver_OSVRHackerDevKit_json));
                    }
                    continue;
                }
            }
            hid_free_enumeration(enumData);

#ifdef OSVR_MULTISERVER_VERBOSE
            first = false;
#endif

        } while (gotDevice);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    bool m_isPathHandled(const char *path) {
        return std::find(begin(m_handledPaths), end(m_handledPaths),
                         std::string(path)) != end(m_handledPaths);
    }
    void m_handlePath(const char *path) {
        m_handledPaths.push_back(std::string(path));
    }
    VRPNMultiserverData &m_data;
    std::vector<std::string> m_handledPaths;
};

OSVR_PLUGIN(com_osvr_Multiserver) {
    osvr::pluginkit::PluginContext context(ctx);

    VRPNMultiserverData &data =
        *context.registerObjectForDeletion(new VRPNMultiserverData);
    context.registerHardwareDetectCallback(new VRPNHardwareDetect(data));

    osvrRegisterDriverInstantiationCallback(
        ctx, "YEI_3Space_Sensor", &wrappedConstructor<&createYEI>, &data);

    return OSVR_RETURN_SUCCESS;
}
