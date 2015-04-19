/** @file
@brief Header file describing interface for an Oculus DK2 HID device.

@date 2015

@author
Sensics, Inc.
<http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
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

#pragma once

#include <vector>
#include <vrpn_HumanInterface.h>
#include <vrpn_Shared.h>

namespace osvr {
namespace oculus_dk2 {

typedef struct {
    int accel[3];
    int gyro[3];
    vrpn_uint16 timestamp;
} OCULUS_IMU_REPORT;

class Oculus_DK2_HID : public vrpn_HidInterface
{
public:
    Oculus_DK2_HID(
        double keepAliveSeconds = 9 //< How often to re-trigger the LED flashing
        );
    virtual ~Oculus_DK2_HID();

    /// Return the latest IMU report(s).  Even if reports are not desired, must be
    // called periodically to keep the LED patterns flashing.
    std::vector<OCULUS_IMU_REPORT> poll();

private:
    vrpn_HidAcceptor *d_filter;
    double d_keepAliveSeconds;
    struct timeval d_lastKeepAlive;

    void writeLEDControl(
        bool enable = true
        , vrpn_uint16 exposureLength = 350
        , vrpn_uint16 frameInterval = 16666
        , vrpn_uint16 vSyncOffset = 0
        , vrpn_uint8 dutyCycle = 127
        , vrpn_uint8 pattern = 1
        , bool autoIncrement = true
        , bool useCarrier = true
        , bool syncInput = false
        , bool vSyncLock = false
        , bool customPattern = false
        , vrpn_uint16 commandId = 0         //< Not sure what this means
        );
};

} // namespace oculus_dk2
} // namespace osvr
