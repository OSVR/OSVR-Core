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
#include "ImageProcessingThread.h"
#include "TrackerThread.h"
#include "TrackingSystem.h"

#include "ImageSources/ImageSource.h"

// Library/third-party includes
#include <osvr/Util/Finally.h>

// Standard includes
#include <iostream>

namespace osvr {
namespace vbtracker {
    ImageProcessingThread::ImageProcessingThread(
        TrackingSystem &trackingSystem, ImageSource &cam,
        TrackerThread &trackerThread, CameraParameters const &camParams,
        std::int32_t cameraUsecOffset)
        : trackingSystem_(trackingSystem), cam_(cam),
          trackerThreadObj_(trackerThread), camParams_(camParams),
          cameraUsecOffset_(cameraUsecOffset),
          logBlobs_(trackingSystem_.getParams().logRawBlobs) {
        if (logBlobs_) {
            blobFile_.open("blobs.csv");
            if (blobFile_) {
                blobFile_ << "sec,usec,x,y,size" << std::endl;
            } else {
                warn() << "Could not open blob file!" << std::endl;
                logBlobs_ = false;
            }
        }
    }

    void ImageProcessingThread::signalDoFrame() {
        {
            std::lock_guard<std::mutex> lock{stateMutex_};
            next_ = NextOp::DoFrame;
        }
        stateCondVar_.notify_all();
    }

    void ImageProcessingThread::signalExit() {
        {
            std::lock_guard<std::mutex> lock{stateMutex_};
            next_ = NextOp::Exit;
        }
        stateCondVar_.notify_all();
    }

    void ImageProcessingThread::threadAction() {
        while (1) {
            {
                std::unique_lock<std::mutex> lock(stateMutex_);

                if (NextOp::Waiting == next_) {
                    /// Wait until we're notified and next_ isn't just "waiting"
                    stateCondVar_.wait(
                        lock, [&] { return next_ != NextOp::Waiting; });
                }

                /// OK, we're not supposed to wait anymore. What do we do?
                if (NextOp::Exit == next_) {
                    // we are all done.
                    exiting_ = true;
                    return;
                }
                /// Otherwise, we should do a frame.
                /// Re-set the next op while we still hold the mutex.
                next_ = NextOp::Waiting;
            }
            doFrame();
        }
    }

    void ImageProcessingThread::doFrame() {
        ImageOutputDataPtr data;
        /// On scope exit, no matter how, signal to the tracker thread that
        /// we're done.
        auto signalCompletion = util::finally([&] {
            trackerThreadObj_.signalImageProcessingComplete(std::move(data),
                                                            frame_, gray_);
        });

        // Pull the image into an OpenCV matrix named m_frame.
        util::time::TimeValue frameTime;
        cam_.retrieve(frame_, gray_, frameTime);
        if (!frame_.data || !gray_.data) {
            // let the tracker thread warn if it wants to, we'll just get
            // out.
            return;
        }

        /// We retrieved a timestamp with that frame...

        /// @todo backdate to account for image transfer image, exposure
        /// time, etc.

        if (cameraUsecOffset_ != 0) {
            // apply offset, if non-zero.
            const util::time::TimeValue offset{0, cameraUsecOffset_};
            osvrTimeValueSum(&frameTime, &offset);
        }

        // Do the slow, but intentionally async-able part of the image
        // processing.
        data = trackingSystem_.performInitialImageProcessing(frameTime, frame_,
                                                             gray_, camParams_);
        // Log blobs, if applicable
        if (logBlobs_) {
            if (!blobFile_) {
                // Oh dear, the file went bad.
                logBlobs_ = false;
                return;
            }
            blobFile_ << data->tv.seconds << "," << data->tv.microseconds;
            for (auto &measurement : data->ledMeasurements) {
                blobFile_ << "," << measurement.loc.x << ","
                          << measurement.loc.y << "," << measurement.diameter;
            }
            blobFile_ << "\n";
        }

        // On return, we'll automatically notify the tracker thread that its
        // results are ready for pickup at the second window.
    }

    std::ostream &ImageProcessingThread::msg() const {
        return std::cout << "[UnifiedTracker:ImgProcThread] ";
    }

    std::ostream &ImageProcessingThread::warn() const {
        return msg() << "Warning: ";
    }

} // namespace vbtracker
} // namespace osvr
