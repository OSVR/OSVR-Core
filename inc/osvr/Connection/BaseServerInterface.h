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
