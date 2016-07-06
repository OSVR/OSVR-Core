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
#include "EdgeHoleBasedLedExtractor.h"
#include "OptionalStream.h"
#include "cvUtils.h"

#ifdef OSVR_USE_REALTIME_LAPLACIAN
#include "RealtimeLaplacian.h"
#endif

// Library/third-party includes
#ifdef OSVR_UVBI_CORE
/// Only trace when building the uvbi-core library.
#include <osvr/Common/Tracing.h>
#endif

// Standard includes
#include <iostream>
#include <utility>

namespace osvr {
namespace vbtracker {
    static const std::uint8_t MAX_JPG_EDGEDETECT_NOISE = 20;
    static const auto PREFIX = "[EdgeHoleBasedLedExtractor] ";
    EdgeHoleParams::EdgeHoleParams()
        : preEdgeDetectionBlurSize(3), laplacianKSize(3), laplacianScale(5),
          edgeDetectErosion(false),
          erosionKernelValue(MAX_JPG_EDGEDETECT_NOISE),
          postEdgeDetectionBlur(true), postEdgeDetectionBlurSize(3),
          postEdgeDetectionBlurThreshold(80) {}

    static const int EDGE_DETECT_DEST_DEPTH = CV_8U;

    EdgeHoleBasedLedExtractor::EdgeHoleBasedLedExtractor(
        EdgeHoleParams const &extractorParams)
        : extParams_(extractorParams)
#ifdef OSVR_USE_REALTIME_LAPLACIAN
          ,
          laplacianImpl_(new RealtimeLaplacian(EDGE_DETECT_DEST_DEPTH,
                                               extParams_.laplacianKSize,
                                               extParams_.laplacianScale))
#endif
    {

        compressionArtifactRemovalKernel_ =
            cv::Mat::ones(cv::Size(3, 3), CV_8U) *
            static_cast<std::uint8_t>(extractorParams.erosionKernelValue);
#ifdef OSVR_OPENCV_2
        compressionArtifactRemoval_ = cv::createMorphologyFilter(
            cv::MORPH_ERODE, CV_8U, compressionArtifactRemovalKernel_);
#endif
    }
#ifdef OSVR_UVBI_CORE
    namespace tracing = ::osvr::common::tracing;
    class BlobExtraction
        : public tracing::TracingRegion<tracing::MainTracePolicy> {
      public:
        BlobExtraction()
            : tracing::TracingRegion<tracing::MainTracePolicy>(
                  "BlobExtraction") {}
    };
#endif

    LedMeasurementVec const &EdgeHoleBasedLedExtractor::
    operator()(cv::Mat const &gray, BlobParams const &p,
               bool verboseBlobOutput) {
        reset();

#ifdef OSVR_UVBI_CORE
        BlobExtraction trace;
#endif

        verbose_ = verboseBlobOutput;

        gray.copyTo(gray_);

        /// Set up the threshold parameters
        auto rangeInfo = ImageRangeInfo(gray_);
        if (rangeInfo.maxVal < p.absoluteMinThreshold) {
            /// Early out - empty image!
            return measurements_;
        }

        auto thresholdInfo = ImageThresholdInfo(rangeInfo, p);
        minBeaconCenterVal_ =
            static_cast<std::uint8_t>(thresholdInfo.minThreshold);

        /// Used to do basic thresholding here first to reduce background noise,
        /// but turns out that actually produced worse results at the end of the
        /// process (presumably by producing very sharp edges)
        // MatType blurred;

        cv::GaussianBlur(gray_, blurred_,
                         cv::Size(extParams_.preEdgeDetectionBlurSize,
                                  extParams_.preEdgeDetectionBlurSize),
                         0, 0);

#ifdef OSVR_USE_REALTIME_LAPLACIAN
        /// Edge detection: re-apply our partially prepared laplacian to this
        /// frame now.
        laplacianImpl_->apply(blurred_, edge_);
#else
        /// Edge detection: apply a laplacian filter to this frame
        cv::Laplacian(blurred_, edge_, CV_8U, extParams_.laplacianKSize,
                      extParams_.laplacianScale);
#endif

        /// removal of mjpeg artifacts.
        if (extParams_.edgeDetectErosion) {
#ifdef OSVR_OPENCV_2
            compressionArtifactRemoval_->apply(edge_, edge_);
#else
            cv::erode(edge_, edge_, compressionArtifactRemovalKernel_);
#endif
        }

        // turn the edge detection into a binary image.
        if (extParams_.postEdgeDetectionBlur) {
            cv::GaussianBlur(edge_, edgeTemp_,
                             cv::Size(extParams_.postEdgeDetectionBlurSize,
                                      extParams_.postEdgeDetectionBlurSize),
                             0, 0);
            cv::threshold(edgeTemp_, edgeBinary_,
                          extParams_.postEdgeDetectionBlurThreshold, 255,
                          cv::THRESH_BINARY);
        } else {
            cv::threshold(edge_, edgeBinary_,
                          extParams_.postEdgeDetectionBlurThreshold, 255,
                          cv::THRESH_BINARY);
        }

        /// Extract beacons from the edge detection image

        // The lambda ("continuation") is called with each "hole" in the edge
        // detection image, it's up to us what to do with the contour we're
        // given. We examine it for suitability as an LED, and if it passes our
        // checks, add a derived measurement to our measurement vector and the
        // contour itself to our list of contours for debugging display.
        edgeBinary_.copyTo(binTemp_);
        consumeHolesOfConnectedComponents(binTemp_, [&](ContourType &&contour) {
            checkBlob(std::move(contour), p);
        });
        return measurements_;
    }
    /// out of line for unique_ptr-based pimpl.
    EdgeHoleBasedLedExtractor::~EdgeHoleBasedLedExtractor() = default;

