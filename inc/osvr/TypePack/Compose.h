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

#ifndef INCLUDED_Compose_h_GUID_D6907ED1_0D1C_4FAE_C5FB_3C1845DA7AA4
#define INCLUDED_Compose_h_GUID_D6907ED1_0D1C_4FAE_C5FB_3C1845DA7AA4

// Internal Includes
#include "Apply.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    /// Compose the Alias Classes \p Fs in the parameter pack \p Ts.
    template <typename... Fs> struct compose {};

    template <typename F0> struct compose<F0> {
        template <typename... Ts> using apply = apply<F0, Ts...>;
    };

    template <typename F0, typename... Fs> struct compose<F0, Fs...> {
        template <typename... Ts>
        using apply =
            typepack::apply<F0, typepack::apply<compose<Fs...>, Ts...>>;
    };
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Compose_h_GUID_D6907ED1_0D1C_4FAE_C5FB_3C1845DA7AA4
