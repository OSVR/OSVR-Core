/** @file
    @brief Header

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_SkeletonInterfaceC_h_GUID_69382E69_DFA1_4FB0_5287_D874B740B4C0
#define INCLUDED_SkeletonInterfaceC_h_GUID_69382E69_DFA1_4FB0_5287_D874B740B4C0

/* Internal Includes */
#include <osvr/PluginKit/DeviceInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

OSVR_EXTERN_C_BEGIN

/** @defgroup PluginKitCSkeleton Skeleton interface (base C API)
    @brief Sends notifications that skeleton reports (aka tracker reports) are
   complete. Skeleton interface is implemented as a device that exposes a
   tracker interface for each skeleton element. For each element, a separate
   tracker report will be sent with an updated pose followed by
   skeletonComplete. and the client can request a complete report of entire
   skeleton (all reported elements), as well as subsets. Note that since it
   exposes a device via multiple interfaces, you will need to "configure" both
   interfaces (Tracker and Skeleton).
    @ingroup PluginKit
    @{
*/

/** @brief Opaque type used in conjunction with a device token to send data on
    Skeleton Interface
*/
typedef struct OSVR_SkeletonDeviceInterfaceObject *OSVR_SkeletonDeviceInterface;

/** @brief Specify that your device will implement the Skeleton interface.

    @param opts The device init options object.
    @param [out] iface An interface object you should retain with the same
    lifetime as the device token in order to send messages conforming to a
   Skeleton interface.
    @param numSensors The number of non-connected skeletons you will be
   reporting. Note that the number of skeleton sensors is not the same as number
   of skeleton elements. For example if plugin device reports two hands that
   will be 2 separate skeleton sensors because hands are not connected via
   common parent. If device can report an entire skeleton then it should only
   report one sensor.
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceSkeletonConfigure(OSVR_INOUT_PTR OSVR_DeviceInitOptions opts,
                            OSVR_OUT_PTR OSVR_SkeletonDeviceInterface *iface,
                            OSVR_IN OSVR_ChannelCount numSensors)
    OSVR_FUNC_NONNULL((1, 2));

/** @brief Report that transmission of tracker reports for given skeleton sensor
   is complete. This method should be called after device reports updated poses
   for skeleton elements for given skeleton sensor. For example, if device is
   reporting two hands (two skeleton sensors), it should report all poses for
   left hand followed by osvrDeviceSkeletonComplete(leftSensor) call and then
   report all poses for right hand followed by
   osvrDeviceSkeletonComplete(rightSensor). This ensures that client receives
   consistent, single-frame reports and avoids bone stretching.
    @param dev Device token
    @param iface Skeleton Interface
    @param sensor Sensor number
    @param timestamp The same timestamp as for your tracker reports
*/
OSVR_PLUGINKIT_EXPORT
OSVR_ReturnCode
osvrDeviceSkeletonComplete(OSVR_IN_PTR OSVR_SkeletonDeviceInterface iface,
                           OSVR_IN OSVR_ChannelCount sensor,
                           OSVR_IN_PTR OSVR_TimeValue const *timestamp)
    OSVR_FUNC_NONNULL((1, 3));

/**@} */ /* end of group */

OSVR_EXTERN_C_END

#endif // INCLUDED_SkeletonInterfaceC_h_GUID_69382E69_DFA1_4FB0_5287_D874B740B4C0
