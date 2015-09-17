/** @file
    @brief C Imaging interface sample

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

/* Internal Includes */
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceCallbackC.h>
#include <osvr/ClientKit/ImagingC.h>

/* Standard includes */
#include <stdio.h>

unsigned int reportNumber = 0;

void imagingCallback(void *userdata, const OSVR_TimeValue *timestamp,
                      const OSVR_ImagingReport *report) {
    /* @todo implement a C equivalent to the C++ empty() check here. */
    OSVR_ClientContext* ctx = (OSVR_ClientContext*)userdata;

    /* The first time, let's print some info. */
    if (reportNumber == 0) {
        printf("Got first report: image is %d width and %d height.\n",
        report->state.metadata.width, report->state.metadata.height);
    }
    else {
        printf("Got report number %d\n", reportNumber);
    }

    reportNumber++;
    if (OSVR_RETURN_SUCCESS != osvrClientFreeImage(*ctx, report->state.data)) {
        printf("Error, osvrClientFreeImage call failed.\n");
    }
}

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("com.osvr.exampleclients.Imaging", 0);
    if(!ctx) {
        printf("Error, could not initialize client context. The server may not be running.\n");
        return -1;
    }

    OSVR_ClientInterface camera = NULL;
    if(OSVR_RETURN_SUCCESS != osvrClientGetInterface(ctx, "/camera", &camera)) {
        printf("Error, could not get the camera interface at /camera.\n");
        return -1;
    }

    /* Register the imaging callback. */
    if(OSVR_RETURN_SUCCESS != osvrRegisterImagingCallback(camera, &imagingCallback, &ctx)) {
        printf("Error, could not register image callback.");
        return -1;
    }

    /* Pretend that this is your application's mainloop. */
    int i;
    for (i = 0; i < 1000000; ++i) {
        if(OSVR_RETURN_SUCCESS != osvrClientUpdate(ctx)) {
            printf("Error, while updating the client context.");
        }
    }

    osvrClientShutdown(ctx);
    printf("Library shut down, exiting.\n");
}
