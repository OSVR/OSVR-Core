/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/RawMessageType.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    RawMessageType::RawMessageType() {}
    RawMessageType::RawMessageType(UnderlyingMessageType msg)
        : m_message(msg) {}
    RawMessageType::UnderlyingMessageType RawMessageType::get() const {
        return m_message.get_value_or(vrpn_ANY_TYPE);
    }
    RawMessageType::UnderlyingMessageType
    RawMessageType::getOr(UnderlyingMessageType valueIfNotSet) const {
        return m_message.get_value_or(valueIfNotSet);
    }
} // namespace common
} // namespace osvr
