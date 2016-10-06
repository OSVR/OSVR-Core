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
#include "CameraParameters.h"
#include "SBDBlobExtractor.h"
#include "TrackedBody.h"
#include "TrackedBodyTarget.h"
#include "TrackingSystem.h"
#include "cvToEigen.h"

// Library/third-party includes
#include <opencv2/highgui/highgui.hpp> // for GUI window

#include <opencv2/imgproc/imgproc.hpp> // for drawing capabilities

// Standard includes
#include <algorithm>
#include <iostream>
#include <vector>

namespace osvr {
namespace vbtracker {
    static const auto CVCOLOR_RED = cv::Vec3b(0, 0, 255);
    static const auto CVCOLOR_YELLOW = cv::Vec3b(0, 255, 255);
    static const auto CVCOLOR_GREEN = cv::Vec3b(0, 255, 0);
    static const auto CVCOLOR_BLUE = cv::Vec3b(255, 0, 0);
    static const auto CVCOLOR_GRAY = cv::Vec3b(127, 127, 127);
    static const auto CVCOLOR_BLACK = cv::Vec3b(0, 0, 0);
    static const auto CVCOLOR_WHITE = cv::Vec3b(255, 255, 255);
    static const auto CVCOLOR_VIOLET = cv::Vec3b(127, 0, 255);

