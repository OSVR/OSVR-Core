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

#ifndef INCLUDED_Contains_h_GUID_39658344_E4B2_4196_15DF_546AE1F0DF0B
#define INCLUDED_Contains_h_GUID_39658344_E4B2_4196_15DF_546AE1F0DF0B

// Internal Includes
#include "ApplyList.h"
#include "Transform.h"
#include "Or.h"
#include "Quote.h"

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace typepack {

    namespace detail {

        template <typename T> struct is_ {
            template <typename Elt> using apply = std::is_same<T, Elt>;
        };
    }

    /// @brief Determines if type @p Needle is in the list @p Haystack
    template <typename Haystack, typename Needle>
    using contains =
        apply_list<quote<or_>, transform<Haystack, detail::is_<Needle>>>;

} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Contains_h_GUID_39658344_E4B2_4196_15DF_546AE1F0DF0B
