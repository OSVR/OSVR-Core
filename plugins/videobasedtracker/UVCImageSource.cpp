/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com>

*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_UVCImageSource_cpp_GUID_2563F019_11B6_4F61_9E8C_C3ED2A573AF6
#define INCLUDED_UVCImageSource_cpp_GUID_2563F019_11B6_4F61_9E8C_C3ED2A573AF6

// Internal Includes
#include "ImageSource.h"
#include "ImageSourceFactories.h"

// Library/third-party includes
#include <libuvc/libuvc.h>
#include <opencv2/core/core_c.h>

// Standard includes
#include <condition_variable>
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <unistd.h>

namespace osvr {
namespace vbtracker {
    class UVCImageSource : public ImageSource {
      public:
        /// Constructor
        UVCImageSource(int vendor_id = 0, int product_id = 0,
                       const char *serial_number = nullptr);

        /// Destructor
        virtual ~UVCImageSource();

        /// @return true if the camera/image source is OK
        virtual bool ok() const override;

        /// Trigger camera capture. May not necessarily include retrieval.
        /// Blocks until an image is available. or failure occurs.
        ///
        /// Timestamp after this call returns.
        ///
        /// @return false if the camera failed.
        virtual bool grab() override;

        /// Get resolution of the images from this source.
        virtual cv::Size resolution() const override;

        /// For those devices that naturally read a non-corrupt color image,
        /// overriding just this method will let the default implementation of
        /// retrieve() do the RGB to Gray for you.
        virtual void retrieveColor(cv::Mat &color) override;

      protected:
        /// This callback function is called each time a new frame is received
        /// from the video stream.
        //@{
        static void callback(uvc_frame_t *frame, void *ptr);
        void callback(uvc_frame_t *frame);
        //@}

      private:
        typedef std::unique_ptr<uvc_frame_t, decltype(&uvc_free_frame)>
            Frame_ptr;
        typedef std::unique_ptr<uvc_context_t,
                                std::function<void(uvc_context_t *)>>
            Context_ptr;
        typedef std::unique_ptr<uvc_device_t,
                                std::function<void(uvc_device_t *)>>
            Device_ptr;
        typedef std::unique_ptr<uvc_device_handle_t,
                                std::function<void(uvc_device_handle_t *)>>
            Device_handle_ptr;

        Context_ptr uvcContext_;
        Device_ptr camera_;
        Device_handle_ptr cameraHandle_;
        uvc_stream_ctrl_t streamControl_;

        cv::Size resolution_;          //< resolution of camera
        std::queue<Frame_ptr> frames_; //< raw UVC frames

        std::mutex mutex_;                         //< to protect frames_
        std::condition_variable frames_available_; //< To allo grab() to wait
                                                   // for frames to become
        // available
    };

    using UVCImageSourcePtr = std::unique_ptr<UVCImageSource>;

    UVCImageSource::UVCImageSource(int vendor_id, int product_id,
                                   const char *serial_number)
        : streamControl_{}, resolution_{0, 0} {

        { // Initialize the libuvc context
            uvc_context_t *context;
            const auto init_res = uvc_init(&context, nullptr);
            if (UVC_SUCCESS != init_res) {
                throw std::runtime_error("Error initializing UVC context: " +
                                         std::string(uvc_strerror(init_res)));
            }
            uvcContext_ = Context_ptr(context, &uvc_exit);
        }

        { // Find the requested camera
            uvc_device_t *device;
            const auto find_res =
                uvc_find_device(uvcContext_.get(), &device, vendor_id,
                                product_id, serial_number);
            if (UVC_SUCCESS != find_res) {
                throw std::runtime_error("Error finding requested camera: " +
                                         std::string(uvc_strerror(find_res)));
            }
            camera_ = Device_ptr(device, uvc_unref_device);
        }

        { // Try to open the device -- requires exclusive access
            uvc_device_handle_t *device_handle;
            const auto open_res = uvc_open(camera_.get(), &device_handle);
            if (UVC_SUCCESS != open_res) {
                throw std::runtime_error("Error opening camera: " +
                                         std::string(uvc_strerror(open_res)));
            }
            cameraHandle_ = Device_handle_ptr(device_handle, uvc_close);
            // uvc_print_diag(cameraHandle_, stdout);
        }

        // Setup streaming parameters
        const int resolution_x = 640; // pixels
        const int resolution_y = 480; // pixels
        resolution_ = cvSize(resolution_x, resolution_y);
        const int frame_rate = 100; // fps
        const auto setup_res = uvc_get_stream_ctrl_format_size(
            cameraHandle_.get(), &streamControl_, UVC_FRAME_FORMAT_MJPEG,
            resolution_x, resolution_y, frame_rate);
        // uvc_print_stream_ctrl(&streamControl_, stdout);
        if (UVC_SUCCESS != setup_res) {
            std::cerr << "Error setting up requested stream format. " +
                             std::string(uvc_strerror(setup_res));
        }

        // Start streaming video.
        const auto stream_res =
            uvc_start_streaming(cameraHandle_.get(), &streamControl_,
                                &UVCImageSource::callback, this, 0);
        if (UVC_SUCCESS != stream_res) {
            throw std::runtime_error("Error streaming from camera: " +
                                     std::string(uvc_strerror(stream_res)));
        }
    }

