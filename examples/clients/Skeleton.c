/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/ClientKit/ContextC.h>
#include <osvr/ClientKit/InterfaceC.h>
#include <osvr/ClientKit/InterfaceStateC.h>

// Library/third-party includes

// Standard includes
#include <stdio.h>

int main() {
    OSVR_ClientContext ctx =
        osvrClientInit("com.osvr.exampleclients.SkeletonCallback", 0);

    OSVR_ClientInterface leftHand = NULL;

    osvrClientGetInterface(
        ctx, "/com_osvr_example_Skeleton/Skeleton/skeleton/0", &leftHand);

    // Pretend that this is your application's mainloop.
    while (1) {
        osvrClientUpdate(ctx);
        // get some skeleton Reports
    }

    osvrClientShutdown(ctx);
    printf("Library shut down, exiting.\n");
    return 0;
}
