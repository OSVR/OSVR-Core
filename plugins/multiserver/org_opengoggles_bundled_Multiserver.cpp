/** @file
    @brief Implementation of the "multiserver" plugin that offers the stock VRPN
   devices.

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include "VRPNMultiserver.h"
#include "DevicesWithParameters.h"
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include "hidapi/hidapi.h"
#include "vrpn_Connection.h"
#include "vrpn_Tracker_RazerHydra.h"
#include "vrpn_Tracker_Filter.h"
#include <boost/noncopyable.hpp>

// Standard includes
#include <iostream>
#include <map>
#include <string>
#include <sstream>

class VRPNHardwareDetect : boost::noncopyable {
  public:
    VRPNHardwareDetect(VRPNMultiserverData &data) : m_data(data) {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        BoundServer server(m_data, ctx);
        struct hid_device_info *enumData = hid_enumerate(0, 0);
        for (struct hid_device_info *dev = enumData; dev != NULL;
             dev = dev->next) {
            if (dev->vendor_id == 0x1532 && dev->product_id == 0x0300) {
                // Razer Hydra
                DeviceFullName name =
                    server.constructAndRegister<vrpn_Tracker_RazerHydra>(
                        "RazerHydra");
                std::string localName = "*" + name.get();

                // Corresponding filter
                DeviceFullName filterName = server.getName("OneEuroFilter");
                osvr::unique_ptr<vrpn_Tracker_FilterOneEuro> filter(
                    new vrpn_Tracker_FilterOneEuro(
                        filterName.get().c_str(), server.getVRPNConnection(),
                        localName.c_str(), 2, 1.15, 1.0, 1.2, 1.5, 5.0, 1.2));
                server.registerDevice<vrpn_Tracker_FilterOneEuro>(
                    filterName, filter.release());
                break;
            }
        }
        hid_free_enumeration(enumData);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    VRPNMultiserverData &m_data;
};

OSVR_PLUGIN(org_opengoggles_bundled_Multiserver) {
    osvr::pluginkit::PluginContext context(ctx);

    VRPNMultiserverData &data =
        *context.registerObjectForDeletion(new VRPNMultiserverData);
    context.registerHardwareDetectCallback(new VRPNHardwareDetect(data));

    osvrRegisterDriverInstantiationCallback(
        ctx, "YEI_3Space_Sensor", &wrappedConstructor<&createYEI>, &data);

    return OSVR_RETURN_SUCCESS;
}
