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

#ifndef INCLUDED_ImageProcessingThread_h_GUID_307E6652_D346_43B4_291A_5BAAEF4BA909
#define INCLUDED_ImageProcessingThread_h_GUID_307E6652_D346_43B4_291A_5BAAEF4BA909

// Internal Includes
#include <CameraParameters.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstdint>
#include <fstream>
#include <iosfwd>
#include <mutex>

namespace osvr {
namespace vbtracker {
    class TrackerThread;
    class TrackingSystem;
    class ImageSource;

    class ImageProcessingThread {
      public:
        explicit ImageProcessingThread(TrackingSystem &trackingSystem,
                                       ImageSource &cam,
                                       TrackerThread &trackerThread,
                                       CameraParameters const &camParams,
                                       std::int32_t cameraUsecOffset);

        /// non-assignable.
        ImageProcessingThread &operator=(ImageProcessingThread &) = delete;

        /// called by TrackerThread
        void signalDoFrame();
        /// called by TrackerThread
        void signalExit();

        /// Entry point for the thread dedicated to this object.
        void threadAction();

        /// Did we get the exit message?
        bool exiting() const { return exiting_; }

      private:
        /// Helper providing a prefixed output stream for normal messages.
        std::ostream &msg() const;
        /// Helper providing a prefixed output stream for warning messages.
        std::ostream &warn() const;
        /// Performs the retrieval and processing of a single frame.
        void doFrame();

        TrackingSystem &trackingSystem_;
        ImageSource &cam_;
        TrackerThread &trackerThreadObj_;
        const CameraParameters camParams_;
        const std::int32_t cameraUsecOffset_;

        /// Output file we stream data on the blobs to.
        bool logBlobs_ = false;
        std::ofstream blobFile_;

        enum class NextOp { Waiting, DoFrame, Exit };

        std::mutex stateMutex_;
        std::condition_variable stateCondVar_;
        NextOp next_ = NextOp::Waiting;

        bool exiting_ = false;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_ImageProcessingThread_h_GUID_307E6652_D346_43B4_291A_5BAAEF4BA909
