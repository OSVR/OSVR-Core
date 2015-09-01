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

#ifndef INCLUDED_QuoteTrait_h_GUID_4A7EACDE_5947_458E_CE4A_63A22C016531
#define INCLUDED_QuoteTrait_h_GUID_4A7EACDE_5947_458E_CE4A_63A22C016531

// Internal Includes
#include "Compose.h"
#include "Quote.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    /// Turn a trait \p C into a Alias Class.
    template <template <typename...> class C>
    using quote_trait = compose<quote<t_>, quote<C>>;
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_QuoteTrait_h_GUID_4A7EACDE_5947_458E_CE4A_63A22C016531
