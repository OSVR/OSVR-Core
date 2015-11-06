/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
// TypePack is part of OSVR-Core.
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_PushFront_h_GUID_71A57FB9_64D6_4AA1_E968_0F91E951C7B4
#define INCLUDED_PushFront_h_GUID_71A57FB9_64D6_4AA1_E968_0F91E951C7B4

// Internal Includes
#include "List.h"
#include "ApplyList.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {
    namespace detail {
        template <typename T> struct push_front_impl {
            template <typename... Ts> using apply = list<T, Ts...>;
        };
    }

    template <typename List, typename T>
    using push_front = apply_list<detail::push_front_impl<T>, List>;
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_PushFront_h_GUID_71A57FB9_64D6_4AA1_E968_0F91E951C7B4
