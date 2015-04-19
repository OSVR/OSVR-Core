/** @file
@brief Implementation of interface for an Oculus DK2 HID device

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

#include "Oculus_DK2.h"

using namespace osvr;
using namespace oculus_dk2;

static const vrpn_uint16 OCULUS_VENDOR = 0x2833;
static const vrpn_uint16 DK2_PRODUCT = 0x0021;

Oculus_DK2_HID::Oculus_DK2_HID(double keepAliveSeconds)
    : vrpn_HidInterface(m_filter = new vrpn_HidProductAcceptor(OCULUS_VENDOR, DK2_PRODUCT))
{
    // Store keep-alive interval.
    m_keepAliveSeconds = keepAliveSeconds;

    // Send a command to turn on the LEDs and record the time at which
    // we did so.
    writeLEDControl();
    vrpn_gettimeofday(&m_lastKeepAlive, NULL);
}

Oculus_DK2_HID::~Oculus_DK2_HID()
{
    // Turn off the LEDs
    writeLEDControl(false);

    // Clean up our memory.
    delete m_filter;
}

std::vector<OCULUS_IMU_REPORT> Oculus_DK2_HID::poll()
{
    // See if it has been long enough to send another keep-alive to
    // the LEDs.
    struct timeval now;
    vrpn_gettimeofday(&now, NULL);
    if (vrpn_TimevalDurationSeconds(now, m_lastKeepAlive) >= m_keepAliveSeconds) {
        writeLEDControl();
        m_lastKeepAlive = now;
    }

    // Clear old reports, which will have already been returned.
    // Read and parse any available IMU reports from the DK2, which will put
    // them into the report vector.
    m_reports.clear();
    update();
    // XXX

    return m_reports;
}

// Thank you to Oliver Kreylos for the info needed to write this function.
// It is based on his OculusRiftHIDReports.cpp, used with permission.
void Oculus_DK2_HID::writeLEDControl(
    bool enable
    , vrpn_uint16 exposureLength
    , vrpn_uint16 frameInterval
    , vrpn_uint16 vSyncOffset
    , vrpn_uint8 dutyCycle
    , vrpn_uint8 pattern
    , bool autoIncrement
    , bool useCarrier
    , bool syncInput
    , bool vSyncLock
    , bool customPattern
    , vrpn_uint16 commandId)
{
    // Buffer to store our report in.
    vrpn_uint8 pktBuffer[13];

    /* Pack the packet buffer, using little-endian packing: */
    vrpn_uint8 *bufptr = pktBuffer;
    vrpn_int32 buflen = sizeof(pktBuffer);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, vrpn_uint8(0x0cU));
    vrpn_buffer_to_little_endian(&bufptr, &buflen, commandId);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, pattern);
    vrpn_uint8 flags = 0x00U;
    if (enable) { flags |= 0x01U; }
    if (autoIncrement) { flags |= 0x02U; }
    if (useCarrier) { flags |= 0x04U; }
    if (syncInput) { flags |= 0x08U; }
    if (vSyncLock) { flags |= 0x10U; }
    if (customPattern) { flags |= 0x20U; }
    vrpn_buffer_to_little_endian(&bufptr, &buflen, flags);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, vrpn_uint8(0x0cU)); // Reserved byte
    vrpn_buffer_to_little_endian(&bufptr, &buflen, exposureLength);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, frameInterval);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, vSyncOffset);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, dutyCycle);

    /* Write the LED control feature report: */
    send_feature_report(sizeof(pktBuffer), pktBuffer);
}

void Oculus_DK2_HID::on_data_received(size_t bytes, vrpn_uint8 *buffer)
{
    // Fill new entries into the vector that will be passed back
    // on the next poll().
    // XXX
}
