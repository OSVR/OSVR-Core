/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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
#include "TrackingDebugDisplay.h"
#include "SBDBlobExtractor.h"

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp>

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    static const auto DEBUG_WINDOW_NAME = "OSVR Tracker Debug Window";
    static const auto DEBUG_FRAME_STRIDE = 11;
    TrackingDebugDisplay::TrackingDebugDisplay(ConfigParams const &params)
        : m_enabled(params.debug), m_windowName(DEBUG_WINDOW_NAME),
          m_debugStride(DEBUG_FRAME_STRIDE) {
        if (!m_enabled) {
            return;
        }
        // cv::namedWindow(m_windowName);

        std::cout << "\nVideo-based tracking debug windows help:\n";
        std::cout
#if 0
        << "  - press 's' to show the detected blobs and the status of "
           "recognized beacons (default)\n"
#endif
            << "  - press 'b' to show the labeled blobs and the "
               "reprojected beacons\n"
            << "  - press 'i' to show the raw input image\n"
            << "  - press 't' to show the blob-detecting threshold image\n"
#if 0
        << "  - press 'p' to dump the current auto-calibrated beacon "
           "positions to a CSV file\n"
#endif
            << "  - press 'q' to quit the debug windows (tracker will "
               "continue operation)\n"
            << std::endl;
    }

    void TrackingDebugDisplay::showDebugImage(cv::Mat const &image) {
        image.copyTo(m_displayedFrame);
        cv::imshow(m_windowName, m_displayedFrame);
    }

    void TrackingDebugDisplay::quitDebug() {
        if (!m_enabled) {
            return;
        }
        cv::destroyWindow(m_windowName);
        m_enabled = false;
    }

    void
    TrackingDebugDisplay::triggerDisplay(TrackingSystem::Impl const &impl) {
        if (!m_enabled) {
            /// We're not displaying things.
            return;
        }

        m_debugStride.advance();
        if (!m_debugStride) {
            /// not our turn.
            return;
        }

        /// Update the display
        switch (m_mode) {
        case DebugDisplayMode::InputImage:
            showDebugImage(impl.frame);
            break;
        case DebugDisplayMode::Thresholding:
            showDebugImage(impl.blobExtractor->getDebugThresholdImage());
            break;
        case DebugDisplayMode::Blobs:
            showDebugImage(impl.blobExtractor->getDebugBlobImage());
            break;
        }

        /// Run the event loop briefly to see if there were keyboard presses.
        int key = cv::waitKey(1);
        switch (key) {
#if 0
            case 's':
            case 'S':
                // Show the concise "status" image (default)
                /// @todo
                m_mode = DebugDisplayMode::Status;
                break;
#endif

        case 'b':
        case 'B':
            // Show the blob/keypoints image
            std::cout << "'b' pressed - Switching to the blobs image in the "
                         "debug window."
                      << std::endl;
            m_mode = DebugDisplayMode::Blobs;
            break;

        case 'i':
        case 'I':
            // Show the input image.
            std::cout << "'i' pressed - Switching to the input image in the "
                         "debug window."
                      << std::endl;
            m_mode = DebugDisplayMode::InputImage;
            break;

        case 't':
        case 'T':
            // Show the thresholded image
            std::cout << "'t' pressed - Switching to the thresholded image in "
                         "the debug window."
                      << std::endl;
            m_mode = DebugDisplayMode::Thresholding;
            break;

#if 0
            case 'p':
            case 'P':
                // Dump the beacon positions to file.
                /// @todo
                break;
#endif

        case 'q':
        case 'Q':
            // Close the debug window.
            std::cout << "'q' pressed - quitting the debug window."
                      << std::endl;
            quitDebug();
            break;

        default:
            // something else or nothing at all, no worries.
            break;
        }
    }

} // namespace vbtracker
} // namespace osvr
