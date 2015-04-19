/** @file
    @brief Wrapper for a vrpn_Tracker_VideoBasedHMDTracker

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>
#include "Oculus_DK2.h"

// Generated JSON header file
#include "com_osvr_VideoBasedHMDTracker_json.h"

// Library/third-party includes
#include <opencv2/core/core.hpp> // for basic OpenCV types
#include <opencv2/core/operations.hpp>
#include <opencv2/highgui/highgui.hpp> // for image capture
#include <opencv2/imgproc/imgproc.hpp> // for image scaling

#include <boost/noncopyable.hpp>

// Standard includes
#include <iostream>
#include <sstream>

#define VBHMD_DEBUG

// Anonymous namespace to avoid symbol collision
namespace {

class VideoBasedHMDTracker : boost::noncopyable {
  public:
      VideoBasedHMDTracker(OSVR_PluginRegContext ctx, int cameraNum = 0, int channel = 0)
          : m_camera(cameraNum), m_channel(channel) {

        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // Configure the tracker interface.
        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Come up with a device name
        std::ostringstream os;
        os << "TrackedCamera" << cameraNum << "_" << channel;

        /// Create an asynchronous (threaded) device
        m_dev.initAsync(ctx, os.str(), opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(
            com_osvr_VideoBasedHMDTracker_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);

        if (m_camera.isOpened()) {
            int height = static_cast<int>(m_camera.get(CV_CAP_PROP_FRAME_HEIGHT));
            int width = static_cast<int>(m_camera.get(CV_CAP_PROP_FRAME_WIDTH));

            // See if this is an Oculus camera by checking the dimensions of
            // the image.  This camera type improperly describes its format
            // as being a color format when it is in fact a mono format.
            m_isOculusCamera = (width == 376) && (height == 480);
#ifdef VBHMD_DEBUG
            std::cout << "Got image of size " << width << "x" << height
                << ", Format " << m_camera.get(CV_CAP_PROP_FORMAT)
                << ", Mode " << m_camera.get(CV_CAP_PROP_MODE) << std::endl;
            if (m_isOculusCamera) {
                std::cout << "Is Oculus camera, reformatting to mono" << std::endl;
                m_dk2 = new osvr::oculus_dk2::Oculus_DK2_HID();
            }
#endif
        }
      }

    ~VideoBasedHMDTracker() {
        // Delete the DK2 if we have one (okay to delete a NULL pointer)
        delete m_dk2;
    }

    OSVR_ReturnCode update() {
        if (!m_camera.isOpened()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        // Trigger a camera grab.
        bool grabbed = m_camera.grab();
        if (!grabbed) {
            // No frame available.
            return OSVR_RETURN_SUCCESS;
        }
        bool retrieved = m_camera.retrieve(m_frame, m_channel);
        if (!retrieved) {
            return OSVR_RETURN_FAILURE;
        }

        // Keep track of when we got the image, since that is our
        // best estimate for when the tracker was at the specified
        // pose.
        OSVR_TimeValue timestamp;
        osvrTimeValueGetNow(&timestamp);

        // If we have an Oculus camera, then we need to reformat the
        // image pixels.  From the documentation: "Note OpenCV 1.x
        // functions cvRetrieveFrame and cv.RetrieveFrame return image
        // stored inside the video capturing structure. It is not
        // allowed to modify or release the image! You can copy
        // the frame using cvCloneImage() and then do whatever
        // you want with the copy."  (This comes from the web page:
        // http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html)
        // So we make a copy of the image in this case and later
        // release it.  Otherwise, we make a reference to it here
        // and then don't release it.
        cv::Mat m_frameCopy = m_frame.clone();
        if (m_isOculusCamera) {
            // From http://doc-ok.org/?p=1095
            // "It advertises itself as having a resolution of 376×480
            // pixels, and a YUYV pixel format (downsampled and interleaved
            // luminance/chroma channels, typical for webcams).  In reality,
            // the camera has a resolution of 752×480 pixels, and uses a
            // simple Y8 greyscale pixel format."
            // From http://www.fourcc.org/yuv.php
            // "...most popular of the various YUV 4:2:2 formats.
            // Horizontal sample period for Y = 1, V = 2, U = 2.
            // Macropixel = 2 image pixels.  U0Y0V0Y1"
            // This seems inconsistent.  From http://www.digitalpreservation.gov/formats/fdd/fdd000365.shtml
            // "Byte 0=8-bit Cb; Byte 1=8-bit Y'0", which seems to
            // say that the first byte is used to determine color
            // and the second to determine luminance, so we should
            // convert color back into another luminance.  Every
            // other byte is a color byte (half of them Cb and half
            // of them Cr).
            // NOTE: We'd like not to have to try and invert the bogus
            // transformation to get back to two luminance channels, but
            // rather just tell the camera to change its decoder.  It turns
            // out that the set(CV_CAP_PROP_FOURCC) is not actually implemented
            // for OpenCV (see cap_unicap.cpp) so we can't do that directly.
            // NOTE: OpenCV uses FFMPEG, which is able to read from cameras,
            // so may use it to capture data from cameras.  If so, that is the
            // driver to be adjusted.
            // NOTE: https://trac.ffmpeg.org/wiki/DirectShow talks about how
            // to get FFMPEG to tell you what formats each camera can produce
            // on Windows.  It lists only YUV for this camera.
            // NOTE: http://www.equasys.de/colorconversion.html provides
            // color conversion matrices to transform between RGB and other
            // color formats (including YCbCr); there are several different
            // matrices depending on intent.
            // NOTE: The FFMPEG file libavfilter/vf_colormatrix.c contains the
            // same coefficients as one of the conversions listed there, which
            // has the following conversion description:
            //  |Y |   | 0 |   | 0.299  0.587  0.114| |R|
            //  |Cb| = |128| + |-0.169 -0.133  0.500|.|G|
            //  |Cr|   |128|   | 0.500 -0.419 -0.081| |B|
            //
            //  |R| |1.000  0.000  1.400| |    Y   |
            //  |G|=|1.000 -0.343 -0.711|.|Cb - 128|
            //  |B| |1.000  1.765  0.000| |Cr - 128|
            // but the code uses coefficients with more resolution (and
            // stored in a different order):
            //  { { +0.5870, +0.1140, +0.2990 }, // Rec.601 (ITU-R BT.470-2/SMPTE 170M) (2)
            //  { -0.3313, +0.5000, -0.1687 },
            //  { -0.4187, -0.0813, +0.5000 } }
            // (but this is only one of four choices, choice 2 of 0-3).
            // These are used in filter_frame(), which switches based on
            // the color space of the source and destination.  The FFMPEG
            // output on the DK2 does not specify the color space, just the
            // encoding format.
            // Inverting this set of equations produces the following
            // conversion back to YCbCr:
            //  TODO
            // m_frameCopy.TODO;

            // Read any reports and discard them.  We do this to keep the
            // LED keepAlive going.
            m_dk2->poll();
        }

#ifdef VBHMD_DEBUG
        if (m_camera.isOpened()) {
            cv::imshow("Debug window", m_frame);
            cv::waitKey(1);
        }
#endif

        // Compute the pose of the HMD w.r.t. the camera frame of reference.
        // NOTE: Use the m_frameCopy image for our calculations, because
        // it will have been reformatted as needed.
        OSVR_PoseState pose;
        osvrPose3SetIdentity(&pose);
        // XXX Compute pose here.

        /// Report the new pose, time-stamped with the time we
        // received the image from the camera.
        osvrDeviceTrackerSendPoseTimestamped(m_dev, m_tracker,
            &pose, 0, &timestamp);
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
    cv::VideoCapture m_camera;
    int m_channel;
    cv::Mat m_frame;
    cv::Mat m_frameCopy;
    bool m_isOculusCamera;    //< Is this image from and Oculus camera?
    osvr::oculus_dk2::Oculus_DK2_HID *m_dk2 = NULL;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

        {
            // Autodetect camera.  This needs to have the same
            // parameter as the constructor for the VideoBasedHMDTracker
            // class above or else it will not be looking for the
            // same camera.  This instance of the camera will auto-
            // delete itself when this block finishes, so should
            // close the camera -- leaving it to be opened again
            // in the constructor.
            cv::VideoCapture cap(0);
            if (!cap.isOpened()) {
                // Failed to find camera
                return OSVR_RETURN_FAILURE;
            }
        }
        m_found = true;

        /// Create our device object, passing the context.
        osvr::pluginkit::registerObjectForDeletion(ctx, new VideoBasedHMDTracker(ctx));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance, so that only one tracker will use this camera.)
    bool m_found;
};
} // namespace

OSVR_PLUGIN(com_osvr_VideoBasedHMDTracker) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerHardwareDetectCallback(new HardwareDetection());

    return OSVR_RETURN_SUCCESS;
}

