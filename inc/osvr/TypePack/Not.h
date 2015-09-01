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

#ifndef INCLUDED_Not_h_GUID_F161831F_2D35_4288_38A9_782ED10CC625
#define INCLUDED_Not_h_GUID_F161831F_2D35_4288_38A9_782ED10CC625

// Internal Includes
#include "Bool.h"

// Library/third-party includes
// - none

// Standard includes
// - none
namespace osvr {
namespace typepack {

    /// Logical not on a single boolean.
    template <typename Bool> using not_ = bool_<!Bool::value>;

} // namespace typepack
} // namespace osvr

#endif // INCLUDED_Not_h_GUID_F161831F_2D35_4288_38A9_782ED10CC625
