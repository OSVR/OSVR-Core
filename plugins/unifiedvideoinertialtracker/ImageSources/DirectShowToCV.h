/** @file
    @brief Header

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

#ifndef INCLUDED_DirectShowToCV_h_GUID_C9B19C46_CA43_4481_A5B9_CB76397DD2C3
#define INCLUDED_DirectShowToCV_h_GUID_C9B19C46_CA43_4481_A5B9_CB76397DD2C3

// Internal Includes
// - none

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
// - none

template <typename CameraType> inline cv::Mat retrieve(CameraType &camera) {
    int minx, miny, maxx, maxy;
    camera.read_range(minx, maxx, miny, maxy);
    auto height = maxy - miny + 1;
    auto width = maxx - minx + 1;
    // auto frame = cv::Mat(height, width, CV_8UC3, camera.get_pixel_buffer());
    auto frame =
        cv::Mat(camera.get_pixel_buffer()).reshape(3 /*channels*/, height);
    auto ret = cv::Mat{};
    cv::flip(frame, ret, 0);
    return ret;
}

#endif // INCLUDED_DirectShowToCV_h_GUID_C9B19C46_CA43_4481_A5B9_CB76397DD2C3
