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
#include "GetVRPNConnection.h"
#include <ogvr/PluginKit/PluginKit.h>
#include <ogvr/PluginHost/RegistrationContext.h>
#include <ogvr/PluginHost/PluginSpecificRegistrationContext.h>
#include <ogvr/Connection/Connection.h>
#include <ogvr/Connection/ConnectionDevice.h>
#include <ogvr/Util/UniquePtr.h>

// Library/third-party includes
#include "hidapi/hidapi.h"
#include "vrpn_Connection.h"
#include "vrpn_Tracker_RazerHydra.h"

// Standard includes
#include <iostream>
#include <map>
#include <string>
#include <sstream>

template <typename T>
class VRPNCustomConnectionDevice : public ogvr::connection::ConnectionDevice {
  public:
    VRPNCustomConnectionDevice(std::string const &name, vrpn_Connection *conn)
        : ogvr::connection::ConnectionDevice(name),
          m_dev(new T(name.c_str(), conn)) {}
    virtual ~VRPNCustomConnectionDevice() {}
    virtual void m_process() { m_dev->mainloop(); }
    virtual void m_sendData(ogvr::util::time::TimeValue const &,
                            ogvr::connection::MessageType *, const char *,
                            size_t) {
        /// Will never be called.
    }

  private:
    ogvr::unique_ptr<T> m_dev;
};

class VRPNHardwarePoll {
  public:
    typedef std::map<std::string, size_t> NameCountMap;
    OGVR_ReturnCode operator()(OGVR_PluginRegContext ctx) {
        bool found = false;
        for (struct hid_device_info *dev = hid_enumerate(0, 0); dev != NULL;
             dev = dev->next) {
            if (found) {
                continue;
            }
            if (dev->vendor_id == 0x1532 && dev->product_id == 0x0300) {
                // Razer Hydra
                found = true;
                constructAndRegister<vrpn_Tracker_RazerHydra>(ctx,
                                                              "RazerHydra");
            }
        }
        return OGVR_RETURN_SUCCESS;
    }
    template <typename T>
    void constructAndRegister(OGVR_PluginRegContext ctx,
                              std::string const &nameStem) {
        ogvr::pluginhost::PluginSpecificRegistrationContext &pluginCtx =
            ogvr::pluginhost::PluginSpecificRegistrationContext::get(ctx);
        std::string name = pluginCtx.getName() + "/" + assignName(nameStem);
        std::cout << "Creating " << name << std::endl;
        ogvr::connection::ConnectionPtr conn =
            ogvr::connection::Connection::retrieveConnection(
                pluginCtx.getParent());
        ogvr::connection::ConnectionDevicePtr dev(
            new VRPNCustomConnectionDevice<T>(name, getVRPNConnection(ctx)));

        conn->addDevice(dev);
    }
    std::string assignName(std::string const &nameStem) {
        size_t num = assignNumber(nameStem);
        std::ostringstream os;
        os << nameStem << num;
        return os.str();
    }
    size_t assignNumber(std::string const &nameStem) {
        NameCountMap::iterator it = m_nameCount.find(nameStem);
        if (it != m_nameCount.end()) {
            it->second++;
            return it->second;
        }
        m_nameCount[nameStem] = 0;
        return 0;
    }

  private:
    NameCountMap m_nameCount;
};

OGVR_PLUGIN(org_opengoggles_bundled_Multiserver) {
    ogvr::pluginkit::PluginContext context(ctx);

    ogvr::unique_ptr<VRPNHardwarePoll> poll(new VRPNHardwarePoll);

    context.registerHardwarePollCallback(poll.release());
    return OGVR_RETURN_SUCCESS;
}
