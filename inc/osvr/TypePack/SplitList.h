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

#ifndef INCLUDED_SplitList_h_GUID_471C8D69_CFAD_4083_5485_5DE0EAA24C7F
#define INCLUDED_SplitList_h_GUID_471C8D69_CFAD_4083_5485_5DE0EAA24C7F

// Internal Includes
#include "List.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {
    namespace detail {
        /// General/dummy case.
        template <typename... List> struct split_list_ {
            using head = void;
            using tail = list<>;
        };

        /// Unwrap type pack.
        template <typename... List>
        struct split_list_<list<List...>> : split_list_<List...> {};

        /// Primary case
        template <typename Head, typename... Tail>
        struct split_list_<Head, Tail...> {
            using head = Head;
            using tail = list<Tail...>;
        };
    } // namespace detail

    /// Get the first element of a list.
    template <typename... List>
    using head = typename detail::split_list_<List...>::head;

    /// Get the list without its first element
    template <typename... List>
    using tail = typename detail::split_list_<List...>::tail;
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_SplitList_h_GUID_471C8D69_CFAD_4083_5485_5DE0EAA24C7F
