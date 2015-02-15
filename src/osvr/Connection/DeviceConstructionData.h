/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_DeviceConstructionData_h_GUID_D54DE33A_8EF1_41AB_4537_4CE726C9EF0E
#define INCLUDED_DeviceConstructionData_h_GUID_D54DE33A_8EF1_41AB_4537_4CE726C9EF0E

// Internal Includes
#include <osvr/Connection/DeviceInitObject.h>

// Library/third-party includes
#include <vrpn_Connection.h>
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {
    class vrpn_BaseFlexServer;
    class DeviceConstructionData : boost::noncopyable {
      public:
        DeviceConstructionData(DeviceInitObject &initObject,
                               vrpn_Connection *connection)
            : obj(initObject), conn(connection), flexServer(nullptr) {}
        std::string getQualifiedName() const { return obj.getQualifiedName(); }
        DeviceInitObject &obj;
        vrpn_Connection *conn;
        vrpn_BaseFlexServer *flexServer;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_DeviceConstructionData_h_GUID_D54DE33A_8EF1_41AB_4537_4CE726C9EF0E
