/** @file
    @brief Implementation of the "multiserver" plugin that offers the stock VRPN
   devices.

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>;
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <ogvr/PluginKit/PluginKit.h>
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

template <typename T>
class VRPNCustomConnectionDevice : public ogvr::connection::ConnectionDevice {
  public:
    VRPNCustomConnectionDevice(std::string const &name, vrpn_Connection *conn)
        : ogvr::connection::ConnectionDevice(name),
          m_dev(new T(name.c_str(), conn)) {}
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
        hid_enumerate(0, 0);
        return OGVR_RETURN_SUCCESS;
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
