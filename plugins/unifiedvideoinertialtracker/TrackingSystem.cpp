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

// Standard includes
#include <algorithm>
#include <iterator>

namespace osvr {
namespace vbtracker {
    struct TrackingSystem::Impl {
        Impl(ConfigParams const &params)
            : blobExtractor(new SBDBlobExtractor(params)) {}
        cv::Mat frame;
        cv::Mat grayImage;
        std::unique_ptr<SBDBlobExtractor> blobExtractor;
    };

    TrackingSystem::TrackingSystem(ConfigParams const &params)
        : m_params(params), m_impl(new Impl(params)) {}

    TrackingSystem::~TrackingSystem() {}

    TrackedBody *TrackingSystem::createTrackedBody() {
        BodyPtr newBody(new TrackedBody(*this));
        m_bodies.emplace_back(std::move(newBody));
        return m_bodies.back().get();
    }

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

    BodyIndices const &TrackingSystem::updateTrackingFromVideoData(
        ImageOutputDataPtr &&imageData) {
        m_updated.clear();
        forEachTarget([&](TrackedBodyTarget &target) {
            auto usedMeasurements =
                target.processLedMeasurements(imageData->ledMeasurements);
            if (usedMeasurements != 0) {
                m_updated.push_back(target.getBody().getId());
            }
        });
        /// @todo OK, now we've updated the LEDs now update the poses.
        return m_updated;
    }

} // namespace vbtracker
} // namespace osvr
