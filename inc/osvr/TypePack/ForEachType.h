/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
// TypePack is part of OSVR-Core.
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef INCLUDED_ForEachType_h_GUID_42B22EE3_302E_4ABD_1FC8_9D5E7146ADEF
#define INCLUDED_ForEachType_h_GUID_42B22EE3_302E_4ABD_1FC8_9D5E7146ADEF

// Internal Includes
#include "Size.h"
#include "SplitList.h"

// Library/third-party includes
// - none

// Standard includes
#include <utility>

namespace osvr {
namespace typepack {
    namespace detail {
        /// Forward declaration of the implementation struct - because we can't
        /// generate code for a branch when the tail is empty.
        template <typename F, typename Head, typename Tail, bool EmptyTail>
        struct for_each_type_impl;

        /// Implementation function that handles the recursion/invocation of the
        /// right implementation struct.
        template <typename F, typename List, typename... Args>
        inline void for_each_type_(F &&f, Args &&... args) {
            using H = typepack::head<List>;
            using T = typepack::tail<List>;
            using Sz = typepack::size<T>;
            static const auto empty = (Sz::value == 0);
            using Impl = for_each_type_impl<F, H, T, empty>;
            Impl::apply(std::forward<F>(f), std::forward<Args>(args)...);
        }

        /// Specialization of struct for when the tail is empty: just calls the
        /// function
        template <typename F, typename Head, typename Tail>
        struct for_each_type_impl<F, Head, Tail, true> {
            template <typename... Args>
            static void apply(F &&f, Args &&... args) {
                f(Head{}, std::forward<Args>(args)...);
            }
        };

        /// Specialization of struct for when the tail is non-empty: calls the
        /// function, then calls the implementation/recursion method above with
        /// the tail.
        template <typename F, typename Head, typename Tail>
        struct for_each_type_impl<F, Head, Tail, false> {
            template <typename... Args>
            static void apply(F &&f, Args &&... args) {
                f(Head{}, std::forward<Args>(args)...);
                for_each_type_<F, Tail>(std::forward<F>(f),
                                        std::forward<Args>(args)...);
            }
        };
    } // namespace detail

    /// Run-time operation on a type list: given a function object, a type list,
    /// and optional arguments to be forwarded to the function call, construct
    /// each type in the type list in turn and call the function-call-operator
    /// of your functor with it (and your optional additional arguments).
    template <typename List, typename F, typename... Args>
    inline void for_each_type(F &&f, Args &&... args) {
        detail::for_each_type_<F, List>(std::forward<F>(f),
                                        std::forward<Args>(args)...);
    }
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_ForEachType_h_GUID_42B22EE3_302E_4ABD_1FC8_9D5E7146ADEF
