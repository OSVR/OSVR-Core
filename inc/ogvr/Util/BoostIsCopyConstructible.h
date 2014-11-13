/** @file
    @brief Header wrapping <boost/type_traits/is_copy_constructible.hpp>
    for when we just use it with static assertions or other non-critical
    uses, since it was added in 1.55.0.

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_BoostIsCopyConstructible_h_GUID_261BD733_2559_4586_CDEF_C551ECB87C4B
#define INCLUDED_BoostIsCopyConstructible_h_GUID_261BD733_2559_4586_CDEF_C551ECB87C4B

#include <boost/version.hpp>
#if BOOST_VERSION >= 105500
#define OGVR_HAVE_BOOST_IS_COPY_CONSTRUCTIBLE
#include <boost/type_traits/is_copy_constructible.hpp>
#endif

#endif // INCLUDED_BoostIsCopyConstructible_h_GUID_261BD733_2559_4586_CDEF_C551ECB87C4B
