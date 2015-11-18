/** @file
    @brief The MultipleAsync plugin example, converted to plain C as a sample.
    All the functionality of the other example plugins (hardware detection
    callbacks, device instantiation callbacks, etc.) can also be performed in
    plain C.

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

/* Internal Includes */
#include <osvr/PluginKit/PluginKitC.h>
#include <osvr/PluginKit/AnalogInterfaceC.h>
#include <osvr/PluginKit/ButtonInterfaceC.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include <osvr/Util/BoolC.h>

#include "org_osvr_example_SampleCPlugin_json.h"

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>

typedef struct MyDevice {
    OSVR_DeviceToken devToken;
    OSVR_AnalogDeviceInterface analog;
    OSVR_ButtonDeviceInterface button;
    OSVR_TrackerDeviceInterface tracker;
    double myVal;
    OSVR_CBool buttonPressed;
} MyDevice;

static OSVR_ReturnCode myDeviceUpdate(void *userdata) {
    MyDevice *mydev = (MyDevice *)userdata;
    /* dummy time-wasting loop - simulating "blocking waiting for device data",
     * which is possible because this is an async device. */
    int i;
    for (i = 0; i < 1000; ++i) {
    }

    /* Make up some dummy data that changes to report. */
    mydev->myVal = (mydev->myVal + 0.1);
    if (mydev->myVal > 10.) {
        mydev->myVal = 0.;
    }

    /* Report the value of channel 0 */
    osvrDeviceAnalogSetValue(mydev->devToken, mydev->analog, mydev->myVal, 0);

    /* Toggle the button 0 */
    if (OSVR_TRUE == mydev->buttonPressed) {
        mydev->buttonPressed = OSVR_FALSE;
    } else {
        mydev->buttonPressed = OSVR_TRUE;
    }
    osvrDeviceButtonSetValue(mydev->devToken, mydev->button,
                             (mydev->buttonPressed == OSVR_TRUE)
                                 ? OSVR_BUTTON_PRESSED
                                 : OSVR_BUTTON_NOT_PRESSED,
                             0);

    /* Report the identity pose for sensor 0 */
    OSVR_PoseState pose;
    osvrPose3SetIdentity(&pose);
    osvrDeviceTrackerSendPose(mydev->devToken, mydev->tracker, &pose, 0);

    return OSVR_RETURN_SUCCESS;
}

/* Startup routine for a device instance - similar function as the constructor
 * in the C++ examples. Note that error checking (return values) has been
 * omitted for clarity - see documentation. */
static void myDeviceInit(OSVR_PluginRegContext ctx, MyDevice *mydev) {
    /* Set initial values in the struct. */
    mydev->myVal = 0.;
    mydev->buttonPressed = OSVR_FALSE;

    /* Create the initialization options */
    OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

    /* Indicate that we'll want 1 analog channel. */
    osvrDeviceAnalogConfigure(opts, &mydev->analog, 1);

    /* Indicate that we'll want 1 button. */
    osvrDeviceButtonConfigure(opts, &mydev->button, 1);

    /* Indicate that we'll report tracking too. */
    osvrDeviceTrackerConfigure(opts, &mydev->tracker);

    /* Create the async device token with the options */
    osvrDeviceSyncInitWithOptions(ctx, "MyDevice", opts, &mydev->devToken);

    /* Send the JSON device descriptor. */
    osvrDeviceSendJsonDescriptor(mydev->devToken,
                                 org_osvr_example_SampleCPlugin_json,
                                 sizeof(org_osvr_example_SampleCPlugin_json));

    /* Register update callback */
    osvrDeviceRegisterUpdateCallback(mydev->devToken, &myDeviceUpdate,
                                     (void *)mydev);
}
/* Shutdown and free routine for a device instance - parallels the combination
 * of the destructor and `delete`/`osvr::util::generic_deleter<>` in the C++
 * examples */
static void myDeviceShutdown(void *mydev) {
    printf("Destroying sample device\n");
    free(mydev);
}

OSVR_PLUGIN(org_osvr_example_SampleCPlugin) {
    /* Allocate a struct for our device data. */
    MyDevice *mydev = (MyDevice *)malloc(sizeof(MyDevice));

    /* Ask the server to tell us when to shutdown the device and free this
     * struct */
    osvrPluginRegisterDataWithDeleteCallback(ctx, &myDeviceShutdown,
                                             (void *)mydev);

    /* Call a function to set up the device callbacks, etc. */
    myDeviceInit(ctx, mydev);

    return OSVR_RETURN_SUCCESS;
}
