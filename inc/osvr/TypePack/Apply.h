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

#ifndef INCLUDED_Apply_h_GUID_BA8D840B_7B79_46A6_2018_3CC5CB4AFCC2
#define INCLUDED_Apply_h_GUID_BA8D840B_7B79_46A6_2018_3CC5CB4AFCC2

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {
    /// Apply an alias class
    template <typename F, typename... Args>
    using apply = typename F::template apply<Args...>;
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_Apply_h_GUID_BA8D840B_7B79_46A6_2018_3CC5CB4AFCC2
