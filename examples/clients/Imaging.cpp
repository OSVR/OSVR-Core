/** @file
    @brief Implementation

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

// Internal Includes
#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>
#include <osvr/ClientKit/Imaging.h>

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <iostream>
#include <string>

/// @brief OpenCV's simple highgui module refers to windows by their name, so we
/// make this global for a simpler demo.
static const std::string windowName("OSVR imaging demo | q or esc to quit");

/// @brief We keep a copy of the last report to avoid de-allocating the image
/// buffer until we have a new report.
osvr::clientkit::ImagingReportOpenCV lastReport;

void imagingCallback(void *userdata,
                     osvr::util::time::TimeValue const &timestamp,
                     osvr::clientkit::ImagingReportOpenCV report) {
    if (report.frame.empty()) {
        std::cout << "Error, frame empty!" << std::endl;
        return;
    }

    cv::imshow(windowName, report.frame);
    lastReport = report;
}
int main() {
    osvr::clientkit::ClientContext context("com.osvr.exampleclients.Imaging");

    osvr::clientkit::Interface camera = context.getInterface("/camera");

    // Register the imaging callback.
    osvr::clientkit::registerImagingCallback(camera, &imagingCallback, NULL);

    // Pretend that this is your application's mainloop.
    // We're using a simple OpenCV "highgui" loop here.
    cv::namedWindow(windowName, CV_WINDOW_KEEPRATIO);
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
