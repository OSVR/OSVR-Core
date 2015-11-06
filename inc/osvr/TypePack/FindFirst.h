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

#ifndef INCLUDED_FindFirst_h_GUID_FA5855C9_2243_4DA5_B9B7_ED766E8348FE
#define INCLUDED_FindFirst_h_GUID_FA5855C9_2243_4DA5_B9B7_ED766E8348FE

// Internal Includes
#include "T.h"
#include "SizeT.h"
#include "List.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {
    namespace detail {
        template <typename Needle, std::size_t i, typename... Ts>
        struct find_first_impl;
        // Expand lists
        template <typename Needle, typename... Ts>
        struct find_first_impl<Needle, 0, list<Ts...>>
            : find_first_impl<Needle, 0, Ts...> {};
        // base case: at the head
        template <typename Needle, std::size_t i, typename... Ts>
        struct find_first_impl<Needle, i, Needle, Ts...> {
            using type = size_t_<i>;
        };
        // Recursive case
        template <typename Needle, std::size_t i, typename Head, typename... Ts>
        struct find_first_impl<Needle, i, Head, Ts...> {
            using type = t_<find_first_impl<Needle, i + 1, Ts...>>;
        };
        /// base case not found
        template <typename Needle, std::size_t i>
        struct find_first_impl<Needle, i> {};

    } // namespace detail

    /// @brief Returns the zero-based index of the first instance of @p Needle
    /// in @p List. Will fail to compile if not found.
    template <typename List, typename Needle>
    using find_first = t_<detail::find_first_impl<Needle, 0, List>>;

} // namespace typepack
} // namespace osvr

#endif // INCLUDED_FindFirst_h_GUID_FA5855C9_2243_4DA5_B9B7_ED766E8348FE
