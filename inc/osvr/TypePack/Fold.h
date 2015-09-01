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

#ifndef INCLUDED_Fold_h_GUID_63DEE2B1_33B4_47B3_1289_B30F2B5BEA6B
#define INCLUDED_Fold_h_GUID_63DEE2B1_33B4_47B3_1289_B30F2B5BEA6B

// Internal Includes
#include "SplitList.h"
#include "T.h"
#include "List.h"
#include "Apply.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    namespace detail {
        // Fold: Forward declaration of general form
        template <typename List, typename State, typename Fun> struct fold_;

        // Fold: Recurse
        template <typename List, typename State, typename Fun>
        struct fold_
            : fold_<tail<List>, t_<apply<Fun, State, head<List>>>, Fun> {};

        // Fold: base case
        template <typename State, typename Fun>
        struct fold_<list<>, State, Fun> {
            using type = State;
        };

    } // namespace detail

    /// @brief Fold the list (right) with the given alias class and initial
    /// state.
    template <typename List, typename State, typename Fun>
    using fold = t_<detail::fold_<List, State, Fun>>;

} // namespace typepack
} // namespace osvr

#endif // INCLUDED_Fold_h_GUID_63DEE2B1_33B4_47B3_1289_B30F2B5BEA6B
