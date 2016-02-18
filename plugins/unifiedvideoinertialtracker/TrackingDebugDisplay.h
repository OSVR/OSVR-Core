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

#ifndef INCLUDED_TrackingDebugDisplay_h_GUID_AEB0A21B_C043_4EB3_F532_8BF3D9DDEF5C
#define INCLUDED_TrackingDebugDisplay_h_GUID_AEB0A21B_C043_4EB3_F532_8BF3D9DDEF5C

// Internal Includes
#include "ConfigParams.h"
#include "TrackingSystem_Impl.h"
#include "LED.h"

// Library/third-party includes
#include <util/Stride.h>

#include <opencv2/core/core.hpp>

// Standard includes
#include <string>
#include <iosfwd>

namespace osvr {
namespace vbtracker {
    enum class DebugDisplayMode { InputImage, Thresholding, Blobs, Status };
    class TrackingSystem;
    class TrackedBodyTarget;
    struct CameraParameters;
    class TrackingDebugDisplay {
      public:
        TrackingDebugDisplay(ConfigParams const &params);

        TrackingDebugDisplay(TrackingDebugDisplay const &) = delete;
        TrackingDebugDisplay &operator=(TrackingDebugDisplay const &) = delete;

        void triggerDisplay(TrackingSystem &tracking,
                            TrackingSystem::Impl const &impl);

        void showDebugImage(cv::Mat const &image, bool needsCopy = true);

        void quitDebug();

      private:
        std::ostream &msg() const;
        cv::Mat createAnnotatedBlobImage(TrackingSystem const &tracking,
                                         CameraParameters const &camParams,
                                         cv::Mat const &blobImage);
        cv::Mat createStatusImage(TrackingSystem const &tracking,
                                  CameraParameters const &camParams,
                                  cv::Mat const &baseImage);

        bool m_enabled;
        DebugDisplayMode m_mode = DebugDisplayMode::Status;
        std::string m_windowName;
        cv::Mat m_displayedFrame;
        ::util::Stride m_debugStride;
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_TrackingDebugDisplay_h_GUID_AEB0A21B_C043_4EB3_F532_8BF3D9DDEF5C
