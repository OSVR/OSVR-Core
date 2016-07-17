/** @file
    @brief Header

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

#ifndef INCLUDED_DeviceConstructionData_h_GUID_D54DE33A_8EF1_41AB_4537_4CE726C9EF0E
#define INCLUDED_DeviceConstructionData_h_GUID_D54DE33A_8EF1_41AB_4537_4CE726C9EF0E

// Internal Includes
#include <osvr/Connection/DeviceInitObject.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>
#include <vrpn_Connection.h>

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
