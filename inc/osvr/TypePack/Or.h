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

#ifndef INCLUDED_Or_h_GUID_2D16A5C5_4937_4695_30A8_842C5DD4B2C7
#define INCLUDED_Or_h_GUID_2D16A5C5_4937_4695_30A8_842C5DD4B2C7

// Internal Includes
#include "Bool.h"
#include "If.h"

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace typepack {

    /// \cond
    namespace detail {
        template <typename... Bools> struct or_impl;

        template <> struct or_impl<> : std::false_type {};

        template <typename Bool, typename... Bools>
        struct or_impl<Bool, Bools...>
            : if_c<Bool::type::value, std::true_type, or_impl<Bools...>> {};
    } // namespace detail
    /// \endcond

    /// Logically or together all the integral constant-wrapped Boolean
    /// parameters, \e with short-circuiting.
    template <typename... Bools> using or_ = t_<detail::or_impl<Bools...>>;

} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Or_h_GUID_2D16A5C5_4937_4695_30A8_842C5DD4B2C7
