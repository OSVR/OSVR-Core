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
#include "TrackedBodyIMU.h"
#include "SpaceTransformations.h"

// Library/third-party includes
#include <osvr/Util/Finally.h>
#include <osvr/Util/EigenInterop.h>

// Standard includes
#include <iostream>
#include <future>

#define OSVR_TRACKER_THREAD_WRAP_WITH_TRY

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
    TrackerThread::~TrackerThread() {
        if (m_imageThread.joinable()) {
            m_imageThread.join();
        }
    }

    void TrackerThread::permitStart() {
        m_startupSignal.set_value();
    }

    void TrackerThread::threadAction() {
        /// The thread internally is organized around processing video frames,
        /// with arrival of IMU reports internally handled as they come. Thus,
        /// we keep getting frames and processing them until we're told to stop,
        /// doing what we can asynchronously to also process incoming IMU
        /// messages.

        msg() << "Tracker thread object invoked, waiting for permitStart()." << std::endl;
        m_startupSignal.get_future().wait();
        /// sleep an extra half a second to give everyone else time to get off the starting blocks.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        msg() << "Tracker thread object entering its main execution loop."
              << std::endl;

#ifdef OSVR_TRACKER_THREAD_WRAP_WITH_TRY
        try {
#endif
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
#ifdef OSVR_TRACKER_THREAD_WRAP_WITH_TRY
        } catch (std::exception const &e) {
            warn() << "Tracker thread object: exiting because of caught "
                      "exception: "
                   << e.what() << std::endl;
            m_run = false;
        }
#endif
        msg() << "Tracker thread object: functor exiting." << std::endl;
    }

    void TrackerThread::triggerStop() {
        /// Main thread method!
        msg() << "Tracker thread object: triggerStop() called" << std::endl;
        std::lock_guard<std::mutex> lock(m_runMutex);
        m_run = false;
    }

    void TrackerThread::submitIMUReport(TrackedBodyIMU &imu,
                                        util::time::TimeValue const &tv,
                                        OSVR_OrientationReport const &report) {
        /// Main thread method!
        {
            std::lock_guard<std::mutex> lock(m_messageMutex);
            m_messages.push(std::make_tuple(&imu, tv, report));
        }
        m_messageCondVar.notify_one();
    }
    void
    TrackerThread::submitIMUReport(TrackedBodyIMU &imu,
                                   util::time::TimeValue const &tv,
                                   OSVR_AngularVelocityReport const &report) {
        /// Main thread method!
        {
            std::lock_guard<std::mutex> lock(m_messageMutex);
            m_messages.push(std::make_tuple(&imu, tv, report));
        }
        m_messageCondVar.notify_one();
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
            // next time...
            warn() << "Camera grab failed." << std::endl;
            return;
        }
        // When we triggered the grab is our current best guess of the time
        // for the image
        /// @todo backdate to account for image transfer image, exposure
        /// time, etc.
        m_triggerTime = util::time::getNow();

        /// Launch an asynchronous task to perform the image retrieval and
        /// initial image processing.
        launchTimeConsumingImageStep();

        bool finishedImage = false;
        do {

            MessageEntry message = boost::none;
            {
                /// Wait for something to do (Completion of image, IMU reports)
                std::unique_lock<std::mutex> lock(m_messageMutex);
                m_messageCondVar.wait(lock, [&] {
                    return m_timeConsumingImageStepComplete ||
                           !m_messages.empty();
                });
                if (m_timeConsumingImageStepComplete) {
                    /// Set a flag to get us out of this innermost loop - we'll
                    /// finish up processing this frame and trigger another grab
                    /// before we look at more IMU data.
                    finishedImage = true;
                } else {
                    // OK, we have some IMU reports to keep us busy in the
                    // meantime. Grab the first one and we'll process it while
                    // not holding the mutex.
                    message = m_messages.front();
                    m_messages.pop();
                }
            } // unlock

            if (!message.empty()) {
                processIMUMessage(message);
            }
        } while (!finishedImage);

        // OK, once we get here, we know the timeConsumingImageStep is complete.
        if (!m_frame.data || !m_frameGray.data) {
            // but it ended early due to error.
            warn() << "Camera retrieve appeared to fail: frames had null "
                      "pointers!"
                   << std::endl;
            return;
        }

        if (!m_imageData) {
            // but it failed to set the pointer? this is very strange...
            warn() << "Initial image processing failed somehow!" << std::endl;
            return;
        }

        // Submit initial image data to the tracking system.
        auto bodyIds =
            m_trackingSystem.updateBodiesFromVideoData(std::move(m_imageData));
        m_imageData.reset();

        // Process any accumulated IMU messages so we don't get backed up.
        std::vector<MessageEntry> imuMessages;
        {
            // Copy out messages inside the mutex.
            std::lock_guard<std::mutex> lock(m_messageMutex);
            if (!m_messages.empty()) {
                auto size = m_messages.size();
                using size_type = decltype(size);
                imuMessages.reserve(size);
                for (size_type i = 0; i < size; ++i) {
                    imuMessages.emplace_back(m_messages.front());
                    m_messages.pop();
                }
            }
        } // unlock
        for (auto const &msg : imuMessages) {
            processIMUMessage(msg);
        }

        updateReportingVector(bodyIds);
    }
    class IMUMessageProcessor : public boost::static_visitor<> {
      public:
        void operator()(boost::none_t const &) const {
            /// dummy overload to handle empty messages
        }

        template <typename TimestampedReport>
        void operator()(TimestampedReport const &report) const {
            /// templated overload to handle real messages since they're
            /// identical except for the final element of the tuple.
            auto &imu = *std::get<0>(report);
            auto timestamp = std::get<1>(report);
            /// Go off to individual methods for the last argument.
            updatePose(imu, timestamp, std::get<2>(report));
        }

        static void updatePose(TrackedBodyIMU &imu,
                               util::time::TimeValue const &timestamp,
                               OSVR_OrientationReport const &ori) {
            imu.updatePoseFromOrientation(
                timestamp, util::eigen_interop::map(ori.rotation).quat());
        }

        static void updatePose(TrackedBodyIMU &imu,
                               util::time::TimeValue const &timestamp,
                               OSVR_AngularVelocityReport const &angVel) {

            imu.updatePoseFromAngularVelocity(
                timestamp,
                util::eigen_interop::map(angVel.state.incrementalRotation)
                    .quat(),
                angVel.state.dt);
        }
    };

    void TrackerThread::processIMUMessage(MessageEntry const &m) {
        boost::apply_visitor(IMUMessageProcessor{}, m);
    }

    void TrackerThread::updateReportingVector(BodyIndices const &bodyIds) {
        for (auto const &bodyId : bodyIds) {
            auto &body = m_trackingSystem.getBody(bodyId);
            m_reportingVec[bodyId.value()]->updateState(
                body.getStateTime(), body.getState(), body.getProcessModel());
        }

        // Extra reports
        if (!m_trackingSystem.haveCameraPose()) {
            // if we don't have camera pose, we can't be calibrated, so none of
            // the extra reports will have data.
            return;
        }
        auto numBodies = m_trackingSystem.getNumBodies();

        auto &cameraPoseReporting = *m_reportingVec[numBodies];
        auto &imuAlignedReporting = *m_reportingVec[numBodies + 1];
        auto &imuCameraSpaceReporting = *m_reportingVec[numBodies + 2];

        if (!m_setCameraPose) {
            m_setCameraPose = true;
            Eigen::Isometry3d trackerToRoomXform =
                m_trackingSystem.getCameraPose();
            for (auto &reporting : m_reportingVec) {
                if (reporting.get() == &cameraPoseReporting ||
                    reporting.get() == &imuAlignedReporting ||
                    reporting.get() == &imuCameraSpaceReporting) {
                    /// Skip these special ones, leave them with an identity
                    /// transform.
                    continue;
                }
                reporting->setTrackerToRoomTransform(trackerToRoomXform);
            }
        }

        using namespace Eigen;
        using namespace std::chrono;
        /// Are we due to report on the camera pose?
        if (!m_nextCameraPoseReport ||
            our_clock::now() > *m_nextCameraPoseReport) {
            m_nextCameraPoseReport = our_clock::now() + seconds(1);
            BodyState state;
            state.position() = m_trackingSystem.getCameraPose().translation();
            state.setQuaternion(
                Quaterniond(m_trackingSystem.getCameraPose().rotation()));
            cameraPoseReporting.updateState(util::time::getNow(), state);
        }

        if (m_trackingSystem.getBody(BodyId(0)).hasIMU()) {
            auto &imu = m_trackingSystem.getBody(BodyId(0)).getIMU();
            if (!imu.hasPoseEstimate()) {
                return;
            }
            Eigen::Quaterniond imuQuat = imu.getPoseEstimate();
            {
                BodyState state;
                state.setQuaternion(imuQuat);
                imuAlignedReporting.updateState(imu.getLastUpdate(), state);
            }
            {
                BodyState state;
                state.setQuaternion(getQuatToCameraSpace(m_trackingSystem) *
                                    imuQuat);
                // Put this one up in the air a little so we can tell the
                // difference.
                state.position() = Eigen::Vector3d(0, 0.5, 0);
                imuCameraSpaceReporting.updateState(imu.getLastUpdate(), state);
            }
        }
    }
    void TrackerThread::launchTimeConsumingImageStep() {
        if (m_imageThread.joinable()) {
            m_imageThread.join();
        }
        /// Our thread would be the only one reading or writing this flag at
        /// this point, so it's OK now to write this without protection.
        m_timeConsumingImageStepComplete = false;
        m_imageThread = std::thread{[&] { timeConsumingImageStep(); }};
    }
    void TrackerThread::timeConsumingImageStep() {
        /// When we return from this function, set a flag indicating we're done
        /// and notify on the condition variable.

        auto signalCompletion = util::finally([&] {
            {
                std::lock_guard<std::mutex> lock{m_messageMutex};
                m_timeConsumingImageStepComplete = true;
            }
            m_messageCondVar.notify_one();
        });

        // Pull the image into an OpenCV matrix named m_frame.
        m_cam.retrieve(m_frame, m_frameGray);
        if (!m_frame.data || !m_frameGray.data) {
            // let the tracker thread warn if it wants to, we'll just get
            // out.
            return;
        }

        // Do the slow, but intentionally async-able part of the image
        // processing.
        m_imageData = m_trackingSystem.performInitialImageProcessing(
            m_triggerTime, m_frame, m_frameGray, m_camParams);
    }
} // namespace vbtracker
} // namespace osvr
