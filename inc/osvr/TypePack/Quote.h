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

#ifndef INCLUDED_Quote_h_GUID_0665E3E5_DDFC_489F_AC33_6AF40B7454A6
#define INCLUDED_Quote_h_GUID_0665E3E5_DDFC_489F_AC33_6AF40B7454A6

// Internal Includes
#include "T.h"
#include "Defer.h"

// Library/third-party includes
// - none

// Standard includes
// - none
namespace osvr {
namespace typepack {

    /// Turn a class template or alias template \p C into a Alias Class.
    template <template <typename...> class C> struct quote {
        // Indirection through defer here needed to avoid Core issue 1430
        // http://open-std.org/jtc1/sc22/wg21/docs/cwg_active.html#1430
        template <typename... Ts> using apply = t_<defer<C, Ts...>>;
    };

} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Quote_h_GUID_0665E3E5_DDFC_489F_AC33_6AF40B7454A6
