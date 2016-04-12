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
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <mutex>

namespace osvr {
namespace vbtracker {
    class UVCImageSource : public ImageSource {
      public:
        /// Constructor
        UVCImageSource(int vendor_id = 0, int product_id = 0, const char* serial_number = nullptr);

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
        uvc_context_t* uvcContext_;
        uvc_device_t* camera_;
        uvc_device_handle_t* cameraHandle_;
        uvc_stream_ctrl_t streamControl_;

        cv::Size resolution_;       //< resolution of camera
        uvc_frame_t* frame_;        //< raw UVC frame

        std::mutex mutex_;          //< to protect frame_
    };

    using UVCImageSourcePtr = std::unique_ptr<UVCImageSource>;

    UVCImageSource::UVCImageSource(int vendor_id, int product_id, const char* serial_number) :
        uvcContext_{nullptr}, camera_{nullptr}, cameraHandle_{nullptr}, streamControl_{}, resolution_{0, 0}, frame_{nullptr}
    {
        // TODO

        // Initialize the libuvc context
        const auto init_res = uvc_init(&uvcContext_, nullptr);
        if (UVC_SUCCESS != init_res) {
            throw std::runtime_error("Error initializing UVC context: " + std::string(uvc_strerror(init_res)));
        }

        // Find the requested camera
        const auto find_res = uvc_find_device(uvcContext_, &camera_, vendor_id, product_id, serial_number);
        if (UVC_SUCCESS != find_res) {
            uvc_exit(uvcContext_);
            throw std::runtime_error("Error finding requested camera: " + std::string(uvc_strerror(find_res)));
        }

        // Try to open the device -- requires exclusive access
        const auto open_res = uvc_open(camera_, &cameraHandle_);
        if (UVC_SUCCESS != open_res) {
            uvc_unref_device(camera_);
            uvc_exit(uvcContext_);
            throw std::runtime_error("Error opening camera: " + std::string(uvc_strerror(open_res)));
        }
        uvc_print_diag(cameraHandle_, stdout);

        // Setup streaming parameters
        const int resolution_x = 640; // pixels
        const int resolution_y = 480; // pixels
        resolution_ = cvSize(resolution_x, resolution_y);
        const int frame_rate = 100; // fps
        const auto setup_res = uvc_get_stream_ctrl_format_size(cameraHandle_,
                                                               &streamControl_,
                                                               UVC_FRAME_FORMAT_MJPEG,
                                                               resolution_x,
                                                               resolution_y,
                                                               frame_rate);
        uvc_print_stream_ctrl(&streamControl_, stdout);
        if (UVC_SUCCESS != setup_res) {
            std::cerr << "Error setting up requested stream format. "
                + std::string(uvc_strerror(setup_res));
        }

        // Start streaming video.
        const auto stream_res = uvc_start_streaming(cameraHandle_, &streamControl_, &UVCImageSource::callback, this, 0);
        if (UVC_SUCCESS != stream_res) {
            uvc_close(cameraHandle_);
            uvc_unref_device(camera_);
            uvc_exit(uvcContext_);
            throw std::runtime_error("Error streaming from camera: " + std::string(uvc_strerror(stream_res)));
        }

    }

    UVCImageSource::~UVCImageSource()
    {
        // Stop streaming video
        uvc_stop_streaming(cameraHandle_);

        // Release our handle on the camera
        uvc_close(cameraHandle_);

        // Release the device descriptor
        uvc_unref_device(camera_);

        // Close the UVC context. This closes and cleans up any existing device
        // handles, and it closes the libusb context if one was not provided
        uvc_exit(uvcContext_);
    }

    bool UVCImageSource::ok() const
    {
        // TODO
        return true;
    }

    bool UVCImageSource::grab()
    {
        // libuvc doesn't provide for separate grab() and retrieve() operations.
        // Instead, grab() will set a flag to alert the callback function that
        // we wish to store the next frame and retrieve() will return that
        // frame.
        std::lock_guard<std::mutex> guard(mutex_);
        return (frame_ != nullptr);
    }

    cv::Size UVCImageSource::resolution() const
    {
        return resolution_;
    }

    void UVCImageSource::retrieveColor(cv::Mat &color)
    {
        // libuvc doesn't provide for separate grab() and retrieve() operations.
        // Instead, grab() will set a flag to alert the callback function that
        // we wish to store the next frame and retrieve() will return that
        // frame.

        std::lock_guard<std::mutex> guard(mutex_);

        if (!frame_) {
            throw std::runtime_error("Error: There's no frame available.");
        }

        // We'll convert the image from YUV/JPEG to rgb, so allocate space
        auto rgb = uvc_allocate_frame(frame_->width * frame_->height * 3);
        if (!rgb) {
            throw std::runtime_error("Error: Unable to allocate the rgb frame.");
        }

        // Do the rgb conversion
        auto convert_ret = uvc_mjpeg2rgb(frame_, rgb);
        if (UVC_SUCCESS != convert_ret) {
            // Try any2rgb() instead
            auto any_ret = uvc_any2rgb(frame_, rgb);
            if (UVC_SUCCESS != any_ret) {
                uvc_free_frame(rgb);
                throw std::runtime_error("Error: Unable to convert frame to rgb: " + std::string(uvc_strerror(convert_ret)));
            }
        }

        // Convert the image to at cv::Mat
        auto img = cvCreateImageHeader(cvSize(rgb->width, rgb->height), IPL_DEPTH_8U, 3);
        cvSetData(img, rgb->data, rgb->width * 3);
        color = cv::cvarrToMat(img);

        uvc_free_frame(rgb);
    }

    void UVCImageSource::callback(uvc_frame_t *frame, void *ptr)
    {
        auto me = static_cast<UVCImageSource*>(ptr);
        me->callback(frame);
    }

    void UVCImageSource::callback(uvc_frame_t *frame)
    {
        // Just copy the UVC frame to keep things quick. We'll do any conversion
        // upon request in the retrieveColor() method.
        std::lock_guard<std::mutex> guard(mutex_);
        if (frame_)
            uvc_free_frame(frame_);
        frame_ = uvc_allocate_frame(frame->data_bytes);
        uvc_duplicate_frame(frame, frame_);
    }

    /// Factory method to open a USB video class (UVC) device as an image
    /// source.
    ImageSourcePtr openUVCCamera(int vendor_id, int product_id, const char* serial_number)
    {
        auto ret = ImageSourcePtr{};
        try {
            auto source = new UVCImageSource(vendor_id, product_id, serial_number);
            ret.reset(source);
        } catch (const std::exception& e) {
            std::cerr << "Caught exception initializing UVC camera image source: " << e.what() << std::endl;
        }
        return ret;
    }

    /// Factory method to open the HDK camera as an image source via libuvc.
    ImageSourcePtr openHDKCameraUVC()
    {
        const int vendor_id = 0x0bda;
        const int product_id = 0x57e8;
        return openUVCCamera(vendor_id, product_id);
    }

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_UVCImageSource_cpp_GUID_2563F019_11B6_4F61_9E8C_C3ED2A573AF6
