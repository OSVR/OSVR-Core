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

#ifndef INCLUDED_Concat_h_GUID_84D1F940_2859_4581_8F5D_16651CB90A21
#define INCLUDED_Concat_h_GUID_84D1F940_2859_4581_8F5D_16651CB90A21

// Internal Includes
#include "List.h"
#include "T.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {
    /// \cond
    namespace detail {
        template <typename... Lists> struct concat_ {};

        template <> struct concat_<> { using type = list<>; };

        template <typename... List1> struct concat_<list<List1...>> {
            using type = list<List1...>;
        };

        template <typename... List1, typename... List2>
        struct concat_<list<List1...>, list<List2...>> {
            using type = list<List1..., List2...>;
        };

        template <typename... List1, typename... List2, typename... List3>
        struct concat_<list<List1...>, list<List2...>, list<List3...>> {
            using type = list<List1..., List2..., List3...>;
        };

        template <typename... List1, typename... List2, typename... List3,
                  typename... Rest>
        struct concat_<list<List1...>, list<List2...>, list<List3...>, Rest...>
            : concat_<list<List1..., List2..., List3...>, Rest...> {};
    } // namespace detail
    /// \endcond

    /// Concatenates several lists into a single list.
    /// \pre The parameters must all be instantiations of \c typepack::list.
    /// \par Complexity
    /// \f$ O(L) \f$ where \f$ L \f$ is the number of lists in the list of
    /// lists.
    template <typename... Lists> using concat = t_<detail::concat_<Lists...>>;

} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Concat_h_GUID_84D1F940_2859_4581_8F5D_16651CB90A21
