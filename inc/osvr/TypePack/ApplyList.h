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

#ifndef INCLUDED_ApplyList_h_GUID_9B48A805_89F9_48A8_3AC8_1C477B7328C7
#define INCLUDED_ApplyList_h_GUID_9B48A805_89F9_48A8_3AC8_1C477B7328C7

// Internal Includes
#include "List.h"
#include "T.h"
#include "Apply.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {
    namespace detail {
        /// General/dummy case.
        template <typename F, typename List> struct apply_list_;
        template <typename F, typename... Args>
        struct apply_list_<F, list<Args...>> {
            // the simpler solution doesn't build with MSVC 2013
            template <typename...> using apply = typepack::apply<F, Args...>;
        };
    } // namespace detail

    /// Apply an alias class, exploding the list of args
    template <typename F, typename Args>
    using apply_list = apply<detail::apply_list_<F, Args>>;
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_ApplyList_h_GUID_9B48A805_89F9_48A8_3AC8_1C477B7328C7
