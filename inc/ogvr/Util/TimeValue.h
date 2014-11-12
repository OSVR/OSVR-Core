/** @file
    @brief Header providing a C++ wrapper around TimeValueC.h

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

#ifndef INCLUDED_TimeValue_h_GUID_AD9F3D81_382D_4394_433B_A8026BE803B6
#define INCLUDED_TimeValue_h_GUID_AD9F3D81_382D_4394_433B_A8026BE803B6

// Internal Includes
#include <ogvr/Util/TimeValueC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {
namespace util {
    /// @brief Functionality related to time and the OGVR_TimeValue abstraction.
    /// @ingroup UtilTime
    ///
    /// Note that this is for C API-bordering areas. For purely C++ code, please
    /// use Boost.Chrono for your time needs.
    namespace time {
        /// @brief C++-friendly typedef for the OGVR_TimeValue structure.
        typedef ::OGVR_TimeValue TimeValue;

        /// @brief Set the given TimeValue to the current time.
        inline void getNow(TimeValue &tv) { ogvrTimeValueGetNow(&tv); }
#ifdef OGVR_HAVE_STRUCT_TIMEVAL
        /// @brief Convert a TimeValue to a struct timeval
        inline void toStructTimeval(struct timeval &dest,
                                    TimeValue const &src) {
            ogvrTimeValueToStructTimeval(&dest, &src);
        }
#endif
    } // namespace time
} // namespace util
} // namespace ogvr

#endif // INCLUDED_TimeValue_h_GUID_AD9F3D81_382D_4394_433B_A8026BE803B6
