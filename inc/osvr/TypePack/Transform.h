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

#ifndef INCLUDED_Transform_h_GUID_901ECEAD_C608_4DAE_CA89_CE3C6EF135B6
#define INCLUDED_Transform_h_GUID_901ECEAD_C608_4DAE_CA89_CE3C6EF135B6

// Internal Includes
#include "Apply.h"
#include "CoerceList.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    namespace detail {

        template <typename, typename> struct transform_ {};
        template <typename... Ts, typename Fun>
        struct transform_<list<Ts...>, Fun> {
            /// required for MSVC2013 to avoid "there are no parameter packs
            /// available to expand"
            template <typename T>
            struct apply_workaround : apply<Fun, T>::type {};
            typedef coerce_list<typename apply_workaround<Ts>::type...> type;
        };
    } // namespace detail
    template <typename List, typename Fun>
    using transform = t_<detail::transform_<List, Fun>>;
} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Transform_h_GUID_901ECEAD_C608_4DAE_CA89_CE3C6EF135B6
