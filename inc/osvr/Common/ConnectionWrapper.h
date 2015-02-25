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

#ifndef INCLUDED_ConnectionWrapper_h_GUID_84960342_5DC0_40B2_E9D0_1E479E85D15B
#define INCLUDED_ConnectionWrapper_h_GUID_84960342_5DC0_40B2_E9D0_1E479E85D15B

// Internal Includes
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    class ConnectionWrapper {
      public:
        ConnectionWrapper(vrpn_ConnectionPtr conn);

      private:
        vrpn_ConnectionPtr m_conn;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_ConnectionWrapper_h_GUID_84960342_5DC0_40B2_E9D0_1E479E85D15B
