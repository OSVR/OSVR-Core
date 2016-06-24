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
#include "CameraParameters.h"
#include "ThreadsafeBodyReporting.h"
#include "TrackingSystem.h"
#include "IMUMessage.h"

#include "ImageSources/ImageSource.h"

// Library/third-party includes
#include <osvr/Util/ClientReportTypesC.h>

#include <opencv2/core/core.hpp> // for basic OpenCV types

#include <boost/noncopyable.hpp>
#include <folly/ProducerConsumerQueue.h>
#include <folly/sorted_vector_types.h>
#include <osvr/TypePack/List.h>

// Standard includes
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <future>
#include <iosfwd>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>

namespace osvr {
namespace vbtracker {
    static const std::chrono::milliseconds IMU_OVERRIDE_SPACING{5};

    struct BodyIdOrdering {
        bool operator()(BodyId const &lhs, BodyId const &rhs) const {
            return lhs.value() < rhs.value();
        }
    };

    using UpdatedBodyIndices = folly::sorted_vector_set<BodyId, BodyIdOrdering>;

    class TrackerThread : boost::noncopyable {
      public:
        TrackerThread(TrackingSystem &trackingSystem, ImageSource &imageSource,
                      BodyReportingVector &reportingVec,
                      CameraParameters const &camParams,
                      std::int32_t cameraUsecOffset = 0);
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
        /// @return false if there is no room in the queue for the message
        bool submitIMUReport(TrackedBodyIMU &imu,
                             util::time::TimeValue const &tv,
                             OSVR_OrientationReport const &report);
        /// @overload
        /// for angular velocity
        bool submitIMUReport(TrackedBodyIMU &imu,
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

        /// Can call as soon as the loop starts (as soon as m_numBodies is
        /// known)
        void setupReportingVectorProcessModels();

        /// Should call only once room calibration is completed.
        bool setupReportingVectorRoomTransforms();

        /// Copy updated body state into the reporting vector.
        void updateReportingVector(UpdatedBodyIndices const &bodyIds);

        /// This function is responsible for launching the descriptively-named
        /// timeConsumingImageStep() asynchronously.
        void launchTimeConsumingImageStep();

        /// The "time consuming image step" - specifically, retrieving the image
        /// and performing the initial blob detection on it. This gets launched
        /// asynchronously by launchTimeConsumingImageStep()
        void timeConsumingImageStep();

        BodyId processIMUMessage(IMUMessage const &m);
        BodyReporting *getCamPoseReporting() const;
        BodyReporting *getIMUReporting() const;
        BodyReporting *getIMUCamReporting() const;
        BodyReporting *getHMDCamReporting() const;
        void updateExtraCameraReport();
        void updateExtraIMUReports();

        TrackingSystem &m_trackingSystem;
        ImageSource &m_cam;
        BodyReportingVector &m_reportingVec;
        CameraParameters m_camParams;
        std::size_t m_numBodies = 0; //< initialized when loop started.
        const std::int32_t m_cameraUsecOffset = 0;

        using our_clock = std::chrono::steady_clock;

        bool shouldSendImuReport() {
            auto now = our_clock::now();
            if (now > m_nextImuOverrideReport) {
                m_nextImuOverrideReport = now + IMU_OVERRIDE_SPACING;
                return true;
            }
            return false;
        }

        void setImuOverrideClock() {
            m_nextImuOverrideReport = our_clock::now() + IMU_OVERRIDE_SPACING;
        }

        our_clock::time_point m_nextImuOverrideReport;
        boost::optional<our_clock::time_point> m_nextCameraPoseReport;

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
        bool m_timeConsumingImageStepComplete = false;
        folly::ProducerConsumerQueue<IMUMessage> m_imuMessages;
        /// @}

        /// Output file we stream data on the blobs to.
        bool m_logBlobs = false;
        std::ofstream m_blobFile;

        /// The thread used by timeConsumingImageStep()
        std::thread m_imageThread;
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_TrackerThread_h_GUID_6544B03C_4EB4_4B82_77F1_16EF83578C64
