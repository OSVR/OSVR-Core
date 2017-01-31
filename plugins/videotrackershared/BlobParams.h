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

#ifndef INCLUDED_BlobParams_h_GUID_ADC18202_C3F1_4CB7_65C4_77BED9F0C981
#define INCLUDED_BlobParams_h_GUID_ADC18202_C3F1_4CB7_65C4_77BED9F0C981

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    /// Blob detection configuration parameters
    struct BlobParams {
        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector - in
        /// pixel units. May not be used (not used by the EdgeHoleExtractor)
        float minDistBetweenBlobs = 3.0f;
#ifdef OSVR_USING_EDGE_HOLE_EXTRACTOR
        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector - in
        /// square pixel units. Edge hole extractor detects larger blobs, so the
        /// min area is correspondingly larger by default.
        float minArea = 3.0f;
#else
        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector - in
        /// square pixel units.
        float minArea = 2.0f;
#endif

        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector - this
        /// is faster than convexity but may be confused by side-views of LEDs.
        bool filterByCircularity = true;

        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector
        float minCircularity = 0.2f;

        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector - this
        /// is a lot more expensive than filterByCircularity
        bool filterByConvexity = true;

        /// Same meaning as the parameter to OpenCV's SimpleBlobDetector
        float minConvexity = 0.85f;

        /// This is the absolute minimum pixel value that will be considered as
        /// a possible signal. Images that contain only values below this will
        /// be totally discarded as containing zero keypoints.
        double absoluteMinThreshold = 50.;

        /// This value, in the range (0, 1), is the linear interpolation factor
        /// between the minimum and maximum value pixel in a frame that will be
        /// the *minimum* threshold value used by the simple blob detector (if
        /// it does not drop below absoluteMinThreshold)
        double minThresholdAlpha = 0.3;

        /// This value, in the range (0, 1), is the linear interpolation factor
        /// between the minimum and maximum value pixel in a frame that will be
        /// the *maximum* threshold value used by the simple blob detector (if
        /// it does not drop below absoluteMinThreshold)
        double maxThresholdAlpha = 0.8;

        /// This is the number of thresholding and contour extraction steps that
        /// the blob extractor will take between the two threshold extrema, and
        /// thus greatly impacts performance. Adjust with care. Not used by the
        /// EdgeHoleExtractor.
        int thresholdSteps = 4;
    };

    struct EdgeHoleParams {
        EdgeHoleParams();
        /// The single-axis size passed to the blur performed on the input image
        /// before the Laplacian edge detection step.
        int preEdgeDetectionBlurSize;

        /// Laplacian edge detection kernel size: must be positive and odd,
        /// advise not messing with this one.
        int laplacianKSize;

        /// Laplacian edge detection output data scale factor, passed to
        /// Laplacian function.
        double laplacianScale;

        /// Should we do an erode step after edge detection, primarily for
        /// removing mjpeg compression artifacts?
        bool edgeDetectErosion;

        /// The value in the erosion kernel.
        int erosionKernelValue;

        /// Whether we perform a blur and threshold step after the edge
        /// detection (in an attempt to smooth and join gaps in the edge
        /// detection results).
        bool postEdgeDetectionBlur;

        /// If postEdgeDetectionBlur is true, the single-axis size passed to the
        /// blur.
        int postEdgeDetectionBlurSize;

        /// If postEdgeDetectionBlur is true, the value used as a threshold to
        /// binarize the image after the blur.
        int postEdgeDetectionBlurThreshold;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_BlobParams_h_GUID_ADC18202_C3F1_4CB7_65C4_77BED9F0C981
