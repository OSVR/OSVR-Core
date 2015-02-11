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

#ifndef INCLUDED_TrackerServerInterface_h_GUID_2FECCA11_65B7_41AA_9326_8CD10202FA64
#define INCLUDED_TrackerServerInterface_h_GUID_2FECCA11_65B7_41AA_9326_8CD10202FA64

// Internal Includes
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {

    /// @brief Interface for external access to generating tracker reports.
    ///
    /// Does not inherit from BaseServerInterface because it wraps an existing
    /// VRPN class.
    class TrackerServerInterface {
      public:
        virtual void sendReport(OSVR_PositionState const &val,
                                OSVR_ChannelCount chan,
                                util::time::TimeValue const &timestamp) = 0;

        virtual void sendReport(OSVR_OrientationState const &val,
                                OSVR_ChannelCount chan,
                                util::time::TimeValue const &timestamp) = 0;

        virtual void sendReport(OSVR_PoseState const &val,
                                OSVR_ChannelCount chan,
                                util::time::TimeValue const &timestamp) = 0;
    };

} // namespace connection
} // namespace osvr

#endif // INCLUDED_TrackerServerInterface_h_GUID_2FECCA11_65B7_41AA_9326_8CD10202FA64