    UVCImageSource::~UVCImageSource() {
        // Stop streaming video (if required)
        if (cameraHandle_) {
            uvc_stop_streaming(cameraHandle_.get());
        }
    }

    bool UVCImageSource::ok() const {
        // TODO
        return true;
    }

    bool UVCImageSource::grab() {
        // Gain access to frames_ queue
        std::unique_lock<std::mutex> lock(mutex_);

        // Wait until there are any frames available
        frames_available_.wait(lock, [this]() { return !frames_.empty(); });

        // Good to go!
        return !frames_.empty();
    }

    cv::Size UVCImageSource::resolution() const { return resolution_; }

    void UVCImageSource::retrieveColor(cv::Mat &color) {
        // Grab a frame from the queue, but don't keep the queue locked!
        std::unique_lock<std::mutex> lock(mutex_);
        if (frames_.empty()) {
            throw std::runtime_error("Error: There's no frames available.");
        }
        Frame_ptr current_frame(frames_.front().release(), &uvc_free_frame);
        frames_.pop();
        lock.unlock();

        // We'll convert the image from YUV/JPEG to rgb, so allocate space
        Frame_ptr rgb(uvc_allocate_frame(current_frame->width *
                                         current_frame->height * 3),
                      &uvc_free_frame);

        if (!rgb) {
            throw std::runtime_error(
                "Error: Unable to allocate the rgb frame.");
        }

        // Do the rgb conversion
        auto convert_ret = uvc_mjpeg2rgb(current_frame.get(), rgb.get());
        if (UVC_SUCCESS != convert_ret) {
            // Try any2rgb() instead
            auto any_ret = uvc_any2rgb(current_frame.get(), rgb.get());
            if (UVC_SUCCESS != any_ret) {
                throw std::runtime_error(
                    "Error: Unable to convert frame to rgb: " +
                    std::string(uvc_strerror(convert_ret)));
            }
        }

        // Convert the image to at cv::Mat
        color = cv::Mat(rgb->height, rgb->width, CV_8UC3, rgb->data).clone();
    }

    void UVCImageSource::callback(uvc_frame_t *frame, void *ptr) {
        auto me = static_cast<UVCImageSource *>(ptr);
        me->callback(frame);
    }

    void UVCImageSource::callback(uvc_frame_t *frame) {
        // Just copy the UVC frame to keep things quick (cannot drop
        // frames or we'll lose tracking!). We'll do any conversion
        // upon request in the retrieveColor() method.

        // Copy the frame, then look for the lock
        Frame_ptr copied_frame(uvc_allocate_frame(frame->data_bytes),
                               &uvc_free_frame);
        uvc_duplicate_frame(frame, copied_frame.get());

        std::lock_guard<std::mutex> lock(mutex_);
        frames_.emplace(copied_frame.release(), &uvc_free_frame);
        if (frames_.size() > 100) {
            std::cerr << "WARNING! Dropping frames from video tracker as they "
                         "are not being processed fast enough! This will "
                         "disrupt tracking."
                      << std::endl;
            frames_ = std::queue<Frame_ptr>(); //< clear the queue
        }

        frames_available_.notify_one();
    }

    /// Factory method to open a USB video class (UVC) device as an image
    /// source.
    ImageSourcePtr openUVCCamera(int vendor_id, int product_id,
                                 const char *serial_number) {
        auto ret = ImageSourcePtr{};
        try {
            auto source =
                new UVCImageSource(vendor_id, product_id, serial_number);
            ret.reset(source);
        } catch (const std::exception &e) {
            std::cerr
                << "Caught exception initializing UVC camera image source: "
                << e.what() << std::endl;
        }
        return ret;
    }

    /// Factory method to open the HDK camera as an image source via libuvc.
    ImageSourcePtr openHDKCameraUVC() {
        const int vendor_id = 0x0bda;
        const int product_id = 0x57e8;
        return openUVCCamera(vendor_id, product_id);
    }

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_UVCImageSource_cpp_GUID_2563F019_11B6_4F61_9E8C_C3ED2A573AF6
