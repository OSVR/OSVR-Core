/** @file
    @brief Header

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

#ifndef INCLUDED_RouterPredicates_h_GUID_18D95DAC_0FC9_44A1_A22A_A41431D75A4D
#define INCLUDED_RouterPredicates_h_GUID_18D95DAC_0FC9_44A1_A22A_A41431D75A4D

// Internal Includes
// - none

// Library/third-party includes
#include <vrpn_Types.h>
#include <vrpn_Tracker.h>
#include <vrpn_Button.h>
#include <vrpn_Analog.h>

// Standard includes
// - none

namespace osvr {
namespace client {
    class SensorPredicate {
      public:
        SensorPredicate(vrpn_int32 sensor) : m_sensor(sensor) {}

        bool operator()(vrpn_ANALOGCB const &info) {
            // VRPN analogs transfer their whole state each time
            return m_sensor < info.num_channel;
        }

        bool operator()(vrpn_TRACKERCB const &info) {
            return info.sensor == m_sensor;
        }

        bool operator()(vrpn_BUTTONCB const &info) {
            return info.button == m_sensor;
        }

      private:
        vrpn_int32 m_sensor;
    };
    class AlwaysTruePredicate {
      public:
        template <typename T> bool operator()(T const &) { return true; }
    };
} // namespace client
} // namespace osvr

#endif // INCLUDED_RouterPredicates_h_GUID_18D95DAC_0FC9_44A1_A22A_A41431D75A4D
