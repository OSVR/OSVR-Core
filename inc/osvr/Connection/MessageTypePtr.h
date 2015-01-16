/** @file
    @brief Header forward declaring MessageType and specifying a smart pointer.

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_MessageTypePtr_h_GUID_95F4FC92_63B2_4D8F_A13D_290855521ED8
#define INCLUDED_MessageTypePtr_h_GUID_95F4FC92_63B2_4D8F_A13D_290855521ED8

// Internal Includes
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none
namespace osvr {
namespace connection {
    class MessageType;
    /// @brief a uniquely-owned handle for holding a message type registration.
    typedef unique_ptr<MessageType> MessageTypePtr;
} // namespace connection
} // namespace osvr
#endif // INCLUDED_MessageTypePtr_h_GUID_95F4FC92_63B2_4D8F_A13D_290855521ED8
