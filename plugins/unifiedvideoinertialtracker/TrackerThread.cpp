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
#include "AdditionalReports.h"
#include "ImageProcessingThread.h"
#include "ProcessIMUMessage.h"
#include "SpaceTransformations.h"
#include "TrackedBody.h"
#include "TrackedBodyIMU.h"

// Library/third-party includes
#include <osvr/Util/EigenInterop.h>
#include <osvr/Util/Finally.h>

// Standard includes
#include <future>
#include <iostream>
#include <type_traits>

#define OSVR_TRACKER_THREAD_WRAP_WITH_TRY

namespace osvr {
namespace vbtracker {
    // 16 and even 32 was too small - we were dropping messages.
    static const uint32_t IMU_MESSAGE_QUEUE_SIZE = 64 + 1;

    TrackerThread::TrackerThread(TrackingSystem &trackingSystem,
                                 ImageSource &imageSource,
                                 BodyReportingVector &reportingVec,
                                 CameraParameters const &camParams,
                                 std::int32_t cameraUsecOffset)
        : m_trackingSystem(trackingSystem), m_cam(imageSource),
          m_reportingVec(reportingVec), m_camParams(camParams),
          m_cameraUsecOffset(cameraUsecOffset),
          m_imuMessages(IMU_MESSAGE_QUEUE_SIZE),
          m_logBlobs(m_trackingSystem.getParams().logRawBlobs) {
        msg() << "Tracker thread object created." << std::endl;

        if (m_logBlobs) {
            m_blobFile.open("blobs.csv");
            if (m_blobFile) {
                m_blobFile << "sec,usec,x,y,size" << std::endl;
            } else {
                warn() << "Could not open blob file!" << std::endl;
                m_logBlobs = false;
            }
        }
    }

    TrackerThread::~TrackerThread() {
        if (m_imageThread.joinable()) {
            m_imageThread.join();
        }
    }

    void TrackerThread::permitStart() { m_startupSignal.set_value(); }

