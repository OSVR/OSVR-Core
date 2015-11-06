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

#ifndef INCLUDED_Always_h_GUID_0FAFDF98_A560_45B9_A434_D3C02136BA6E
#define INCLUDED_Always_h_GUID_0FAFDF98_A560_45B9_A434_D3C02136BA6E

// Internal Includes
#include "T.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    /// @brief A Alias Class that always returns \p T.
    template <typename T> struct always {
      private:
        // Redirect through a class template for compilers that have not
        // yet implemented CWG 1558:
        // <http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#1558>
        template <typename...> struct impl { using type = T; };

      public:
        template <typename... Ts> using apply = t_<impl<Ts...>>;
    };
} // namespace typepack
} // namespace osvr

#endif // INCLUDED_Always_h_GUID_0FAFDF98_A560_45B9_A434_D3C02136BA6E
