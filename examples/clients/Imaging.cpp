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
#include <osvr/ClientKit/Imaging.h>
#include <osvr/Util/OpenCVTypeDispatch.h>

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <iostream>
#include <string>

/// @brief OpenCV's simple highgui module refers to windows by their name, so we
/// make this global for a simpler demo.
static const std::string
    windowNameAndInstructions("OSVR imaging demo | q or esc to quit");

bool gotSomething = false;

void imagingCallback(void *userdata,
                     osvr::util::time::TimeValue const &timestamp,
                     osvr::clientkit::ImagingReport report) {
    // Convert the image pointer into an OpenCV matrix.
    cv::Mat frame(report.metadata.height, report.metadata.width,
        osvr::util::computeOpenCVMatType(report.metadata),
        report.buffer.get());

    if (frame.empty()) {
        std::cout << "Error, frame empty!" << std::endl;
        return;
    }

    /// The first time, let's print some info.
    if (!gotSomething) {
        gotSomething = true;
        std::cout << "Got first report: image is " << frame.cols << "x"
                  << frame.rows << std::endl;
    }

    cv::imshow(windowNameAndInstructions, frame);
    osvr::clientkit::ImagingReport &lastReport =
        *static_cast<osvr::clientkit::ImagingReport *>(userdata);
    lastReport = report;
}
int main() {
    osvr::clientkit::ClientContext context("com.osvr.exampleclients.Imaging");

    osvr::clientkit::Interface camera = context.getInterface("/camera");

    /// We keep a copy of the last report to avoid de-allocating the image
    /// buffer until we have a new report.
    osvr::clientkit::ImagingReport lastReport;

    // Register the imaging callback.
    osvr::clientkit::registerImagingCallback(camera, &imagingCallback,
                                             &lastReport);

    // Output instructions to the console.
    std::cout << std::endl << windowNameAndInstructions << std::endl;

    // Pretend that this is your application's mainloop.
    // We're using a simple OpenCV "highgui" loop here.
    cv::namedWindow(windowNameAndInstructions);
    while (1) {
        context.update();
        char key = static_cast<char>(cv::waitKey(1)); // wait 1 ms for a key
        if ('q' == key || 'Q' == key || 27 /*esc*/ == key) {
            break;
        }
    }

    std::cout << "Library shut down, exiting." << std::endl;
    return 0;
}