    static const auto DEBUG_WINDOW_NAME = "OSVR Tracker Debug Window";
    static const auto DEBUG_FRAME_STRIDE = 11;
    TrackingDebugDisplay::TrackingDebugDisplay(ConfigParams const &params)
        : m_enabled(params.debug), m_windowName(DEBUG_WINDOW_NAME),
          m_debugStride(DEBUG_FRAME_STRIDE),
          m_performingOptimization(params.performingOptimization) {
        if (!m_enabled) {
            return;
        }
        // cv::namedWindow(m_windowName);
        if (m_performingOptimization) {
            m_mode = DebugDisplayMode::StatusWithAllReprojections;
        } else {
            std::cout << "\nVideo-based tracking debug windows help:\n";
            std::cout
                << "  - press 's' to show the detected blobs and the status of "
                   "recognized beacons (default)\n"
                << "  - press 'p' to show the same status view as above, but "
                   "with the (re)projected locations of even non-detected "
                   "beacons drawn\n"
                << "  - press 'b' to show the labeled blobs and the "
                   "reprojected beacons\n"
                << "  - press 'i' to show the raw input image\n"
                << "  - press 't' to show the blob-detecting threshold image\n"
                << "  - press 'q' to quit the debug windows (tracker will "
                   "continue operation)\n"
                << std::endl;
        }
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

    struct WindowCoordsPoint {
        // explicit WindowCoordsPoint(cv::Point2f p) : point(p) {}
        cv::Point2f point;
    };

    inline WindowCoordsPoint invertLoc(cv::Mat const &image, cv::Point2f loc) {

        return USING_INVERTED_LED_POSITION
                   ? WindowCoordsPoint{cv::Point2f(image.cols - loc.x,
                                                   image.rows - loc.y)}
                   : WindowCoordsPoint{loc};
    }

    inline Eigen::Vector2d pointToEigenVec(cv::Point2f pt) {
        return Eigen::Vector2d(pt.x, pt.y);
    }
    inline cv::Point2f eigenVecToPoint(Eigen::Vector2d const &vec) {
        return cv::Point2f(vec.x(), vec.y());
    }
    namespace {
        /// Utility class to hold on to a reference to an image so we can more
        /// easily do things to it.
        class DebugImage {
          public:
            explicit DebugImage(cv::Mat &im) : image_(im) {}
            DebugImage(DebugImage const &) = delete;
            DebugImage &operator=(DebugImage const &) = delete;

            /// Utility function to draw a keypoint-sized circle on the image at
            /// the LED location.
            void drawLedCircle(Led const &led, bool filled, cv::Vec3b color) {
                cv::circle(image_, led.getLocation(),
                           led.getMeasurement().diameter / 2.,
                           cv::Scalar(color), filled ? -1 : 1);
            }

            /// Utility function to label an LED with its 1-based beacon ID (in
            /// window space)
            void drawLedLabel(OneBasedBeaconId id, WindowCoordsPoint const &loc,
                              cv::Vec3b color = CVCOLOR_GRAY, double size = 0.5,
                              cv::Point2f offset = cv::Point2f(0, 0)) {
                auto label = std::to_string(id.value());
                cv::putText(image_, label, loc.point + offset,
                            cv::FONT_HERSHEY_SIMPLEX, size, cv::Scalar(color));
            }

            /// Utility function to label an LED with its 1-based beacon ID
            void drawLedLabel(OneBasedBeaconId id, cv::Point2f location,
                              cv::Vec3b color = CVCOLOR_GRAY, double size = 0.5,
                              cv::Point2f offset = cv::Point2f(0, 0)) {
                drawLedLabel(id, toWindowCoords(location), color, size, offset);
            }

            /// @overload
            /// Takes an Eigen::Vector2d as the location, with optional offset.
            void drawLedLabel(OneBasedBeaconId id, Eigen::Vector2d const &loc,
                              cv::Vec3b color = CVCOLOR_GRAY, double size = 0.5,
                              cv::Point2f offset = cv::Point2f(0, 0)) {
                drawLedLabel(id, eigenVecToPoint(loc), color, size, offset);
            }

            /// @overload
            /// Takes an LED directly, with optional offset.
            void drawLedLabel(Led const &led, cv::Vec3b color = CVCOLOR_GRAY,
                              double size = 0.5,
                              cv::Point2f offset = cv::Point2f(0, 0)) {
                drawLedLabel(led.getOneBasedID(),
                             WindowCoordsPoint{led.getLocation()}, color, size,
                             offset);
            }

            void drawStatusMessage(std::string message,
                                   cv::Vec3b color = CVCOLOR_GREEN,
                                   std::int16_t line = 1) {
                static const auto LINE_OFFSET = 20.f;
                cv::putText(image_, message, cv::Point2f(0, line * LINE_OFFSET),
                            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(color));
            }

            WindowCoordsPoint toWindowCoords(cv::Point2f loc) const {
                return USING_INVERTED_LED_POSITION
                           ? WindowCoordsPoint{cv::Point2f(image_.cols - loc.x,
                                                           image_.rows - loc.y)}
                           : WindowCoordsPoint{loc};
            }

            WindowCoordsPoint
            vecToWindowCoords(Eigen::Vector2d const &loc) const {
                return toWindowCoords(eigenVecToPoint(loc));
            }

            cv::Mat &image() { return image_; }

          private:
            cv::Mat &image_;
        };
    } // namespace
    namespace {
        /// Functor to perform reprojection of beacons from target space to
        class Reprojection {
          public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW
            Reprojection(TrackedBodyTarget const &target,
                         CameraParameters const &camParams)
                : m_body(target.getBody()),
                  m_xform3d(m_body.getState().getIsometry()),
                  m_offset(target.getTargetToBody()),
                  m_fl(camParams.focalLength()),
                  m_pp(camParams.eiPrincipalPoint()) {}

            /// Function call operator - takes in a 3d vector in body space and
            /// projects it into 2d.
            Eigen::Vector2d operator()(Eigen::Vector3d const &bodyPoint) {
                Eigen::Vector3d camPoints = m_xform3d * (bodyPoint + m_offset);
                return (camPoints.head<2>() / camPoints[2]) * m_fl + m_pp;
            }

          private:
            TrackedBody const &m_body;
            Eigen::Isometry3d m_xform3d;
            Eigen::Vector3d m_offset;
            double m_fl;
            Eigen::Vector2d m_pp;
        };
    } // namespace
    inline void drawOriginAxes(Reprojection &reproject, DebugImage &dbgImg,
                               double size = 0.02) {
        auto drawLine = [&](Eigen::Vector3d const &axis, cv::Vec3b color) {
            auto beginWindowPoint =
                dbgImg.vecToWindowCoords(reproject(axis * 0.5 * size));
            auto endWindowPoint =
                dbgImg.vecToWindowCoords(reproject(axis * -0.5 * size));
            cv::line(dbgImg.image(), beginWindowPoint.point,
                     endWindowPoint.point, cv::Scalar(color));
        };

        drawLine(Eigen::Vector3d::UnitX(), CVCOLOR_RED);
        drawLine(Eigen::Vector3d::UnitY(), CVCOLOR_GREEN);
        drawLine(Eigen::Vector3d::UnitZ(), CVCOLOR_BLUE);
    }

    inline Eigen::Vector2d
    getBeaconReprojection(Reprojection &reprojection,
                          TrackedBodyTarget const &target,
                          ZeroBasedBeaconId beaconId) {

#if 0
        // We want to un-compensate for the beacon offset here.
        return reprojection(target.getBeaconAutocalibPosition(beaconId) -
                            target.getBeaconOffset());
#else
        return reprojection(target.getBeaconAutocalibPosition(beaconId));
#endif
    }

    cv::Mat TrackingDebugDisplay::createAnnotatedBlobImage(
        TrackingSystem const &tracking, CameraParameters const &camParams,
        cv::Mat const &blobImage) {
        cv::Mat output;
        blobImage.copyTo(output);
        DebugImage img(output);
        if (tracking.getNumBodies() == 0) {
            /// No bodies - just show blobs.
            img.drawStatusMessage(
                "No tracked bodies registered, only showing detected blobs",
                CVCOLOR_RED);
            return output;
        }
        /// @todo right now, just looks at body 0, target 0 - generalize
        auto &body = tracking.getBody(BodyId{0});

        auto targetPtr = body.getTarget(TargetId{0});

        if (!targetPtr) {
            /// No optical target 0 on this body, just show blobs.
            img.drawStatusMessage(
                "No target registered on body 0, only showing detected blobs",
                CVCOLOR_RED);
            return output;
        }
        const auto labelOffset = cv::Point2f(1, 1);
        const auto textSize = 0.5;

        /// Label each of the blobs.
        auto &leds = targetPtr->leds();
        for (auto &led : leds) {
            img.drawLedLabel(led, CVCOLOR_RED, textSize, labelOffset);
        }

        if (targetPtr->hasPoseEstimate()) {
            /// Reproject the beacons.
            Reprojection reproject{*targetPtr, camParams};

            auto numBeacons = targetPtr->getNumBeacons();
            using size_type = decltype(numBeacons);
            for (size_type i = 0; i < numBeacons; ++i) {
                auto beaconId = ZeroBasedBeaconId(i);
                Eigen::Vector2d imagePoint =
                    getBeaconReprojection(reproject, *targetPtr, beaconId);
                img.drawLedLabel(makeOneBased(beaconId), imagePoint,
                                 CVCOLOR_GREEN, textSize, labelOffset);
            }
        } else {
            img.drawStatusMessage("No video tracker pose for this "
                                  "target, so green reprojection "
                                  "not shown",
                                  CVCOLOR_GRAY);
        }
        return output;
    }

    cv::Mat TrackingDebugDisplay::createStatusImage(
        TrackingSystem const &tracking, CameraParameters const &camParams,
        cv::Mat const &baseImage, bool reprojectUnseenBeacons) {
        cv::Mat output;

        DebugImage img(output);
        baseImage.copyTo(output);

        if (tracking.getNumBodies() == 0) {
            /// No bodies - show a message and swit
            img.drawStatusMessage("No tracked bodies registered, "
                                  "showing raw input image - press "
                                  "b to show blobs",
                                  CVCOLOR_RED);
            return output;
        }
        /// @todo right now, just looks at body 0, target 0 - generalize
        auto &body = tracking.getBody(BodyId{0});

        auto targetPtr = body.getTarget(TargetId{0});

        if (!targetPtr) {
            /// No optical target 0 on this body
            img.drawStatusMessage("No target registered on body 0, "
                                  "showing raw input image - press "
                                  "b to show blobs",
                                  CVCOLOR_RED);
            return output;
        }

        /// OK, so if we get here, we have a valid target, so we can start with
        /// the base image as we'd like.

        const auto textSize = 0.25;
        const auto mainBeaconLabelColor = CVCOLOR_BLUE;
        const auto baseBeaconLabelColor =
            m_performingOptimization ? CVCOLOR_WHITE : CVCOLOR_BLACK;

        auto gotPose = targetPtr->hasPoseEstimate();
        auto numBeacons = targetPtr->getNumBeacons();

        using BeaconIdContainer = std::vector<ZeroBasedBeaconId>;

        auto drawnBeaconIds = BeaconIdContainer{};
        auto recordBeaconAsDrawn = [&](ZeroBasedBeaconId id) {
            drawnBeaconIds.push_back(id);
        };

        /// Unidentified blobs look the same whether or not we have a pose, so
        /// we make a little lambda here to avoid repeating ourselves too much.
        auto drawUnidentifiedBlob = [&img](Led const &led) {
            /// Red empty circle for un-identified blob
            img.drawLedCircle(led, false, CVCOLOR_RED);
        };

        if (gotPose) {
            /// We have a pose - so we'll reproject identified beacons.
            Reprojection reproject{*targetPtr, camParams};

            /// Draw axes.
            drawOriginAxes(reproject, img);

            for (auto const &led : targetPtr->leds()) {
                if (led.identified()) {
                    /// Identified, and we have a pose

                    auto beaconId = led.getID();
                    recordBeaconAsDrawn(beaconId);

                    // Color-code identified beacons based on
                    // whether or not we used their data.
                    auto color =
                        led.wasUsedLastFrame() ? CVCOLOR_GREEN : CVCOLOR_YELLOW;
                    img.drawLedCircle(led, true, color);

                    /// Draw main label in black, then draw the reprojection in
                    /// blue on top, creating a bit of a shadow effect. Not
                    /// best visiblity ever, but...

                    /// label at keypoint location
                    img.drawLedLabel(led, baseBeaconLabelColor, textSize);

                    /// label at reprojection
                    Eigen::Vector2d imagePoint =
                        getBeaconReprojection(reproject, *targetPtr, beaconId);
                    auto windowPoint =
                        WindowCoordsPoint{eigenVecToPoint(imagePoint)};
                    img.drawLedLabel(makeOneBased(beaconId), imagePoint,
                                     mainBeaconLabelColor, textSize);
                } else {
                    drawUnidentifiedBlob(led);
                }
            }
            if (reprojectUnseenBeacons) {
                /// Sort so we can use binary_search.
                auto comparator = [](ZeroBasedBeaconId const &a,
                                     ZeroBasedBeaconId const &b) {
                    return a.value() < b.value();
                };
                std::sort(begin(drawnBeaconIds), end(drawnBeaconIds),
                          comparator);
                auto haveDrawnBeaconAlready = [&](ZeroBasedBeaconId id) {
                    return std::binary_search(begin(drawnBeaconIds),
                                              end(drawnBeaconIds), id,
                                              comparator);
                };
                /// Now, we must draw reprojections of the unseen beacons.
                auto numBeacons = targetPtr->getNumBeacons();
                for (UnderlyingBeaconIdType i = 0; i < numBeacons; ++i) {
                    auto beaconId = ZeroBasedBeaconId(i);
                    if (haveDrawnBeaconAlready(beaconId)) {
                        /// already drawn - so skip it.
                        continue;
                    }
                    /// label at reprojection
                    Eigen::Vector2d imagePoint =
                        getBeaconReprojection(reproject, *targetPtr, beaconId);
                    auto windowPoint =
                        WindowCoordsPoint{eigenVecToPoint(imagePoint)};
                    img.drawLedLabel(makeOneBased(beaconId), imagePoint,
                                     CVCOLOR_VIOLET, textSize);
                }
            }
        } else {
            /// If we don't have a pose...
            for (auto const &led : targetPtr->leds()) {
                if (led.identified()) {
                    // If identified, but we don't have a pose, draw
                    // them as yellow outlines.
                    img.drawLedCircle(led, false, CVCOLOR_YELLOW);
                    img.drawLedLabel(led, baseBeaconLabelColor, textSize);
                } else {
                    drawUnidentifiedBlob(led);
                }
            }
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
        case DebugDisplayMode::Status:
            showDebugImage(
                createStatusImage(tracking, impl.camParams, impl.frame), false);
            break;
        case DebugDisplayMode::StatusWithAllReprojections:
            showDebugImage(
                createStatusImage(tracking, impl.camParams, impl.frame, true),
                false);
            break;
        }

        /// Run the event loop briefly to see if there were keyboard presses.
        int key = cv::waitKey(1);
        if (m_performingOptimization) {
            // Don't handle any key presses - not safe to switch if running the
            // optimizer.
            return;
        }

        switch (key) {

        case 's':
        case 'S':
            // Show the concise "status" image (default)
            msg() << "'s' pressed - Switching to the status image in the "
                     "debug window."
                  << std::endl;
            m_mode = DebugDisplayMode::Status;
            break;

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

        case 'p':
        case 'P':
            msg() << "'p' pressed - Switching to the status image with all "
                     "reprojections in the debug window."
                  << std::endl;
            m_mode = DebugDisplayMode::StatusWithAllReprojections;
            break;

        case 'q':
        case 'Q':
            // Close the debug window.
            msg() << "'q' pressed - quitting the debug window." << std::endl;
            quitDebug();
            break;

        case 'o':
        case 'O':
            // toggle orientation from IMU
            /// @todo TEMPORARY DEBUGGING CODE - REMOVE!
            {
                auto newState = !tracking.getParams().imu.useOrientation;
                msg() << "Toggling orientation usage to " << std::boolalpha
                      << newState << std::endl;
                tracking.setUseIMU(newState);
                break;
            }
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
