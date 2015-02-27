/** @file
    @brief Header providing a C++ wrapper around TimeValueC.h

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
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
#include <osvr/Util/TimeValueC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Functionality related to time and the OSVR_TimeValue abstraction.
    /// @ingroup UtilTime
    ///
    /// Note that this is for C API-bordering areas. For purely C++ code, please
    /// use Boost.Chrono for your time needs.
    namespace time {
        /// @brief C++-friendly typedef for the OSVR_TimeValue structure.
        typedef ::OSVR_TimeValue TimeValue;

        /// @brief Set the given TimeValue to the current time.
        inline void getNow(TimeValue &tv) { osvrTimeValueGetNow(&tv); }
#ifdef OSVR_HAVE_STRUCT_TIMEVAL
        /// @brief Convert a TimeValue to a struct timeval
        inline void toStructTimeval(struct timeval &dest,
                                    TimeValue const &src) {
            osvrTimeValueToStructTimeval(&dest, &src);
        }
        /// @brief Convert a struct timeval to a TimeValue
        inline void fromStructTimeval(TimeValue &dest,
                                      struct timeval const &src) {
            osvrStructTimevalToTimeValue(&dest, &src);
        }

        /// @brief Convert a struct timeval to a TimeValue
        inline TimeValue fromStructTimeval(struct timeval const &src) {
            TimeValue dest;
            osvrStructTimevalToTimeValue(&dest, &src);
            return dest;
        }
#ifdef OSVR_STRUCT_TIMEVAL_INCLUDED
        /// @brief Convert a TimeValue to a struct timeval
        inline struct timeval toStructTimeval(TimeValue const &src) {
            struct timeval dest;
            osvrTimeValueToStructTimeval(&dest, &src);
            return dest;
        }
#endif

#endif
    } // namespace time
} // namespace util
} // namespace osvr

#endif // INCLUDED_TimeValue_h_GUID_AD9F3D81_382D_4394_433B_A8026BE803B6
