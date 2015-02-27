/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_ReportStateTraits_h_GUID_44BA0D27_B1B4_4A8C_5D47_2AFECC2C2BEE
#define INCLUDED_ReportStateTraits_h_GUID_44BA0D27_B1B4_4A8C_5D47_2AFECC2C2BEE

// Internal Includes
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace client {
    namespace traits {
        /// @brief Type predicate: Whether callbacks of a report type should
        /// store state for that type.
        template <typename T> struct KeepStateForReport : std::true_type {};

        /// @brief Don't store the imaging reports as state, since doing so
        /// would involve extra lifetime management.
        template <>
        struct KeepStateForReport<OSVR_ImagingReport> : std::false_type {};

    } // namespace traits

} // namespace client
} // namespace osvr
#endif // INCLUDED_ReportStateTraits_h_GUID_44BA0D27_B1B4_4A8C_5D47_2AFECC2C2BEE
