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

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Traits class for use with MessageHandler.
    struct BaseDeviceMessage {
        typedef vrpn_MESSAGEHANDLER handler_type;
        class registration_type {
          public:
            registration_type(BaseDevice *dev) : m_dev(dev) {}

            void registerHandler(handler_type handler, void *userdata,
                                 RawSenderType const &,
                                 RawMessageType const &msgType) {
                m_dev->registerHandler(handler, userdata, msgType);
            }
            void unregisterHandler(handler_type handler, void *userdata,
                                   RawSenderType const &,
                                   RawMessageType const &msgType) {
                m_dev->unregisterHandler(handler, userdata, msgType);
            }

          private:
            BaseDevice *m_dev;
        };
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_BaseMessageTraits_h_GUID_AB3AFAC5_54F4_41BF_78D9_CE10525DD053
