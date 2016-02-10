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
#include <deque>
#include <thread>
#include <mutex>
#include <future>

namespace osvr {
namespace vbtracker {

    using TimestampedOrientation =
        std::pair<util::time::TimeValue, OSVR_OrientationReport>;
    using TimestampedAngVel =
        std::pair<util::time::TimeValue, OSVR_AngularVelocityReport>;
    using MessageEntry =
        boost::variant<TimestampedOrientation, TimestampedAngVel>;

    class TrackerThread : boost::noncopyable {
      public:
        TrackerThread(TrackingSystem &trackingSystem, ImageSource &imageSource,
                      BodyReportingVector &reportingVec,
                      CameraParameters const &camParams);
        /// Thread function-call operator: should be invoked by a lambda in a
        /// dedicated thread.
        void operator()();

        /// Call from the main thread!
        void triggerStop();

      private:
        /// Helper providing a prefixed output stream for normal messages.
        std::ostream &msg() const;
        /// Helper providing a prefixed output stream for warning messages.
        std::ostream &warn() const;
        void doFrame();
        TrackingSystem &m_trackingSystem;
        ImageSource &m_cam;
        BodyReportingVector &m_reportingVec;
        CameraParameters m_camParams;
        cv::Mat m_frame;
        cv::Mat m_frameGray;
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
        std::deque<MessageEntry> m_messages;
        ImageOutputDataPtr m_imageData;
        /// @}
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_TrackerThread_h_GUID_6544B03C_4EB4_4B82_77F1_16EF83578C64
