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
#include "TrackerThread.h"
#include "TrackedBody.h"

// Library/third-party includes
// - none

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    TrackerThread::TrackerThread(TrackingSystem &trackingSystem,
                                 ImageSource &imageSource,
                                 BodyReportingVector &reportingVec,
                                 CameraParameters const &camParams)
        : m_trackingSystem(trackingSystem), m_cam(imageSource),
          m_reportingVec(reportingVec), m_camParams(camParams) {
        msg() << "Tracker thread object created." << std::endl;
    }
    void TrackerThread::operator()() {
        /// The thread internally is organized around processing video frames,
        /// with arrival of IMU reports internally handled as they come. Thus,
        /// we keep getting frames and processing them until we're told to stop,
        /// doing what we can asynchronously to also process incoming IMU
        /// messages.

        msg() << "Tracker thread object invoked." << std::endl;
        bool keepGoing = true;
        while (keepGoing) {
            doFrame();

            {
                /// Copy the run flag.
                std::lock_guard<std::mutex> lock(m_runMutex);
                keepGoing = m_run;
            }
            if (!keepGoing) {
                msg() << "Tracker thread object: Just checked our run flag "
                         "and noticed it turned false..."
                      << std::endl;
            }
        }
        msg() << "Tracker thread object: functor exiting." << std::endl;
    }

    /// Call from the main thread!
    void TrackerThread::triggerStop() {
        msg() << "Tracker thread object: triggerStop() called" << std::endl;
        std::lock_guard<std::mutex> lock(m_runMutex);
        m_run = false;
    }

    std::ostream &TrackerThread::msg() const {
        return std::cout << "[UnifiedTracker] ";
    }
    std::ostream &TrackerThread::warn() const { return msg() << "Warning: "; }
    void TrackerThread::doFrame() {
        // Check camera status.
        if (!m_cam.ok()) {
            // Hmm, camera seems bad. Might regain it? Skip for now...
            warn() << "Camera is reporting it is not OK." << std::endl;
            return;
        }
        // Trigger a grab.
        if (!m_cam.grab()) {
            // Again failing without quitting, in hopes we get better luck
            // next
            // time...
            warn() << "Camera grab failed." << std::endl;
            return;
        }
        // When we triggered the grab is our current best guess of the time
        // for the image
        /// @todo backdate to account for image transfer image, exposure
        /// time, etc.
        auto tv = util::time::getNow();

        // Pull the image into an OpenCV matrix named m_frame.
        m_cam.retrieve(m_frame, m_frameGray);
        if (!m_frame.data || !m_frameGray.data) {
            warn() << "Camera retrieve appeared to fail: frames had null "
                      "pointers!"
                   << std::endl;
            return;
        }

        /// Launch an asynchronous task to perform the initial image
        /// processing.
        auto imageProcFuture = std::async(
            std::launch::async, [&]() -> vbtracker::ImageOutputDataPtr {
                return m_trackingSystem.performInitialImageProcessing(
                    tv, m_frame, m_frameGray, m_camParams);
            });

        /// @todo handle IMU reports in here

        /// By calling .get() on the std::future we block on the async we
        /// launched.
        // Submit to the tracking system.
        auto bodyIds =
            m_trackingSystem.updateBodiesFromVideoData(imageProcFuture.get());

        for (auto const &bodyId : bodyIds) {
            auto &body = m_trackingSystem.getBody(bodyId);
            m_reportingVec[bodyId.value()]->updateState(
                body.getStateTime(), body.getState(), body.getProcessModel());
        }
    }
} // namespace vbtracker
} // namespace osvr
