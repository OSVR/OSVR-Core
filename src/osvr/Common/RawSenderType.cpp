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
#include <osvr/Common/RawSenderType.h>

// Library/third-party includes
#include <vrpn_BaseClass.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    RawSenderType::RawSenderType() {}
    RawSenderType::RawSenderType(UnderlyingSenderType sender)
        : m_sender(sender) {}
    RawSenderType::UnderlyingSenderType RawSenderType::get() const {
        return m_sender.get_value_or(vrpn_ANY_SENDER);
    }
    RawSenderType::UnderlyingSenderType
    RawSenderType::getOr(UnderlyingSenderType valueIfNotSet) const {
        return m_sender.get_value_or(valueIfNotSet);
    }
} // namespace common
} // namespace osvr
