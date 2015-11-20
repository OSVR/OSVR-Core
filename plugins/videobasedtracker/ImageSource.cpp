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
#include "ImageSource.h"

// Library/third-party includes
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    ImageSource::~ImageSource() {}
    void ImageSource::retrieve(cv::Mat &color, cv::Mat &gray) {
        retrieveColor(color);
        cv::cvtColor(color, gray, CV_RGB2GRAY);
    }
} // namespace vbtracker
} // namespace osvr
