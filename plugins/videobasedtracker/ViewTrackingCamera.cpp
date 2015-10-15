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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "DirectShowToCV.h"
#include "directx_camera_server.h"

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <memory>
#include <iostream>
#include <chrono>
#include <sstream>

/// @brief OpenCV's simple highgui module refers to windows by their name, so we
/// make this global for a simpler demo.
static const std::string windowNameAndInstructions(
    "OSVR tracking camera preview | q or esc to quit");

// This string begins the DevicePath provided by Windows for the HDK's camera.
static const auto HDK_CAMERA_PATH_PREFIX = "\\\\?\\usb#vid_0bda&pid_57e8&mi_00";

class FrameCounter {
  public:
    FrameCounter() { reset(); }

    void gotFrame() {
        ++m_frames;
        auto now = clock::now();
        if (now >= m_end) {
            auto duration =
                std::chrono::duration_cast<std::chrono::duration<double>>(
                    now - m_begin);
            std::cout << m_frames / duration.count() << " FPS read from camera"
                      << std::endl;
            reset();
        }
    }

    void reset() {
        m_begin = clock::now();
        m_end = m_begin + std::chrono::seconds(1);
        m_frames = 0;
    }

  private:
    using clock = std::chrono::system_clock;
    using time_point = std::chrono::time_point<clock>;
    time_point m_begin;
    time_point m_end;
    std::size_t m_frames = 0;
};

int main(int argc, char *argv[]) {
    auto cam = std::unique_ptr<directx_camera_server>{
        new directx_camera_server(HDK_CAMERA_PATH_PREFIX)};
    if (!cam->read_image_to_memory()) {
        std::cerr
            << "Couldn't find, open, or read from the OSVR HDK tracking camera."
            << std::endl;
        return -1;
    }
    auto FRAME_DISPLAY_STRIDE = 3u;
    if (argc > 1) {
        auto is = std::istringstream{argv[1]};
        if (is >> FRAME_DISPLAY_STRIDE) {
            std::cout << "Custom display stride passed: "
                      << FRAME_DISPLAY_STRIDE << std::endl;
        } else {
            std::cout << "Could not parse first command-line argument as a "
                         "display stride : '"
                      << argv[1] << "' (will use default)" << std::endl;
        }
    }
    std::cout << "Will display 1 out of every " << FRAME_DISPLAY_STRIDE
              << " frames captured." << std::endl;
    auto frame = cv::Mat{};

    FrameCounter counter;
    cv::namedWindow(windowNameAndInstructions);
    auto frameCount = std::size_t{0};
    do {
        frame = retrieve(*cam);
        counter.gotFrame();
        ++frameCount;
        if (frameCount % FRAME_DISPLAY_STRIDE == 0) {
            frameCount = 0;
            cv::imshow(windowNameAndInstructions, frame);

            char key = static_cast<char>(cv::waitKey(1)); // wait 1 ms for a key
            if ('q' == key || 'Q' == key || 27 /*esc*/ == key) {
                break;
            }
        }
    } while (cam->read_image_to_memory());
    return 0;
}
