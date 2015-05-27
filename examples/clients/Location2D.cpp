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


void printLocation2DReport(const OSVR_Location2DReport *report){

    
	std::cout << 
		report->location.data[0] << "; " <<
		report->location.data[1] << "\t" << std::endl;	

}

void location2DCallback(void * /*userdata*/,
						const OSVR_TimeValue * /*timestamp*/ ,
					 const OSVR_Location2DReport *report) {
	std::cout << "Got 2D Location Report, for sensor #" << report->sensor << std::endl;
		printLocation2DReport(report);
}


int main() {
    osvr::clientkit::ClientContext context("com.osvr.exampleclients.Location2DCallback");

    osvr::clientkit::Interface location = context.getInterface("/com_osvr_EyeTracker/EyeTracker/eyetracker");

	location.registerCallback(&location2DCallback, NULL);

	// Pretend that this is your application's mainloop.
    while (1) {
        context.update();
    }
	
    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
