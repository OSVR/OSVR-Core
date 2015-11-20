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

#ifndef INCLUDED_ImageSourceFactories_h_GUID_9C2DA062_802C_41A0_E014_82E9EB8A7D5F
#define INCLUDED_ImageSourceFactories_h_GUID_9C2DA062_802C_41A0_E014_82E9EB8A7D5F

// Internal Includes
#include "ImageSource.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    /// Factory method to open an OpenCV camera by number.
    ImageSourcePtr openOpenCVCamera(int which);

#ifdef _WIN32
    /// Factory method to get the HDK camera as an image source, via DirectShow.
    ImageSourcePtr openHDKCameraDirectShow();
#endif

    /// Factory method to open a directory of tif files named 0001.tif and
    /// onward as an image source (looping)
    ImageSourcePtr openImageFileSequence(std::string const &dir);

    /// Factory method to wrap an image source, already determined to be an
    /// Oculus DK2 camera, with unscrambling and keep-alive code.
    ImageSourcePtr openDK2WrappedCamera(ImageSourcePtr &&cam, bool doHid);
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_ImageSourceFactories_h_GUID_9C2DA062_802C_41A0_E014_82E9EB8A7D5F
