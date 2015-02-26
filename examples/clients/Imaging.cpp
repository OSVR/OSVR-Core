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

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <iostream>
#include <string>

static const std::string windowName("OSVR imaging demo | q or esc to quit");
cv::Mat frame;

int main() {
    osvr::clientkit::ClientContext context("com.osvr.exampleclients.Imaging");

    osvr::clientkit::Interface camera = context.getInterface("/camera");

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
