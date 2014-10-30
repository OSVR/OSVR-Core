/** @file
    @brief Header

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

#ifndef INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183
#define INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183

// Internal Includes
#include <ogvr/Util/SharedPtr.h>
#include <ogvr/PluginKit/Export.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {

class Connection;
/// @brief How one must hold a Connection.
typedef shared_ptr<Connection> ConnectionPtr;

/// @brief Class wrapping a messaging transport (server or internal) connection.
class Connection {
  public:
    /// @name Factory methods
    ///
    /// More to come.
    /// @{
    /// @brief Factory method to create a local-machine-only connection
    OGVR_PLUGINKIT_EXPORT static ConnectionPtr createLocalConnection();
    /// @}

    OGVR_PLUGINKIT_EXPORT virtual ~Connection();

  private:
    Connection();
};
}
#endif // INCLUDED_Connection_h_GUID_61C65986_E2C9_498F_59F0_8EFC712BA183
