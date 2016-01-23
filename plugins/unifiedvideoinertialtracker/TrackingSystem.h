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

#ifndef INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50
#define INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50

// Internal Includes
#include "ConfigParams.h"
#include "ImageProcessing.h"

// Library/third-party includes
#include <osvr/Util/TimeValue.h>
#include <opencv2/core/core.hpp>

// Standard includes
#include <vector>
#include <memory>
#include <cstddef>

namespace osvr {
namespace vbtracker {
    class TrackedBody;
    class TrackingSystem {
      public:
        /// @name Setup and Teardown
        /// @{
        TrackingSystem(ConfigParams const &params);
        ~TrackingSystem();
        TrackedBody *createTrackedBody();
        /// @}

        /// @name Runtime methods
        /// @{
        /// Perform the initial phase of image processing. This does not modify
        /// the bodies, so it can happen in parallel/background processing. It's
        /// also the most expensive, so that's handy.
        ImageOutputDataPtr
        performInitialImageProcessing(util::time::TimeValue const &tv,
                                      cv::Mat const &frame,
                                      cv::Mat const &frameGray);
        using BodyIndicies = std::vector<std::size_t>;
        /// This is the second half of the video-based tracking algorithm - the
        /// part that actually changes tracking state. Please std::move the
        /// output of the first step into this step.
        ///
        /// @return A reference to a vector of body indices that were updated
        /// with this latest frame.
        BodyIndicies const &
        updateTrackingFromVideoData(ImageOutputDataPtr &&imageData);
        /// @}

        /// @name Accessors
        /// @{
        std::size_t getNumBodies() const { return m_bodies.size(); }
        TrackedBody &getBody(std::size_t i) { return *m_bodies.at(i); }
        /// @}

        /// @todo refactor;
        ConfigParams const &getParams() const { return m_params; }

      private:
        using BodyPtr = std::unique_ptr<TrackedBody>;
        ConfigParams m_params;
        std::vector<std::size_t> m_updated;
        std::vector<BodyPtr> m_bodies;

        /// private impl;
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50
