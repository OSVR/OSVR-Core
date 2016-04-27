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

#ifndef INCLUDED_LedMeasurement_h_GUID_FB847F67_347B_4C8E_AC9B_33179ED6B0E6
#define INCLUDED_LedMeasurement_h_GUID_FB847F67_347B_4C8E_AC9B_33179ED6B0E6

// Internal Includes
#include "BasicTypes.h"

// Library/third-party includes
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

// Standard includes
#include <cassert>
#include <vector>

namespace osvr {
namespace vbtracker {
    struct LedMeasurement {
      private:
        static float estimateArea(float diameter) {
            return static_cast<float>((diameter / 2) * (diameter / 2) * CV_PI);
        }

      public:
        LedMeasurement() = default;
        /// Constructor for a measurement
        /// @param beaconArea measured area: if <= 0, will be estimated based on
        /// an assumption of circularity and the diameter.
        LedMeasurement(cv::Point2f location, float diam, cv::Size imgSize,
                       float beaconArea = -1)
            : loc(location), imageSize(imgSize), brightness(diam),
              diameter(diam),
              area(beaconArea <= 0 ? estimateArea(diameter) : beaconArea) {}

        /// Constructor for a measurement from a KeyPoint and image size.
        /// @param beaconArea measured area: if <= 0, will be estimated based on
        /// an assumption of circularity and the diameter.
        LedMeasurement(cv::KeyPoint const &kp, cv::Size imgSize,
                       float beaconArea = -1)
            : LedMeasurement(kp.pt, kp.size, imgSize, beaconArea) {
            /// Delegates to the constructor taking Point2f.
        }

        /// Constructor primarily used by replay for calibration/optimization
        /// purposes.
        LedMeasurement(float x, float y, float diam, cv::Size imgSize,
                       float beaconArea = -1)
            : LedMeasurement(cv::Point2f(x, y), diam, imgSize, beaconArea) {
            /// Delegates to the constructor taking Point2f.
        }

        /// Location in image space - should be undistorted when passed to the
        /// Led class.
        cv::Point2f loc;

        /// Size of the image the measurement came from.
        cv::Size imageSize;

        /// "Brightness" - currently actually diameter.
        Brightness brightness;

        /// Blob diameter in pixels.
        float diameter = 0.f;

        /// Area in pixels
        float area = 1.f;

        /// Blob circularity (as defined by OpenCV) - in [0,1]
        float circularity = 0.f;

        /// Do we know an upright bounding box? (that is, is the next member
        /// valid?)
        bool knowBoundingBox() const { return knowBoundingBox_; }

        /// Dimensions of the upright bounding box.
        /// only valid if knowBoundingBox();
        cv::Size2f boundingBoxSize() const {
            assert(knowBoundingBox() && "call to boundingBoxSize() invalid if "
                                        "knowBoundingBox() is false");
            return boundingBox_;
        }

        /// Set the upright bounding box (from a size)
        void setBoundingBox(cv::Size2f size) {
            knowBoundingBox_ = true;
            boundingBox_ = size;
        }

        /// Set the upright bounding box (from a Rect)
        void setBoundingBox(cv::Rect const &box) {
            knowBoundingBox_ = true;
            boundingBox_ = box.size();
        }

      private:
        /// Do we know an upright bounding box? (that is, is the next member
        /// valid?)
        bool knowBoundingBox_ = false;
        /// Dimensions of the upright bounding box.
        cv::Size2f boundingBox_;
    };

    typedef std::vector<LedMeasurement> LedMeasurementVec;
    typedef LedMeasurementVec::iterator LedMeasurementVecIterator;

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_LedMeasurement_h_GUID_FB847F67_347B_4C8E_AC9B_33179ED6B0E6
