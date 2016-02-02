/** @file
    @brief Implementation

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

// Internal Includes
#include "TrackingSystem.h"
#include "TrackedBody.h"
#include "TrackedBodyTarget.h"
#include "SBDBlobExtractor.h"
#include "UndistortMeasurements.h"

// Library/third-party includes
#include <boost/assert.hpp>
#include <util/Stride.h>

// Standard includes
#include <algorithm>
#include <iterator>

namespace osvr {
namespace vbtracker {
    enum class DebugDisplayMode { InputImage, Thresholding, Blobs };

    static const auto DEBUG_FRAME_STRIDE = 11;
    class TrackingDebugDisplay {
      public:
        TrackingDebugDisplay(ConfigParams const &params)
            : m_enabled(params.debug), m_debugStride(DEBUG_FRAME_STRIDE) {}

        void processFrame(TrackingSystem::Impl &impl);

      private:
        bool m_enabled;
        DebugDisplayMode m_mode = DebugDisplayMode::Blobs;
        ::util::Stride m_debugStride;
    };

    struct TrackingSystem::Impl {
        Impl(ConfigParams const &params)
            : blobExtractor(new SBDBlobExtractor(params)),
              debugDisplay(params) {}
        cv::Mat frame;
        cv::Mat frameGray;
        util::time::TimeValue lastFrame;
        LedUpdateCount updateCount;
        std::unique_ptr<SBDBlobExtractor> blobExtractor;
        TrackingDebugDisplay debugDisplay;
    };

    TrackingSystem::TrackingSystem(ConfigParams const &params)
        : m_params(params), m_impl(new Impl(params)) {}

    TrackingSystem::~TrackingSystem() {}

    TrackedBody *TrackingSystem::createTrackedBody() {
        auto newId = BodyId(m_bodies.size());
        BodyPtr newBody(new TrackedBody(*this, newId));
        m_bodies.emplace_back(std::move(newBody));
        return m_bodies.back().get();
    }

    TrackedBodyTarget *TrackingSystem::getTarget(BodyTargetId target) {
        return getBody(target.first).getTarget(target.second);
    }

#if 0
    BodyId TrackingSystem::getIdForBody(TrackedBody const &body) const {
        BodyId ret;

        auto bodyPtr = &body;
        /// Find iterator to the body whose smart pointer's raw contents compare
        /// equal to the raw address we just got.
        auto it = std::find_if(
            begin(m_bodies), end(m_bodies),
            [bodyPtr](BodyPtr const &ptr) { return ptr.get() == bodyPtr; });

        BOOST_ASSERT_MSG(end(m_bodies) != it,
                         "Shouldn't be able to be asked about a tracked "
                         "body that's not in our tracking system!");
        if (end(m_bodies) == it) {
            /// Return an empty/invalid ID if we couldn't find it - that would
            /// be weird.
            return ret;
        }
        ret = BodyId(std::distance(begin(m_bodies), it));
        return ret;
    }
#endif

    ImageOutputDataPtr TrackingSystem::performInitialImageProcessing(
        util::time::TimeValue const &tv, cv::Mat const &frame,
        cv::Mat const &frameGray, CameraParameters const &camParams) {

        ImageOutputDataPtr ret(new ImageProcessingOutput);
        ret->tv = tv;
        ret->frame = frame;
        ret->frameGray = frameGray;
        auto rawMeasurements =
            m_impl->blobExtractor->extractBlobs(ret->frameGray);
        ret->ledMeasurements = undistortLeds(rawMeasurements, camParams);
        return ret;
    }

    LedUpdateCount const &
    TrackingSystem::updateLedsFromVideoData(ImageOutputDataPtr &&imageData) {
        /// Clear internal data, we're invalidating things here.
        m_updated.clear();
        auto &updateCount = m_impl->updateCount;
        updateCount.clear();

        /// Update our frame cache, since we're taking ownership of the image
        /// data now.
        m_impl->frame = imageData->frame;
        m_impl->frameGray = imageData->frameGray;

        /// Go through each target and try to process the measurements.
        forEachTarget([&](TrackedBodyTarget &target) {
            auto usedMeasurements =
                target.processLedMeasurements(imageData->ledMeasurements);
            if (usedMeasurements != 0) {
                updateCount[target.getQualifiedId()] = usedMeasurements;
            }
        });
        return updateCount;
    }

    BodyIndices const &
    TrackingSystem::updateBodiesFromVideoData(ImageOutputDataPtr &&imageData) {
        updateLedsFromVideoData(std::move(imageData));

        auto const &updateCount = m_impl->updateCount;
        for (auto &bodyWithMeasurements : updateCount) {
            auto bodyId = bodyWithMeasurements.first;
        }
        updatePoseEstimates();
        return m_updated;
    }

    void TrackingSystem::updatePoseEstimates() {}

} // namespace vbtracker
} // namespace osvr
