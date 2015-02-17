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
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    typedef boost::optional<int32_t> MessageKeyType;
    class MessageRegistration {
      public:
        virtual MessageKeyType registerMessage(const char *messageName) = 0;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_MessageRegistration_h_GUID_F431F1DB_4193_42AB_3376_C67740E2C6FE
