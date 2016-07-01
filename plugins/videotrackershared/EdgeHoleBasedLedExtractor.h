/** @file
    @brief Header

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

#ifndef INCLUDED_EdgeHoleBasedLedExtractor_h_GUID_225A962F_C636_4CA5_2D16_964D63A0571A
#define INCLUDED_EdgeHoleBasedLedExtractor_h_GUID_225A962F_C636_4CA5_2D16_964D63A0571A

// Internal Includes
#include <BlobExtractor.h>
#include <BlobParams.h>
#include <LedMeasurement.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <cstdint>
#include <memory>
#include <tuple>
#include <vector>

/// @todo Sadly can't enable this until we have a persistent thread for image
/// processing, since thread-local storage is used by OpenCV for its OpenCL
/// state.
#undef OSVR_PERMIT_OPENCL

#if ((defined(CV_VERSION_EPOCH) && CV_VERSION_EPOCH >= 3) ||                   \
     (!defined(CV_VERSION_EPOCH) && defined(CV_VERSION_MAJOR) &&               \
      CV_VERSION_MAJOR >= 3)) &&                                               \
    defined(OSVR_PERMIT_OPENCL)
#define OSVR_EDGEHOLE_UMAT 1
#else
#define OSVR_EDGEHOLE_UMAT 0
#endif

namespace osvr {
namespace vbtracker {
    /// forward declaration
    class RealtimeLaplacian;

    enum class RejectReason { Area, CenterPointValue, Circularity, Convexity };
    class EdgeHoleBasedLedExtractor {
      public:
#if OSVR_EDGEHOLE_UMAT
        using MatType = cv::UMat;
        using ExternalMatGetterReturn = cv::Mat;
#else
        using MatType = cv::Mat;
        using ExternalMatGetterReturn = cv::Mat const &;
#endif

        explicit EdgeHoleBasedLedExtractor(
            EdgeHoleParams const &extractorParams = EdgeHoleParams());
        LedMeasurementVec const &operator()(cv::Mat const &gray,
                                            BlobParams const &p,
                                            bool verboseBlobOutput = false);
        ~EdgeHoleBasedLedExtractor();

        using ContourId = std::size_t;
        // Contour ID and center.
        using RejectType = std::tuple<ContourId, RejectReason, cv::Point2d>;
        using RejectList = std::vector<RejectType>;

        void reset();

        ExternalMatGetterReturn getInputGrayImage() const {
            return externalMatGetter(gray_);
        }
        ExternalMatGetterReturn getEdgeDetectedImage() const {
            return externalMatGetter(edge_);
        }
        ExternalMatGetterReturn getEdgeDetectedBinarizedImage() const {
            return externalMatGetter(edgeBinary_);
        }
        ContourList const &getContours() const { return contours_; }
        LedMeasurementVec const &getMeasurements() const {
            return measurements_;
        }
        RejectList const &getRejectList() const { return rejectList_; }

      private:
#if OSVR_EDGEHOLE_UMAT
        static ExternalMatGetterReturn externalMatGetter(MatType const &input) {
            return input.getMat(cv::ACCESS_READ);
        }
#else
        static ExternalMatGetterReturn externalMatGetter(MatType const &input) {
            return input;
        }
#endif
        void checkBlob(ContourType &&contour, BlobParams const &p);
        void addToRejectList(ContourId id, RejectReason reason,
                             BlobData const &data) {
            rejectList_.emplace_back(id, reason, data.center);
        }
        std::uint8_t minBeaconCenterVal_ = 127;

        /// @name Frames/intermediates someone might care about
        /// @{
        MatType gray_;
        MatType edge_;
        MatType edgeBinary_;
        /// @}

        /// @name Temporary intermediate frames
        /// @brief kept around to avoid allocation in OpenCV each frame
        /// @{
        /// Copy of gray_ for blurring before edge detection
        MatType blurred_;
        /// Copy of edge_ for post-edge detection blurring prior to threshold
        MatType edgeTemp_;
        /// copy of edgeBinary_ consumed destructively by findContours
        MatType binTemp_;
        /// @}

        /// Erosion filter to remove spurious edges pointing out the camera gave
        /// us an mjpeg-compressed stream.
        cv::Ptr<cv::FilterEngine> compressionArtifactRemoval_;

        std::unique_ptr<RealtimeLaplacian> laplacianImpl_;

        ContourList contours_;
        LedMeasurementVec measurements_;
        RejectList rejectList_;
        bool verbose_ = false;

        ContourId contourId_ = 0;

        /// parameters
        const EdgeHoleParams extParams_;
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_EdgeHoleBasedLedExtractor_h_GUID_225A962F_C636_4CA5_2D16_964D63A0571A
