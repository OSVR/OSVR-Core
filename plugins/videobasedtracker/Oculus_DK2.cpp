/** @file
@brief Implementation of interface for an Oculus DK2 HID device

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

#include "Oculus_DK2.h"
//#include <opencv2/core/operations.hpp>
#include <opencv2/imgproc/imgproc.hpp> // for image scaling

using namespace osvr;
using namespace oculus_dk2;

static const vrpn_uint16 OCULUS_VENDOR = 0x2833;
static const vrpn_uint16 DK2_PRODUCT = 0x0021;

Oculus_DK2_HID::Oculus_DK2_HID(double keepAliveSeconds)
    : vrpn_HidInterface(
          m_filter = new vrpn_HidProductAcceptor(OCULUS_VENDOR, DK2_PRODUCT)) {
    // Store keep-alive interval.
    m_keepAliveSeconds = keepAliveSeconds;

    // Send a command to turn on the LEDs and record the time at which
    // we did so.
    writeLEDControl();
    vrpn_gettimeofday(&m_lastKeepAlive, NULL);
}

Oculus_DK2_HID::~Oculus_DK2_HID() {
    // Turn off the LEDs
    writeLEDControl(false);

    // Clean up our memory.
    delete m_filter;
}

std::vector<OCULUS_IMU_REPORT> Oculus_DK2_HID::poll() {
    // See if it has been long enough to send another keep-alive to
    // the LEDs.
    struct timeval now;
    vrpn_gettimeofday(&now, NULL);
    if (vrpn_TimevalDurationSeconds(now, m_lastKeepAlive) >=
        m_keepAliveSeconds) {
        writeKeepAlive();
        m_lastKeepAlive = now;
    }

    // Clear old reports, which will have already been returned.
    // Read and parse any available IMU reports from the DK2, which will put
    // them into the report vector.
    m_reports.clear();
    update();

    return m_reports;
}

// Thank you to Oliver Kreylos for the info needed to write this function.
// It is based on his OculusRiftHIDReports.cpp, used with permission.
void Oculus_DK2_HID::writeLEDControl(
    bool enable, vrpn_uint16 exposureLength, vrpn_uint16 frameInterval,
    vrpn_uint16 vSyncOffset, vrpn_uint8 dutyCycle, vrpn_uint8 pattern,
    bool autoIncrement, bool useCarrier, bool syncInput, bool vSyncLock,
    bool customPattern, vrpn_uint16 commandId) {
    // Buffer to store our report in.
    vrpn_uint8 pktBuffer[13];

    /* Pack the packet buffer, using little-endian packing: */
    vrpn_uint8 *bufptr = pktBuffer;
    vrpn_int32 buflen = sizeof(pktBuffer);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, vrpn_uint8(0x0cU));
    vrpn_buffer_to_little_endian(&bufptr, &buflen, commandId);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, pattern);
    vrpn_uint8 flags = 0x00U;
    if (enable) {
        flags |= 0x01U;
    }
    if (autoIncrement) {
        flags |= 0x02U;
    }
    if (useCarrier) {
        flags |= 0x04U;
    }
    if (syncInput) {
        flags |= 0x08U;
    }
    if (vSyncLock) {
        flags |= 0x10U;
    }
    if (customPattern) {
        flags |= 0x20U;
    }
    vrpn_buffer_to_little_endian(&bufptr, &buflen, flags);
    vrpn_buffer_to_little_endian(&bufptr, &buflen,
                                 vrpn_uint8(0x0cU)); // Reserved byte
    vrpn_buffer_to_little_endian(&bufptr, &buflen, exposureLength);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, frameInterval);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, vSyncOffset);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, dutyCycle);

    /* Write the LED control feature report: */
    send_feature_report(sizeof(pktBuffer), pktBuffer);
}

// Thank you to Oliver Kreylos for the info needed to write this function.
// It is based on his OculusRiftHIDReports.cpp, used with permission.
void Oculus_DK2_HID::writeKeepAlive(bool keepLEDs, vrpn_uint16 interval,
                                    vrpn_uint16 commandId) {
    // Buffer to store our report in.
    vrpn_uint8 pktBuffer[6];

    /* Pack the packet buffer, using little-endian packing: */
    vrpn_uint8 *bufptr = pktBuffer;
    vrpn_int32 buflen = sizeof(pktBuffer);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, vrpn_uint8(0x11U));
    vrpn_buffer_to_little_endian(&bufptr, &buflen, commandId);
    vrpn_uint8 flags = keepLEDs ? 0x0bU : 0x01U;
    vrpn_buffer_to_little_endian(&bufptr, &buflen, flags);
    vrpn_buffer_to_little_endian(&bufptr, &buflen, interval);

    /* Write the LED control feature report: */
    send_feature_report(sizeof(pktBuffer), pktBuffer);
}

void Oculus_DK2_HID::on_data_received(size_t bytes, vrpn_uint8 *buffer) {
    // Fill new entries into the vector that will be passed back
    // on the next poll().
    //   TODO: Read the values from the IMU and store them into the
    // vector that we'll return on the next call to poll().
    // XXX
}

cv::Mat osvr::oculus_dk2::unscramble_image(const cv::Mat &image) {
    //   From the documentation: "Note OpenCV 1.x
    // functions cvRetrieveFrame and cv.RetrieveFrame return image
    // stored inside the video capturing structure. It is not
    // allowed to modify or release the image! You can copy
    // the frame using cvCloneImage() and then do whatever
    // you want with the copy."  (This comes from the web page:
    // http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html)

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
    // This seems inconsistent.  From
    // http://www.digitalpreservation.gov/formats/fdd/fdd000365.shtml
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
    //  { { +0.5870, +0.1140, +0.2990 }, // Rec.601 (ITU-R BT.470-2/SMPTE 170M)
    //  (2)
    //  { -0.3313, +0.5000, -0.1687 },
    //  { -0.4187, -0.0813, +0.5000 } }
    // (but this is only one of four choices, choice 2 of 0-3).
    // These are used in filter_frame(), which switches based on
    // the color space of the source and destination.  The FFMPEG
    // output on the DK2 does not specify the color space, just the
    // encoding format.

    // Okay, so here we convert from BGR back into YUV.
    cv::Mat yuvImage;
    cv::cvtColor(image, yuvImage, cv::COLOR_BGR2YCrCb);

    // Then we repack the individual components; every Y is used
    // (second and fourth entry), but Cb is the first entry of
    // four and Cr the third:  Cb0 Y0 Cr0 Y1 Cb2 Y2 Cr2 Y3.
    // So the image itself has interplated Cb and Cr values...
    // For now, we do a brain-dead conversion, where we double the width
    // of the YUV image, make it grayscale, and copy the Y channels from
    // the input image into neighboring pixels in the output image; doubling
    // every one.
    //  TODO: Invert the transformation used to get from YUV to BGR and
    // determine the actual components.
    cv::Mat outImage(yuvImage.rows, yuvImage.cols * 2, CV_8UC1, cv::Scalar(0));
    for (int r = 0; r < yuvImage.rows; r++) {
        for (int c = 0; c < yuvImage.cols; c++) {
            outImage.at<unsigned char>(r, c * 2) =
                yuvImage.at<cv::Vec3b>(r, c)[0];
            outImage.at<unsigned char>(r, c * 2 + 1) =
                yuvImage.at<cv::Vec3b>(r, c)[0];
        }
    }

    return outImage;
}
