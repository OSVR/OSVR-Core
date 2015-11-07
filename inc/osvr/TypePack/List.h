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
// Part of OSVR-Core.
//
// Incorporates code from:
// Copyright Eric Niebler 2014-2015
//
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/meta
//

#ifndef INCLUDED_List_h_GUID_4A334D86_D6A9_4A99_045F_FFAEEB10CB76
#define INCLUDED_List_h_GUID_4A334D86_D6A9_4A99_045F_FFAEEB10CB76

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace typepack {

    namespace detail {
        struct list_base_ {};
    } // namespace detail

    /// @brief A wrapper for a template parameter pack of types.
    ///
    /// Note that passing a single list<...> as the parameter to list<> will not
    /// change the type (doesn't nest the lists), so this is safe. If you need
    /// to ensure some argument is just a list, see typepack::coerce_list.
    template <typename... Ts> struct list : detail::list_base_ {
        using type = list;
    };
    template <typename... Ts> struct list<list<Ts...>> : list<Ts...>::type {};

} // namespace typepack
} // namespace osvr

#endif // INCLUDED_List_h_GUID_4A334D86_D6A9_4A99_045F_FFAEEB10CB76