    void TrackerThread::threadAction() {
        /// The thread internally is organized around processing video frames,
        /// with arrival of IMU reports internally handled as they come. Thus,
        /// we keep getting frames and processing them until we're told to stop,
        /// doing what we can asynchronously to also process incoming IMU
        /// messages.

        msg() << "Tracker thread object invoked, waiting for permitStart()."
              << std::endl;
        m_startupSignal.get_future().wait();
        /// sleep an extra half a second to give everyone else time to get off
        /// the starting blocks.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_numBodies = m_trackingSystem.getNumBodies();
        setupReportingVectorProcessModels();
        msg() << "Tracker thread object entering its main execution loop."
              << std::endl;

#ifdef OSVR_TRACKER_THREAD_WRAP_WITH_TRY
        try {
#endif
            bool keepGoing = true;
            while (keepGoing) {
                /// Call the doFrame() method to perform one video frame's worth
                /// of processing.
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

    bool TrackerThread::submitIMUReport(TrackedBodyIMU &imu,
                                        util::time::TimeValue const &tv,
                                        OSVR_OrientationReport const &report) {
        /// Main thread method!
        if (!m_imuMessages.write(makeImuReport(imu, tv, report))) {
            // no room for IMU message!
            msg() << "Dropped IMU orientation message!\n";
            return false;
        }
        m_messageCondVar.notify_one();
        return true;
    }

    bool
    TrackerThread::submitIMUReport(TrackedBodyIMU &imu,
                                   util::time::TimeValue const &tv,
                                   OSVR_AngularVelocityReport const &report) {
        /// Main thread method!
        if (!m_imuMessages.write(makeImuReport(imu, tv, report))) {
            // no room for IMU message!
            return false;
        }
        m_messageCondVar.notify_one();
        return true;
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
        // When we triggered the grab was a good guess of the time
        // for the image before that got moved upstream into the ImageSource
        // library.

        /// Launch an asynchronous task to perform the image retrieval and
        /// initial image processing.
        launchTimeConsumingImageStep();

        setImuOverrideClock();
        UpdatedBodyIndices imuIndices;

        bool finishedImage = false;
        do {

            {
                /// Wait for something to do (Completion of image, IMU reports)
                std::unique_lock<std::mutex> lock(m_messageMutex);
                m_messageCondVar.wait(lock, [&] {
                    return m_timeConsumingImageStepComplete ||
                           !m_imuMessages.isEmpty();
                });
                if (m_timeConsumingImageStepComplete) {
                    /// Set a flag to get us out of this innermost loop - we'll
                    /// finish up processing this frame and trigger another grab
                    /// before we look at more IMU data.
                    finishedImage = true;
                }
                // Otherwise we have some IMU reports to keep us busy in the
                // meantime.
            } // unlock

            if (!finishedImage) {
                /// This means we got out of waiting on the condition variable
                /// because of an IMU message. Handle one.

                IMUMessage message = boost::none;
                if (!m_imuMessages.read(message) || message.empty()) {
                    // couldn't read a message, or read an empty message
                    continue;
                }

                // process it.
                auto id = processIMUMessage(message);
                if (id.empty()) {
                    // processed but got an empty body ID
                    continue;
                }

                // insert index into the list
                imuIndices.insert(id);

                // if it's time, send a report even if we haven't gotten a video
                // frame with useful things in it yet.
                if (shouldSendImuReport()) {
                    updateReportingVector(imuIndices);
                    imuIndices.clear();
                }
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

        // Sort those body IDs so we can merge them with the body IDs from any
        // IMU messages we're about to process.
        UpdatedBodyIndices sortedBodyIds{begin(bodyIds), end(bodyIds)};
        for (auto &id : imuIndices) {
            sortedBodyIds.insert(id);
        }
        imuIndices.clear();

        // process those IMU messages and add any unique IDs to the vector
        // returned by the image tracker.
        // We only want to process a fixed number of messages so we don't get
        // stuck here in a loop without servicing the camera.
        std::size_t numMessages = m_imuMessages.sizeGuess();
        {
            IMUMessage message = boost::none;
            for (std::size_t i = 0; i < numMessages; ++i) {
                if (!m_imuMessages.read(message)) {
                    // ran out of messages earlier than expected.
                    break;
                }
                auto id = processIMUMessage(message);
                if (!id.empty()) {
                    sortedBodyIds.insert(id);
                }
            }
        }

        updateReportingVector(sortedBodyIds);
    }

    BodyId TrackerThread::processIMUMessage(IMUMessage const &m) {
        return osvr::vbtracker::processImuMessage(m);
    }

    BodyReporting *TrackerThread::getCamPoseReporting() const {
#ifdef OSVR_OUTPUT_CAMERA_POSE
        return m_reportingVec[m_numBodies + extra_outputs::outputCamIndex]
            .get();
#else
            return nullptr;
#endif
    }

    BodyReporting *TrackerThread::getIMUReporting() const {
#ifdef OSVR_OUTPUT_IMU
        return m_reportingVec[m_numBodies + extra_outputs::outputImuIndex]
            .get();
#else
            return nullptr;
#endif
    }

    BodyReporting *TrackerThread::getIMUCamReporting() const {
#ifdef OSVR_OUTPUT_IMU_CAM
        return m_reportingVec[m_numBodies + extra_outputs::outputImuCamIndex]
            .get();
#else
            return nullptr;
#endif
    }

    BodyReporting *TrackerThread::getHMDCamReporting() const {

#ifdef OSVR_OUTPUT_HMD_CAM
        return m_reportingVec[m_numBodies + extra_outputs::outputHMDCamIndex]
            .get();
#else
            return nullptr;
#endif
    }

    void TrackerThread::updateExtraCameraReport() {
#ifndef OSVR_OUTPUT_CAMERA_POSE
        return;
#endif

        using namespace Eigen;
        using namespace std::chrono;
        /// Are we due to report on the camera pose?
        auto rightNow = our_clock::now();
        if (!m_nextCameraPoseReport || rightNow > *m_nextCameraPoseReport) {
            m_nextCameraPoseReport = rightNow + seconds(1);
            BodyState state;
            state.position() = m_trackingSystem.getCameraPose().translation();
            state.setQuaternion(
                Quaterniond(m_trackingSystem.getCameraPose().rotation()));
            getCamPoseReporting()->updateState(util::time::getNow(), state);
        }
    }

    void TrackerThread::updateExtraIMUReports() {

#if !defined(OSVR_OUTPUT_IMU) && !defined(OSVR_OUTPUT_IMU_CAM)
        // Not reporting these special reports, do nothing.
        return;
#endif

        if (!m_trackingSystem.getBody(BodyId(0)).hasIMU()) {
            // no IMU, nothing to report.
            return;
        }
        auto &imu = m_trackingSystem.getBody(BodyId(0)).getIMU();
        if (!imu.hasPoseEstimate()) {
            return;
        }
        Eigen::Quaterniond imuQuat = imu.getPoseEstimate();

#ifdef OSVR_OUTPUT_IMU
        {
            BodyState state;
            state.setQuaternion(imuQuat);
            getIMUReporting()->updateState(imu.getLastUpdate(), state);
        }
#endif

#ifdef OSVR_OUTPUT_IMU_CAM

        {
            BodyState state;
            state.setQuaternion(getQuatToCameraSpace(m_trackingSystem) *
                                imuQuat);
            // Put this one up in the air a little so we can tell the
            // difference.
            state.position() = Eigen::Vector3d(0, 0.5, 0);
            getIMUCamReporting()->updateState(imu.getLastUpdate(), state);
        }
#endif
    }

    void TrackerThread::setupReportingVectorProcessModels() {
        for (BodyId::wrapped_type i = 0; i < m_numBodies; ++i) {
            auto bodyId = BodyId{i};
            auto &body = m_trackingSystem.getBody(bodyId);
            m_reportingVec[i]->initProcessModel(body.getProcessModel());
#ifdef OSVR_OUTPUT_HMD_CAM
            if (bodyId == BodyId(0)) {
                getCamPoseReporting()->initProcessModel(body.getProcessModel());
            }
#endif
        }
    }

    bool TrackerThread::setupReportingVectorRoomTransforms() {
        if (!m_trackingSystem.haveCameraPose()) {
            /// can't do this if we haven't got the transform yet...
            return false;
        }
        if (m_setCameraPose) {
            /// It's already been done.
            return true;
        }

        /// Just set the room transform once.
        m_setCameraPose = true;

        /// These functions all return pointers.
        /// If any of these reports are not enabled, they'll return nullptr.
        auto cameraPoseReporting = getCamPoseReporting();
        auto imuAlignedReporting = getIMUReporting();
        auto imuCameraSpaceReporting = getIMUCamReporting();
        auto hmdCameraSpaceReporting = getHMDCamReporting();
        Eigen::Isometry3d trackerToRoomXform = m_trackingSystem.getCameraPose();
        for (auto &reporting : m_reportingVec) {

            /// Skip these special ones, leave them with an identity
            /// transform as their "camera to room" transform
            auto reportingRaw = reporting.get();
            if (reportingRaw == cameraPoseReporting ||
                reportingRaw == imuAlignedReporting ||
                reportingRaw == imuCameraSpaceReporting ||
                reportingRaw == hmdCameraSpaceReporting) {
                continue;
            }

            /// All other tracker reports, set the tracker to room
            /// transform.
            reporting->setTrackerToRoomTransform(trackerToRoomXform);
        }
        return true;
    }

    void
    TrackerThread::updateReportingVector(UpdatedBodyIndices const &bodyIds) {
        if (!setupReportingVectorRoomTransforms()) {
            // false return means that we don't have calibration data yet, so no
            // sense in reporting the other things.
            return;
        }
        for (auto const &bodyId : bodyIds) {
            auto &body = m_trackingSystem.getBody(bodyId);
            m_reportingVec[bodyId.value()]->updateState(body.getStateTime(),
                                                        body.getState());
#ifdef OSVR_OUTPUT_HMD_CAM
            if (bodyId == BodyId(0)) {
                getCamPoseReporting()->updateState(body.getStateTime(),
                                                   body.getState());
            }
#endif
        }

        /// Update the extra reports (if applicable)
        updateExtraCameraReport();
        updateExtraIMUReports();
    }

    void TrackerThread::launchTimeConsumingImageStep() {
        if (m_imageThread.joinable()) {
            m_imageThread.join();
        }
        /// Our thread would be the only one reading or writing this flag at
        /// this point, so it's OK now to write this without protection.
        m_timeConsumingImageStepComplete = false;
        /// @todo How to re-use this thread instead of launching a new one each
        /// frame?
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
        util::time::TimeValue frameTime;
        m_cam.retrieve(m_frame, m_frameGray, frameTime);
        if (!m_frame.data || !m_frameGray.data) {
            // let the tracker thread warn if it wants to, we'll just get
            // out.
            return;
        }

        /// We retrieved a timestamp with that frame...

        /// @todo backdate to account for image transfer image, exposure
        /// time, etc.

        if (m_cameraUsecOffset != 0) {
            // apply offset, if non-zero.
            const util::time::TimeValue offset{0, m_cameraUsecOffset};
            osvrTimeValueSum(&frameTime, &offset);
        }

        // Do the slow, but intentionally async-able part of the image
        // processing.
        m_imageData = m_trackingSystem.performInitialImageProcessing(
            frameTime, m_frame, m_frameGray, m_camParams);

        if (m_logBlobs) {
            if (!m_blobFile) {
                // Oh dear, the file went bad.
                m_logBlobs = false;
                return;
            }
            m_blobFile << m_imageData->tv.seconds << ","
                       << m_imageData->tv.microseconds;
            for (auto &measurement : m_imageData->ledMeasurements) {
                m_blobFile << "," << measurement.loc.x << ","
                           << measurement.loc.y << "," << measurement.diameter;
            }
            m_blobFile << "\n";
        }
    }
} // namespace vbtracker
} // namespace osvr
