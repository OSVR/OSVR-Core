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

#ifndef INCLUDED_TrackingSystem_Impl_h_GUID_9EC4CAF8_58AA_45A5_59A0_6B1FB4B86BE7
#define INCLUDED_TrackingSystem_Impl_h_GUID_9EC4CAF8_58AA_45A5_59A0_6B1FB4B86BE7

// Internal Includes
#include "TrackingSystem.h"
#include "CameraParameters.h"
#include "ConfigParams.h"
#include "RoomCalibration.h"

// Library/third-party includes
#include <osvr/Util/TimeValue.h>
#include <osvr/Util/EigenCoreGeometry.h>
#include <opencv2/core/core.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
#include <memory>

namespace osvr {
namespace vbtracker {
    class TrackingDebugDisplay;
    class SBDBlobExtractor;
    /// Private implementation structure for TrackingSystem
    struct TrackingSystem::Impl : private boost::noncopyable {
        Impl(ConfigParams const &params);
        ~Impl();
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        void triggerDebugDisplay(TrackingSystem &tracking);

        /// @name Cached data from the ImageProcessingOutput updated in phase 2
        /// @{
        /// Cached copy of the last grey frame
        cv::Mat frame;
        /// Cached copy of the last grey frame
        cv::Mat frameGray;
        /// Cached copy of the last (undistorted) camera parameters to be used.
        CameraParameters camParams;
        util::time::TimeValue lastFrame;
        /// @}
        bool roomCalibCompleteCached = false;

        bool haveCameraPose = false;
        Eigen::Isometry3d cameraPose;
        Eigen::Isometry3d cameraPoseInv;

        RoomCalibration calib;

        LedUpdateCount updateCount;
        std::unique_ptr<SBDBlobExtractor> blobExtractor;
        std::unique_ptr<TrackingDebugDisplay> debugDisplay;
    };

} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_TrackingSystem_Impl_h_GUID_9EC4CAF8_58AA_45A5_59A0_6B1FB4B86BE7
