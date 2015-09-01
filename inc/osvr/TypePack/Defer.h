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

#ifndef INCLUDED_Defer_h_GUID_FB817A3F_C608_43B7_BE11_2E81132DE2E0
#define INCLUDED_Defer_h_GUID_FB817A3F_C608_43B7_BE11_2E81132DE2E0

// Internal Includes
#include "List.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    /// \cond
    namespace detail {

        template <template <typename...> class, typename> struct defer_ {};

        template <template <typename...> class C, typename... Ts>
        struct defer_<C, list<Ts...>> {
            using type = C<Ts...>;
        };
    } // namespace detail
    /// \endcond

    ///////////////////////////////////////////////////////////////////////////////////////////
    // defer
    /// A wrapper that defers the instantiation of a template \p C with type
    /// parameters \p Ts in
    /// a \c lambda or \c let expression.
    ///
    /// In the code below, the lambda would ideally be written as
    /// `lambda<_a,_b,push_back<_a,_b>>`, however this fails since
    /// `push_back` expects its first
    /// argument to be a list, not a placeholder. Instead, we express it
    /// using \c defer as
    /// follows:
    ///
    /// \code
    /// template<typename List>
    /// using reverse = reverse_fold<List, list<>, lambda<_a, _b,
    /// defer<push_back, _a, _b>>>;
    /// \endcode
    template <template <typename...> class C, typename... Ts>
    struct defer : detail::defer_<C, list<Ts...>> {};

} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Defer_h_GUID_FB817A3F_C608_43B7_BE11_2E81132DE2E0
