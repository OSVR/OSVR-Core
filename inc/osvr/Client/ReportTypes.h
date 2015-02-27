/** @file
    @brief Header containing a typelist of all special report types.

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

#ifndef INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9
#define INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>

// Library/third-party includes
#include <boost/mpl/vector.hpp>

// Standard includes
// - none

namespace osvr {
namespace client {
    /// @brief Type traits and metaprogramming supports related to
    /// specially-handled reports.
    namespace traits {
        /// @brief A typelist containing all specially-handled report types.
        typedef boost::mpl::vector<OSVR_AnalogReport, OSVR_ButtonReport,
                                   OSVR_PoseReport, OSVR_PositionReport,
                                   OSVR_OrientationReport,
                                   OSVR_ImagingReport> ReportTypes;

    } // namespace traits

} // namespace client
} // namespace osvr

#endif // INCLUDED_ReportTypes_h_GUID_BB8F47AD_5F82_439E_C3F9_7C57D016D3C9
