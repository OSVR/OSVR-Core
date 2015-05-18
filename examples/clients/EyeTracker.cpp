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
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/Util/ClientReportTypesC.h>

// Library/third-party includes

// Standard includes
#include <iostream>
#include <string>


void printEyeTrackerReport(const OSVR_EyeTrackerReport *report){


	std::cout << "2D Data:" <<
		report->gaze.gazeDirection2D.data[0] << "; " <<
		report->gaze.gazeDirection2D.data[1] << std::endl;

	std::cout << "3D Data:" <<
		report->gaze.gazeDirection3D.data[0] << "; " <<
		report->gaze.gazeDirection3D.data[1] << "; " <<
		report->gaze.gazeDirection3D.data[2] << std::endl;

}

void eyeTrackerCallback(void * /*userdata*/,
						const OSVR_TimeValue * /*timestamp*/ ,
					 const OSVR_EyeTrackerReport *report) {
	std::cout << "Got Eye Tracker Report: " << std::endl;
		printEyeTrackerReport(report);
}


int main() {
    osvr::clientkit::ClientContext context("com.osvr.exampleclients.EyeTrackerCallback");

    osvr::clientkit::Interface eyetracker = context.getInterface("/eyetracker");

	//eyetracker.registerCallback(&eyeTrackerCallback, NULL);

	// Pretend that this is your application's mainloop.
    while (1) {
        context.update();
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