    void EdgeHoleBasedLedExtractor::reset() {
        contours_.clear();
        measurements_.clear();
        rejectList_.clear();
        contourId_ = 0;
    }
    void EdgeHoleBasedLedExtractor::checkBlob(ContourType &&contour,
                                              BlobParams const &p) {

        auto data = getBlobDataFromContour(contour);
        auto debugStream = [&] {
#ifdef OSVR_DEBUG_CONTOUR_CONDITIONS
            return outputIf(std::cout, true);
#else
            return outputIf(std::cout, verbose_);
#endif
        };
        auto myId = contourId_;
        contourId_++;
        debugStream() << "\nContour ID " << myId << " centered at "
                      << data.center;
        debugStream() << " - diameter: " << data.diameter;
        debugStream() << " - area: " << data.area;
        debugStream() << " - circularity: " << data.circularity;
        debugStream() << " - bounding box size: " << data.bounds.size();
        if (data.area < p.minArea) {
            debugStream() << "Reject based on area: " << data.area << " < "
                          << p.minArea << "\n";

            addToRejectList(myId, RejectReason::Area, data);
            return;
        }

        {
            /// Check to see if we accidentally picked up a non-LED
            /// stuck between a few bright ones.
            cv::Mat patch;
            cv::getRectSubPix(gray_, cv::Size(1, 1),
                              castPointToFloat(data.center), patch);
            auto centerPointValue = patch.at<unsigned char>(0, 0);
            if (centerPointValue < minBeaconCenterVal_) {
                debugStream() << "Reject based on center point value: "
                              << int(centerPointValue) << " < "
                              << int(minBeaconCenterVal_) << "\n";
                addToRejectList(myId, RejectReason::CenterPointValue, data);
                return;
            }
        }

        if (p.filterByCircularity) {
            if (data.circularity < p.minCircularity) {
                debugStream()
                    << "Reject based on circularity: " << data.circularity
                    << " < " << p.minCircularity << "\n";
                addToRejectList(myId, RejectReason::Circularity, data);
                return;
            }
        }
        if (p.filterByConvexity) {
            auto convexity = getConvexity(contour, data.area);
            debugStream() << " - convexity: " << convexity;
            if (convexity < p.minConvexity) {
                debugStream() << "Reject based on convexity: " << convexity
                              << " < " << p.minConvexity << "\n";
                addToRejectList(myId, RejectReason::Convexity, data);

                return;
            }
        }

        debugStream() << "Accepted!\n";
        {
            auto newMeas =
                LedMeasurement(castPointToFloat(data.center),
                               static_cast<float>(data.diameter), gray_.size(),
                               static_cast<float>(data.area));
            newMeas.circularity = static_cast<float>(data.circularity);
            newMeas.setBoundingBox(data.bounds);

            measurements_.emplace_back(std::move(newMeas));
        }
        contours_.emplace_back(std::move(contour));
    }
} // namespace vbtracker
} // namespace osvr
