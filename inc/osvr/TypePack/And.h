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

#ifndef INCLUDED_And_h_GUID_9FF8A8BB_A3FE_4EBC_E400_07158E7E3B6D
#define INCLUDED_And_h_GUID_9FF8A8BB_A3FE_4EBC_E400_07158E7E3B6D

// Internal Includes
#include "Bool.h"
#include "If.h"
#include "T.h"

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace typepack {

    /// \cond
    namespace detail {
        template <typename... Bools> struct and_impl;

        template <> struct and_impl<> : std::true_type {};

        template <typename Bool, typename... Bools>
        struct and_impl<Bool, Bools...>
            : if_<bool_<!Bool::type::value>, std::false_type,
                  and_impl<Bools...>> {};

    } // namespace detail
    /// \endcond

    /// Logically and together all the integral constant-wrapped Boolean
    /// parameters, \e with short-circuiting.
    template <typename... Bools> using and_ = t_<detail::and_impl<Bools...>>;

} // namespace typepack
} // namespace osvr
#endif // INCLUDED_And_h_GUID_9FF8A8BB_A3FE_4EBC_E400_07158E7E3B6D
