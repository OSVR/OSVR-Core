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
#include "DirectShowHDKCameraFactory.h"
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

static const auto FPS_MEASUREMENT_PERIOD = std::chrono::seconds(3);
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
        m_end = m_begin + FPS_MEASUREMENT_PERIOD;
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
    auto cam = getDirectShowHDKCamera();
    if (!cam || !cam->read_image_to_memory()) {
        std::cerr << "Couldn't find, open, or read from the OSVR HDK tracking "
                     "camera.\n"
                  << "Press enter to exit." << std::endl;
        std::cin.ignore();
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

    auto savedFrame = false;
    static const auto FILENAME = "capture.png";
    static const auto FILENAME_STEM = "image";
    static const auto EXTENSION = ".png";
    auto captures = std::size_t{0};
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
            if (!savedFrame) {
                savedFrame = true;
                cv::imwrite(FILENAME, frame);
                std::cout << "Wrote a captured frame to " << FILENAME
                          << std::endl;
            }
            char key = static_cast<char>(cv::waitKey(1)); // wait 1 ms for a key
            if ('q' == key || 'Q' == key || 27 /*esc*/ == key) {
                break;
            } else if ('c' == key) {
                // capture
                std::ostringstream os;
                os << FILENAME_STEM << captures << EXTENSION;
                cv::imwrite(os.str(), frame);
                std::cout << "Captured frame to " << os.str() << std::endl;
                captures++;
            }
        }
    } while (cam->read_image_to_memory());
    return 0;
}
