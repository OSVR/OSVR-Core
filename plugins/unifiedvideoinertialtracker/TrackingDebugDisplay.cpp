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
#include "TrackingSystem.h"
#include "TrackedBody.h"
#include "TrackedBodyTarget.h"
#include "CameraParameters.h"
#include "cvToEigen.h"

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp> // for GUI window

#include <opencv2/imgproc/imgproc.hpp> // for drawing capabilities

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    // static const auto RED_SCALAR = cv::Scalar(0, 0, 255);
    static const auto CVCOLOR_RED = cv::Vec3b(0, 0, 255);
    static const auto CVCOLOR_YELLOW = cv::Vec3b(0, 255, 255);
    static const auto CVCOLOR_GREEN = cv::Vec3b(0, 255, 0);
    static const auto CVCOLOR_GRAY = cv::Vec3b(127, 127, 127);

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

    void TrackingDebugDisplay::showDebugImage(cv::Mat const &image,
                                              bool needsCopy) {
        if (needsCopy) {
            image.copyTo(m_displayedFrame);
        } else {
            m_displayedFrame = image;
        }
        cv::imshow(m_windowName, m_displayedFrame);
    }

    void TrackingDebugDisplay::quitDebug() {
        if (!m_enabled) {
            return;
        }
        cv::destroyWindow(m_windowName);
        m_enabled = false;
    }

    /// Utility function to draw a keypoint-sized circle on the image at the LED
    /// location.
    inline void drawLedCircleOnImage(cv::Mat &image, Led const &led,
                                     bool filled, cv::Vec3b color) {
        cv::circle(image, led.getLocation(), led.getMeasurement().diameter / 2.,
                   cv::Scalar(color), filled ? -1 : 1);
    }

    /// Utility function to label an LED with its 1-based beacon ID
    inline void drawLedLabelOnImage(cv::Mat &image, OneBasedBeaconId id,
                                    cv::Point2f location,
                                    cv::Vec3b color = CVCOLOR_GRAY,
                                    double size = 0.5) {
        auto label = std::to_string(id.value());
        cv::putText(image, label, location, cv::FONT_HERSHEY_SIMPLEX, size,
                    cv::Scalar(color));
    }

    /// @overload
    /// Takes an LED directly, with optional offset.
    inline void drawLedLabelOnImage(cv::Mat &image, Led const &led,
                                    cv::Vec3b color = CVCOLOR_GRAY,
                                    double size = 0.5,
                                    cv::Point2f offset = cv::Point2f(0, 0)) {
        drawLedLabelOnImage(image, led.getOneBasedID(),
                            led.getLocation() + offset, color, size);
    }
    namespace {
        /// Functor to perform reprojection of beacons from target space to
        class Reprojection {
          public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            Reprojection(TrackedBodyTarget const &target,
                         CameraParameters const &camParams)
                : m_body(target.getBody()),
                  m_xform3d(m_body.getState().getIsometry()),
                  m_fl(camParams.focalLength()),
                  m_pp(camParams.eiPrincipalPoint()) {}

            /// Function call operator - takes in a 3d vector in body space and
            /// projects it into 2d.
            Eigen::Vector2d operator()(Eigen::Vector3d const &bodyPoint) {
                Eigen::Vector3d camPoints = m_xform3d * bodyPoint;
                return (camPoints.head<2>() / camPoints[2]) * m_fl + m_pp;
            }

          private:
            TrackedBody const &m_body;
            Eigen::Isometry3d m_xform3d;
            double m_fl;
            Eigen::Vector2d m_pp;
        };
    } // namespace
    cv::Mat TrackingDebugDisplay::createAnnotatedBlobImage(
        TrackingSystem const &tracking, CameraParameters const &camParams,
        cv::Mat const &blobImage) {
        cv::Mat output;
        blobImage.copyTo(output);

        if (tracking.getNumBodies() == 0) {
            /// No bodies - just show blobs.
            return output;
        }
        /// @todo right now, just looks at body 0, target 0 - generalize
        auto &body = tracking.getBody(BodyId{0});

        auto targetPtr = body.getTarget(TargetId{0});

        if (!targetPtr) {
            /// No optical target 0 on this body, just show blobs.
            return output;
        }
        const auto labelOffset = cv::Point2f(1, 1);
        const auto textSize = 0.5;

        /// Label each of the blobs.
        auto &leds = targetPtr->leds();
        for (auto &led : leds) {
            drawLedLabelOnImage(output, led, CVCOLOR_RED, textSize,
                                labelOffset);
        }

        if (targetPtr->hasPoseEstimate()) {
            //msg() << "Have a pose estimate, will reproject\n";
            /// Reproject the beacons.
            Reprojection reproject{*targetPtr, camParams};

            auto numBeacons = targetPtr->getNumBeacons();
            using size_type = decltype(numBeacons);
            for (size_type i = 0; i < numBeacons; ++i) {
                auto beaconId = ZeroBasedBeaconId(i);
                Eigen::Vector2d imagePoint =
                    reproject(targetPtr->getBeaconAutocalibPosition(beaconId));
                drawLedLabelOnImage(
                    output, makeOneBased(beaconId),
                    cv::Point2f(imagePoint.x(), imagePoint.y()) + labelOffset,
                    CVCOLOR_GREEN, textSize);
            }
        } else {

            //msg() << "Can't reproject\n";
        }
        return output;
    }

    void
    TrackingDebugDisplay::triggerDisplay(TrackingSystem &tracking,
                                         TrackingSystem::Impl const &impl) {
        if (!m_enabled) {
            /// We're not displaying things.
            return;
        }

        m_debugStride.advance();
        if (!m_debugStride) {
            /// not our turn.
            return;
        }
        auto &blobEx = impl.blobExtractor;
        /// Update the display
        switch (m_mode) {
        case DebugDisplayMode::InputImage:
            showDebugImage(impl.frame);
            break;
        case DebugDisplayMode::Thresholding:
            showDebugImage(blobEx->getDebugThresholdImage());
            break;
        case DebugDisplayMode::Blobs:
            showDebugImage(
                createAnnotatedBlobImage(tracking, impl.camParams,
                                         blobEx->getDebugBlobImage()),
                false);
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
            msg() << "'b' pressed - Switching to the blobs image in the "
                     "debug window."
                  << std::endl;
            m_mode = DebugDisplayMode::Blobs;
            break;

        case 'i':
        case 'I':
            // Show the input image.
            msg() << "'i' pressed - Switching to the input image in the "
                     "debug window."
                  << std::endl;
            m_mode = DebugDisplayMode::InputImage;
            break;

        case 't':
        case 'T':
            // Show the thresholded image
            msg() << "'t' pressed - Switching to the thresholded image in "
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
            msg() << "'q' pressed - quitting the debug window." << std::endl;
            quitDebug();
            break;

        default:
            // something else or nothing at all, no worries.
            break;
        }
    }

    std::ostream &TrackingDebugDisplay::msg() const {
        return std::cout << "[Tracking Debug Display] ";
    }

} // namespace vbtracker
} // namespace osvr
