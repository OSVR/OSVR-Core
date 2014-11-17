/** @file
    @brief Header forward-declaring TimeValue C++ API.

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

#ifndef INCLUDED_TimeValue_fwd_h_GUID_5F1C94A1_49EC_438E_2640_ED67A6F85ED3
#define INCLUDED_TimeValue_fwd_h_GUID_5F1C94A1_49EC_438E_2640_ED67A6F85ED3

struct OSVR_TimeValue;
namespace osvr {
namespace util {
    namespace time {
        typedef ::OSVR_TimeValue TimeValue;
    } // namespace time
} // namespace util
} // namespace osvr

#endif // INCLUDED_TimeValue_fwd_h_GUID_5F1C94A1_49EC_438E_2640_ED67A6F85ED3
