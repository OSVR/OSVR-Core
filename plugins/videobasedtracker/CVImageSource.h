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

#ifndef INCLUDED_CVImageSource_h_GUID_4DB5B33E_0FFD_40A5_D38D_92FDA7D8326D
#define INCLUDED_CVImageSource_h_GUID_4DB5B33E_0FFD_40A5_D38D_92FDA7D8326D

// Internal Includes
#include "ImageSource.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    /// Factory method to open an OpenCV camera.
    ImageSourcePtr openOpenCVCamera(int which);
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_CVImageSource_h_GUID_4DB5B33E_0FFD_40A5_D38D_92FDA7D8326D
