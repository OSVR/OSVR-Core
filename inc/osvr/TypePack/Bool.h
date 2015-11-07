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
// Part of OSVR-Core.
//
// This file incorporates code from "meta":
// Copyright Eric Niebler 2014-2015
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef INCLUDED_Bool_h_GUID_BD15BC1D_7D84_4CB7_1866_6FB218461737
#define INCLUDED_Bool_h_GUID_BD15BC1D_7D84_4CB7_1866_6FB218461737

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace typepack {
    /// @brief Alias template to simplify creating a boolean integral constant
    template <bool V> using bool_ = std::integral_constant<bool, V>;
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_Bool_h_GUID_BD15BC1D_7D84_4CB7_1866_6FB218461737
