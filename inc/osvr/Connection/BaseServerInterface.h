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

#ifndef INCLUDED_BaseServerInterface_h_GUID_C3BFB8DB_2F11_43B6_791E_22E800890BEB
#define INCLUDED_BaseServerInterface_h_GUID_C3BFB8DB_2F11_43B6_791E_22E800890BEB

// Internal Includes
#include <osvr/Connection/DeviceTokenPtr.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace osvr {
namespace connection {
    /// @brief Base interface class for device interfaces that don't wrap
    /// existing VRPN classes.
    ///
    /// They need to be able to be held and deleted generically, as well as be
    /// able to register their message types.
    class BaseServerInterface : boost::noncopyable {
      public:
        /// @brief Virtual destructor
        virtual ~BaseServerInterface();

        /// @brief Interface for registering message types.
        virtual void registerMessageTypes(DeviceToken &dev) = 0;
    };

    typedef shared_ptr<BaseServerInterface> ServerInterfacePtr;
} // namespace connection
} // namespace osvr

#endif // INCLUDED_BaseServerInterface_h_GUID_C3BFB8DB_2F11_43B6_791E_22E800890BEB
