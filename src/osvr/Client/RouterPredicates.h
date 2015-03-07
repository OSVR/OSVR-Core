/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
