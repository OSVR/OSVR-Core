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

#ifndef INCLUDED_HasType_h_GUID_C99B75F2_4F44_49BD_6152_3CF9B0B80DAB
#define INCLUDED_HasType_h_GUID_C99B75F2_4F44_49BD_6152_3CF9B0B80DAB

// Internal Includes
#include "Void.h"

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace typepack {
    /// \cond
    namespace detail {
        template <typename, typename = void> struct has_type_ {
            using type = std::false_type;
        };

        template <typename T> struct has_type_<T, void_<typename T::type>> {
            using type = std::true_type;
        };

    } // namespace detail
    /// \endcond

    /// An alias for `std::true_type` if `T::type` exists and names a type;
    /// otherwise, it's an alias for `std::false_type`.
    template <typename T> using has_type = t_<detail::has_type_<T>>;
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_HasType_h_GUID_C99B75F2_4F44_49BD_6152_3CF9B0B80DAB
