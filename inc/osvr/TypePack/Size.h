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

#ifndef INCLUDED_Size_h_GUID_138C49DE_2C02_4BA6_C173_58BC95D82363
#define INCLUDED_Size_h_GUID_138C49DE_2C02_4BA6_C173_58BC95D82363

// Internal Includes
#include "CoerceList.h"
#include "SizeT.h"

// Library/third-party includes
// - none

// Standard includes
// - none
namespace osvr {
namespace typepack {
    namespace detail {
        template <typename... Ts> struct size;

        // The following fails with clang due to a bug.
        // <https://llvm.org/bugs/show_bug.cgi?id=14858>
        // template <typename... Ts> using size_impl =
        // size_t_<sizeof...(Ts)>;
        // template <typename... Ts>
        // struct size<list<Ts...>> : size_impl<Ts...> {};
        template <typename... Ts>
        struct size<list<Ts...>> : size_t_<sizeof...(Ts)> {};
    } // namespace detail

    /// @brief Get the size of a list (number of elements.)
    template <typename... Ts> using size = detail::size<coerce_list<Ts...>>;

    /// @brief Synonym for typepack::size
    template <typename... Ts> using length = size<coerce_list<Ts...>>;

} // namespace typepack
} // namespace osvr
#endif // INCLUDED_Size_h_GUID_138C49DE_2C02_4BA6_C173_58BC95D82363
