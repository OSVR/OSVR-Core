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

#ifndef INCLUDED_T_h_GUID_7DC620D4_E3EA_4FB4_0C0E_9E9F512138EB
#define INCLUDED_T_h_GUID_7DC620D4_E3EA_4FB4_0C0E_9E9F512138EB

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {
    /// @brief A convenience alias template to extract the nested `type` within
    /// the supplied `T`.
    ///
    /// The name was chosen to parallel how the traits in C++14 (like
    /// `std::enable_if`) have been complemented by aliases ending in `_t` (like
    /// `std::enable_if_t<COND>` being equivalent to `typename
    /// std::enable_if<COND>::type`)
    ///
    /// Note that the name is `t_`, unlike in `meta` where the semi-illegal name
    /// `_t` is used. (Leading underscores are between risky and not permitted.)
    template <typename T> using t_ = typename T::type;
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_T_h_GUID_7DC620D4_E3EA_4FB4_0C0E_9E9F512138EB
