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

#ifndef INCLUDED_GuardInterfaceDummy_h_GUID_3882E1F8_E44A_4D95_E4D3_6C60A9C94F05
#define INCLUDED_GuardInterfaceDummy_h_GUID_3882E1F8_E44A_4D95_E4D3_6C60A9C94F05

// Internal Includes
#include <osvr/Util/GuardInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    class DummyGuard : public GuardInterface {
      public:
        virtual bool lock() { return true; }
        virtual ~DummyGuard() {}
    };

} // namespace util
} // namespace osvr

#endif // INCLUDED_GuardInterfaceDummy_h_GUID_3882E1F8_E44A_4D95_E4D3_6C60A9C94F05
