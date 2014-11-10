/** @file
    @brief Header forward declaring MessageType and specifying a smart pointer.

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

#ifndef INCLUDED_MessageTypePtr_h_GUID_95F4FC92_63B2_4D8F_A13D_290855521ED8
#define INCLUDED_MessageTypePtr_h_GUID_95F4FC92_63B2_4D8F_A13D_290855521ED8

// Internal Includes
#include <ogvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none
namespace ogvr {
namespace connection {
class MessageType;
/// @brief a uniquely-owned handle for holding a message type registration.
typedef unique_ptr<MessageType> MessageTypePtr;
} // namespace connection
} // namespace ogvr
#endif // INCLUDED_MessageTypePtr_h_GUID_95F4FC92_63B2_4D8F_A13D_290855521ED8
