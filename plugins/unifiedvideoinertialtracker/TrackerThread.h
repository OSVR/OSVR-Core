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

#ifndef INCLUDED_TrackerThread_h_GUID_6544B03C_4EB4_4B82_77F1_16EF83578C64
#define INCLUDED_TrackerThread_h_GUID_6544B03C_4EB4_4B82_77F1_16EF83578C64

// Internal Includes
#include "TrackingSystem.h"
#include "ThreadsafeBodyReporting.h"
#include "CameraParameters.h"

#include "ImageSources/ImageSource.h"

// Library/third-party includes
#include <osvr/Util/ClientReportTypesC.h>

#include <opencv2/core/core.hpp> // for basic OpenCV types

#include <boost/noncopyable.hpp>
#include <boost/variant.hpp>

// Standard includes
#include <iosfwd>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <tuple>
#include <chrono>
#include <future>

namespace osvr {
namespace vbtracker {
    class TrackedBodyIMU;

    using TimestampedOrientation =
        std::tuple<TrackedBodyIMU *, util::time::TimeValue,
                   OSVR_OrientationReport>;
    using TimestampedAngVel =
        std::tuple<TrackedBodyIMU *, util::time::TimeValue,
                   OSVR_AngularVelocityReport>;
    using MessageEntry = boost::variant<boost::none_t, TimestampedOrientation,
                                        TimestampedAngVel>;

    class TrackerThread : boost::noncopyable {
      public:
        TrackerThread(TrackingSystem &trackingSystem, ImageSource &imageSource,
                      BodyReportingVector &reportingVec,
                      CameraParameters const &camParams);
        ~TrackerThread();
        /// Thread function-call operator: should be invoked by a lambda in a
        /// dedicated thread.
        void threadAction();

        /// @name Main thread methods
        /// @{
        /// The thread starts and immediately blocks. Calling this allows it to
        /// proceed with execution.
        void permitStart();

        /// Call from the main thread to trigger this thread's execution to exit
        /// after the current frame.
        void triggerStop();

        /// Submit an orientation report for an IMU
        void submitIMUReport(TrackedBodyIMU &imu,
                             util::time::TimeValue const &tv,
                             OSVR_OrientationReport const &report);
        void submitIMUReport(TrackedBodyIMU &imu,
                             util::time::TimeValue const &tv,
                             OSVR_AngularVelocityReport const &report);
        /// @}

      private:
        /// Helper providing a prefixed output stream for normal messages.
        std::ostream &msg() const;
        /// Helper providing a prefixed output stream for warning messages.
        std::ostream &warn() const;

        /// Main function called repeatedly, once for each (attempted) frame of
        /// video.
        void doFrame();

        /// Copy updated body state into the reporting vector.
        void updateReportingVector(BodyIndices const &bodyIds);

        /// This function is responsible for launching the descriptively-named
        /// timeConsumingImageStep() asynchronously.
        void launchTimeConsumingImageStep();

        /// The "time consuming image step" - specifically, retrieving the image
        /// and performing the initial blob detection on it. This gets launched
        /// asynchronously by launchTimeConsumingImageStep()
        void timeConsumingImageStep();

        void processIMUMessage(MessageEntry const &m);

        TrackingSystem &m_trackingSystem;
        ImageSource &m_cam;
        BodyReportingVector &m_reportingVec;
        CameraParameters m_camParams;

        using our_clock = std::chrono::steady_clock;
        boost::optional<our_clock::time_point> m_nextCameraPoseReport;

        /// Time that the last camera grab was triggered.
        util::time::TimeValue m_triggerTime;

        /// a void promise, as suggested by Scott Meyers, to hold the thread
        /// operation at the beginning until we want it to really start running.
        std::promise<void> m_startupSignal;

        bool m_setCameraPose = false;

        /// @name Updated asynchronously by timeConsumingImageStep()
        /// @{
        cv::Mat m_frame;
        cv::Mat m_frameGray;
        ImageOutputDataPtr m_imageData;
        /// @}

        /// @name Run flag
        /// @{
        std::mutex m_runMutex;
        bool m_run = true;
        /// @}

        /// @name Message queue for async image processing and receiving IMU
        /// reports from other threads.
        /// @{
        std::condition_variable m_messageCondVar;
        std::mutex m_messageMutex;
        std::queue<MessageEntry> m_messages;
        bool m_timeConsumingImageStepComplete = false;
        /// @}

        /// The thread used by timeConsumingImageStep()
        std::thread m_imageThread;
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_TrackerThread_h_GUID_6544B03C_4EB4_4B82_77F1_16EF83578C64
