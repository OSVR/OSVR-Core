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

#ifndef INCLUDED_MessageRegistration_h_GUID_F431F1DB_4193_42AB_3376_C67740E2C6FE
#define INCLUDED_MessageRegistration_h_GUID_F431F1DB_4193_42AB_3376_C67740E2C6FE

// Internal Includes
#include <osvr/Common/RawMessageType.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    /// @brief Class template wrapping message-specific data and/or logic.
    ///
    /// @tparam T Your message-specific type: must have a `static const char *
    /// identifier()`
    /// method returning the string ID of the message.
    template <typename T> class MessageRegistration : public T {
      public:
        RawMessageType type;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_MessageRegistration_h_GUID_F431F1DB_4193_42AB_3376_C67740E2C6FE
