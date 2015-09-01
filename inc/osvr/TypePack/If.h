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

#ifndef INCLUDED_If_h_GUID_F34A2E21_4328_478B_E11B_2E8FBD606BF8
#define INCLUDED_If_h_GUID_F34A2E21_4328_478B_E11B_2E8FBD606BF8

// Internal Includes
#include "Bool.h"
#include "T.h"

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace typepack {
    /// \cond
    namespace detail {
        template <typename...> struct if_impl {};

        template <typename If>
        struct if_impl<If> : std::enable_if<If::type::value> {};

        template <typename If, typename Then>
        struct if_impl<If, Then> : std::enable_if<If::type::value, Then> {};

        template <typename If, typename Then, typename Else>
        struct if_impl<If, Then, Else>
            : std::conditional<If::type::value, Then, Else> {};
    } // namespace detail
    /// \endcond

    /// Select one type or another depending on a compile-time Boolean integral
    /// constant type.
    template <typename... Args> using if_ = t_<detail::if_impl<Args...>>;

    /// Select one type or another depending on a compile-time Boolean value.
    template <bool If, typename... Args>
    using if_c = t_<detail::if_impl<bool_<If>, Args...>>;
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_If_h_GUID_F34A2E21_4328_478B_E11B_2E8FBD606BF8
