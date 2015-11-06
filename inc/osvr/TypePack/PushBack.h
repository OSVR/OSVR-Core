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

#ifndef INCLUDED_PushBack_h_GUID_86DCDCD9_8EF3_4E95_FB45_B2209B97104A
#define INCLUDED_PushBack_h_GUID_86DCDCD9_8EF3_4E95_FB45_B2209B97104A

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
        template <typename T> struct push_back_impl {
            template <typename... Ts> using apply = list<Ts..., T>;
        };
    } // namespace detail
    template <typename List, typename T>
    using push_back = apply_list<detail::push_back_impl<T>, List>;
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_PushBack_h_GUID_86DCDCD9_8EF3_4E95_FB45_B2209B97104A
