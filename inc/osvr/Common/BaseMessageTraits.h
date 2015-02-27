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

#ifndef INCLUDED_BaseMessageTraits_h_GUID_AB3AFAC5_54F4_41BF_78D9_CE10525DD053
#define INCLUDED_BaseMessageTraits_h_GUID_AB3AFAC5_54F4_41BF_78D9_CE10525DD053

// Internal Includes
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/MessageHandler.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Traits class for use with MessageHandler.
    typedef ImpliedSenderMessageHandleTraits<vrpn_MESSAGEHANDLER, BaseDevice>
        BaseDeviceMessageHandleTraits;
} // namespace common
} // namespace osvr
#endif // INCLUDED_BaseMessageTraits_h_GUID_AB3AFAC5_54F4_41BF_78D9_CE10525DD053
