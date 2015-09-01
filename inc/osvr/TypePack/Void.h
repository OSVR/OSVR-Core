/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

    @author
    Eric Niebler
*/

// Copyright 2015 Sensics, Inc.
// TypePack is part of OSVR-Core.
//
// Incorporates code from "meta":
// Copyright Eric Niebler 2014-2015
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef INCLUDED_Void_h_GUID_C0BDC2E5_4EB7_4BA1_93F7_841769EB1655
#define INCLUDED_Void_h_GUID_C0BDC2E5_4EB7_4BA1_93F7_841769EB1655

// Internal Includes
#include "Apply.h"
#include "Always.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    /// An alias for `void`.
    template <typename... Ts> using void_ = apply<always<void>, Ts...>;

} // namespace typepack
} // namespace osvr

#endif // INCLUDED_Void_h_GUID_C0BDC2E5_4EB7_4BA1_93F7_841769EB1655
