/** @file
    @brief Main file for a video-based HMD tracker.

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
#include "Oculus_DK2.h"
#include "VideoBasedTracker.h"
#include "HDKLedIdentifierFactory.h"
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "com_osvr_VideoBasedHMDTracker_json.h"

// Library/third-party includes
#include <opencv2/core/core.hpp> // for basic OpenCV types
#include <opencv2/core/operations.hpp>
#include <opencv2/highgui/highgui.hpp> // for image capture
#include <opencv2/imgproc/imgproc.hpp> // for image scaling
#include <json/value.h>
#include <json/reader.h>

#include <boost/noncopyable.hpp>

// Standard includes
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>

// Define the constant below to use a DirectShow-based workaround to not
// being able to open the OSVR HDK camera using OpenCV.
/// @todo Remove this code and the DirectShow stuff once the camera can be read
/// by OpenCV
#define VBHMD_USE_DIRECTSHOW
#ifdef VBHMD_USE_DIRECTSHOW
#include "directx_camera_server.h"
#include "DirectShowHDKCameraFactory.h"
#include "DirectShowToCV.h"
using CameraPtr = std::unique_ptr<directx_camera_server>;
#else
using CameraPtr = std::unique_ptr<cv::VideoCapture>;
#endif

// Define the constant below to print timing information (how many updates
// per second we are getting).
//#define VBHMD_TIMING

// Define the constant below to set a directory to save the video frames that
// are acquired
// with files in a format that can later be read by VBHMD_FAKE_IMAGES
//#define VBHMD_SAVE_IMAGES "./Frames"

// Define the constant below to read from a set of files with names
// 0001.tif and above; specify the directory name to read from
//#define VBHMD_FAKE_IMAGES "./Frames"
//#define VBHMD_FAKE_IMAGES "C:/tmp/HDK_far"
//#define VBHMD_FAKE_IMAGES "F:/taylorr/Personal/Work/consulting/sensics/OSVR/src/OSVR-Core/plugins/videobasedtracker/simulated_images/animation_from_fake"
//#define VBHMD_FAKE_IMAGES "F:/taylorr/Personal/Work/consulting/sensics/OSVR/src/OSVR-Core/plugins/videobasedtracker/HDK_random_images"
//#define VBHMD_FAKE_IMAGES "C:/tmp/Roman_125_400_GAIN_1p5m"

// Anonymous namespace to avoid symbol collision
namespace {

class VideoBasedHMDTracker : boost::noncopyable {
  public:
    VideoBasedHMDTracker(OSVR_PluginRegContext ctx, CameraPtr &&camera,
                         int devNumber = 0, bool showDebug = false)
#ifndef VBHMD_FAKE_IMAGES
        : m_camera(std::move(camera)), m_vbtracker(showDebug)
#else
        : m_vbtracker(showDebug)
#endif
    {
        // Set the number of threads for OpenCV to use.
        cv::setNumThreads(1);

        // Initialize things from parameters and from defaults.  Do it here
        // rather than in an initialization list so that we're independent of
        // member order declaration.
        m_channel = 0;
        m_type = Unknown;

        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        // Configure the tracker interface.
        osvrDeviceTrackerConfigure(opts, &m_tracker);

        /// Come up with a device name
        std::ostringstream os;
        os << "TrackedCamera" << devNumber << "_" << m_channel;

        /// Create an asynchronous (threaded) device
        m_dev.initAsync(ctx, os.str(), opts);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(com_osvr_VideoBasedHMDTracker_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);

        //===============================================
        // Figure out what type of HMD we're using.
        int height = 0;
        int width = 0;
#ifdef VBHMD_FAKE_IMAGES
        // Read a vector of images, which we'll loop through.
        int imageNum = 1;
        do {
            std::ostringstream fileName;
            fileName << VBHMD_FAKE_IMAGES << "/";
            fileName << std::setfill('0') << std::setw(4) << imageNum++;
            fileName << ".tif";
            cv::Mat image;
            std::cout << "Trying to read image from " << fileName.str()
                      << std::endl;
            image = cv::imread(fileName.str().c_str(), CV_LOAD_IMAGE_COLOR);
            if (!image.data) {
                break;
            }
            m_images.push_back(image);
        } while (true);
        m_currentImage = 0;

        if (m_images.size() == 0) {
            std::cerr << "Could not read any images from " << VBHMD_FAKE_IMAGES
                      << std::endl;
            return;
        }

        // The fake tracker uses real LED positions and a
        // simulated camera, whose parameters we describe here.
        width = m_images[0].cols;
        height = m_images[0].rows;
        m_type = Fake;

        // Here's where we get 700 for the focal length:
        // The aspect ratio of the camera in Blender is determined by two
        // parameters:
        // the focal length(which defaults to 35mm) and the sensor size (which
        // is set on the camera control panel, and defaults to 32 in my
        // version). This is the horizontal size of the image sensor in mm,
        // according to the pop-up info box. The model in OpenCV only has the
        // focal length as a parameter. So the question becomes how to set the
        // sensor size in Blender to match what OpenCV is expecting. The basic
        // issue is that the camera matrix in OpenCV stores the focal length
        // in pixel units, not millimeters.  For the default image sensor width
        // of 32mm, and an image resolution of 640x480, we compute the OpenCV
        // focal length as follows: 35mm * (640 pixels / 32 mm) = 700 pixels.
        // The resulting camera matrix would be: { {700, 0, 320}, {0, 700, 240},
        // {0, 0, 1} }

        double cx = width / 2.0;
        double cy = height / 2.0;
        double fx = 700;
        double fy = fx;
        std::vector<std::vector<double> > m;
        m.push_back({fx, 0.0, cx});
        m.push_back({0.0, fy, cy});
        m.push_back({0.0, 0.0, 1.0});
        std::vector<double> d;
        d.push_back(0);
        d.push_back(0);
        d.push_back(0);
        d.push_back(0);
        d.push_back(0);
        //        m_identifiers.push_back(new
        //        osvr::vbtracker::OsvrHdkLedIdentifier(osvr::vbtracker::OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS));

        m_vbtracker.addSensor(
            osvr::vbtracker::createHDKLedIdentifierSimulated(0), m, d,
            osvr::vbtracker::OsvrHdkLedLocations_SENSOR0, 4, 2);
        // There are sometimes only four beacons on the back unit (two of
        // the LEDs are disabled), so we let things work with just those.
        m_vbtracker.addSensor(
            osvr::vbtracker::createHDKLedIdentifierSimulated(1), m, d,
            osvr::vbtracker::OsvrHdkLedLocations_SENSOR1, 4, 0);

#else
#ifdef VBHMD_USE_DIRECTSHOW
        // Read a frame from the camera, so that we get all of the info we
        // need and start the filter graph running
        /// @Todo Move this into the device itself, so it is ready to go
        // as soon as it is opened.
        m_camera->read_image_to_memory();
#endif

        if (m_camera->isOpened()) {
#ifdef VBHMD_USE_DIRECTSHOW
            int minx, miny, maxx, maxy;
            m_camera->read_range(minx, maxx, miny, maxy);
            height = maxy - miny + 1;
            width = maxx - minx + 1;
#else
            height = static_cast<int>(m_camera->get(CV_CAP_PROP_FRAME_HEIGHT));
            width = static_cast<int>(m_camera->get(CV_CAP_PROP_FRAME_WIDTH));
#endif

            // See if this is an Oculus camera by checking the dimensions of
            // the image.  This camera type improperly describes its format
            // as being a color format when it is in fact a mono format.
            bool isOculusCamera = (width == 376) && (height == 480);
            if (isOculusCamera) {
                m_type = OculusDK2;
            } else {
                /// @todo Check to see if the resolution/name matches the OSVR
                /// HDK camera
                m_type = OSVRHDK;
            }
            if (m_type == OculusDK2) {
                std::cout << "Is Oculus camera, reformatting to mono"
                          << std::endl;
                m_dk2.reset(new osvr::oculus_dk2::Oculus_DK2_HID());
            }
        }

        //===============================================
        // Configure objects and set up data structures and devices based on the
        // type of device we have.

        switch (m_type) {
        case OculusDK2: {
            // TODO: Fill these in when they are known
            // m_identifiers.push_back(XXX);
            // m_estimator.push_back(XXX);
            m_vbtracker.addOculusSensor();

            // Set Oculus' camera capture parameters as described in Oliver
            // Kreylos' OculusRiftDK2VideoDevice.cpp program.  Thank you for him
            // for sharing this with us, used with permission.

            // Trying to find the closest matches to what was being done
            // in OculusRiftDK2VideoDevice.cpp, but I don't think we're going to
            // be able to set everything we need to.  In fact, these don't seem
            // to be doing anything (gain does not change the brightness, for
            // example) and all but the gain setting fails (we must not have the
            // XIMEA interface).
            /// @todo There is no OS-independent way to set these parameters on
            /// the camera, so we're not going to be able to use it.
            /// @todo Would like to set a number of things, but since these are
            ///  not working, giving up.
        } break;

        case OSVRHDK: {
            // See http://paulbourke.net/miscellaneous/lens/ for diagram.
            // The focal length of a lens is the distance from the center of
            // the lens to the point at which objects and infinity focus.  The
            // horizontal field of view = 2 * atan (0.5 * width / focal length).
            //  hFOV / 2 = atan( 0.5 * W / FL )
            //  tan (hFOV / 2) = 0.5 * W / FL
            //  2 * tan (hFOV/2) = W / FL
            //  FL = W / ( 2 * tan(hFOV/2) )
            // We want the focal length in units of pixels.  The manufacturer
            // tells us that the optical FOV is 82.9 degrees; we assume that
            // this is a diagonal measurement.  Since the camera resolution is
            // 640x480 pixels, the diagonal is sqrt( 640*640 + 480*480 ) = 800.
            // If we use the equation above, but compute the diagonal focal
            // length (they are linearly related), we get
            //  FL = 800 / ( 2 * tan(82.9/2) )
            //  FL = 452.9
            // Testing with the tracked position when the unit was about 10
            // inches from the camera and nearly centered produced a Z estimate
            // of 0.255, where 0.254 is expected.  This is well within the
            // ruler-based method of estimating 10 inches, so seems to be
            // correct.

            // Distortion correction comes from the following data points:
            // (first column is distance from center in units of millimeters;
            // second column is distortion in percent).
            // 1.19     0.944444444
            // 1.104    1.152777778
            // 1.032    1.319444444
            // 0.89     1.430555556
            // 0.642    1.833333333
            // 0.514    2.041666667
            // 0.368    2.347222222
            // 0.302    2.347222222
            // 0.208    1.861111111
            // 0.082    0.666666667
            // 0.004    0.027777778
            // 0 0
            // Convert these to a function as follows:
            // -The sensor is 640x480 with 3 micron pixels.
            // -Center of the sensor is (320, 240)
            // -So, for instance, if you have a pixel at (500,400), it is (180,
            //   160) pixels away from the center so sqrt(180x180 + 160x160) =
            //   240.83 pixels.
            //   This means 240.83 x 3 = 722.49 microns or 0.72249 mm.
            //   Thus, you read 0.722 on the left column and you get the
            //   distortion value in percent by reading from the right column.
            //  -Multiply (@todo or divide, I'm never certain) the distance by
            //   (1 + that factor)
            // (That's the real radius.)
            // That conversion followed by multiplying back to pixel units
            // results in the following camera-pixel to actual-location mapping
            // (inverting the order, so the center is at the top):
            //  0.000	0.000
            //  1.333	1.334
            //  27.333	27.516
            //  69.333	70.624
            //  100.667	103.030
            //  122.667	125.546
            //  171.333	174.831
            //  214.000	217.923
            //  296.667	300.911
            //  344.000	348.539
            //  368.000	372.242
            //  396.667	400.413
            // The k1-k3 coefficients we want to solve for are the ones that
            // solve the equation: rCorrected = r(1 + k1*r^2 + k2*r^4 + k3*r^6)
            // So, rCorrected/r = 1 + k1*r^2 + k2*r^4 + k3*r^6
            // So, rCorrected/r - 1 = k1*r^2 + k2*r^4 + k3*r^6
            // Solving for (rCorrected/r - 1) in the above equations, ignoring
            // 0, produces the following list of values, where the left column
            // is r and the right is rCorrected/r - 1:
            //  1.333	0.000277778
            //  27.333	0.006666667
            //  69.333	0.018611111
            //  100.667	0.023472222
            //  122.667	0.023472222
            //  171.333	0.020416667
            //  214.000	0.018333333
            //  296.667	0.014305556
            //  344.000	0.013194444
            //  368.000	0.011527778
            //  396.667	0.009444444
            // (which of course is the second column of the first table /100)
            // Using the "R" programming environment to solve for the k1, k2, k3
            // for the equation Y = k1*x^2 + k2*x^4 + x3*x^6, where X = r (left
            // column) and Y = rCorrected/2-1 (right column):
            // R code:
            /*
              x <- c(1.333, 27.333, 69.333, 100.667, 122.667, 171.333, 214.000,
              296.667, 344.000, 368.000, 396.667)
              y <- c(0.000277778, 0.006666667, 0.018611111, 0.023472222,
              0.023472222, 0.020416667, 0.018333333, 0.014305556, 0.013194444,
              0.011527778, 0.009444444)
              model <- lm(y~0+I(x^2)+I(x^4)+I(x^6))
              coefficients(model)
            */
            // Output:
            //       I(x^2)        I(x^4)        I(x^6)
            // 1.252916e-06 -1.752020e-11  6.405327e-17

            // The manufacturer specs distortion < 3% on the module and 1.5% on
            // the lens, so we ignore the distortion where unknown and put in 0
            // coefficients.
            double cx = width / 2.0;
            double cy = height / 2.0;
            double fx =
                452.9; // 700.0; // XXX This needs to be in pixels, not mm
            double fy = fx;
            std::vector<std::vector<double> > m;
            m.push_back({fx, 0.0, cx});
            m.push_back({0.0, fy, cy});
            m.push_back({0.0, 0.0, 1.0});
            /// @todo Make it possible to read these values from the config file
            /// for the camera
            double k1 = 1.252916e-06;
            double k2 = -1.752020e-11;
            double k3 = 6.405327e-17;
            double p1 = 0;
            double p2 = 0;
            std::vector<double> d = {k1, k2, p1, p2, k3};
            m_vbtracker.addSensor(
                osvr::vbtracker::createHDKLedIdentifier(0), m, d,
                osvr::vbtracker::OsvrHdkLedLocations_SENSOR0, 6, 0);
            m_vbtracker.addSensor(
                osvr::vbtracker::createHDKLedIdentifier(1), m, d,
                osvr::vbtracker::OsvrHdkLedLocations_SENSOR1, 4, 0);

        } break;

        default: // Also handles the "Unknown" case.
            // We've already got a NULL identifier and estimator, so nothing to
            // do.
            break;
        }
#endif
    }

    OSVR_ReturnCode update() {
#ifdef VBHMD_FAKE_IMAGES
        // Wrap the image count back around if it has gone too
        // high.
        if (m_currentImage >= m_images.size()) {
            m_currentImage = 0;
        }
        // Read an image if there is one to be had, and
        // increment the frame count.  Otherwise, fail.
        if (m_currentImage >= m_images.size()) {
            return OSVR_RETURN_FAILURE;
        } else {
            m_frame = m_images[m_currentImage++];
        }

        // Sleep 1/120th of a second, to simulate a reasonable
        // frame rate.
        vrpn_SleepMsecs(1000 / 120);
#else
        if (!m_camera->isOpened()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

//==================================================================
// Trigger a camera grab.  Pull it into an OpenCV matrix named
// m_frame.
#ifdef VBHMD_USE_DIRECTSHOW
        if (!m_camera->read_image_to_memory()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }
        m_frame = retrieve(*m_camera);

#else
        if (!m_camera->grab()) {
            // No frame available.
            return OSVR_RETURN_SUCCESS;
        }
        if (!m_camera->retrieve(m_frame, m_channel)) {
            return OSVR_RETURN_FAILURE;
        }
#endif

#ifdef VBHMD_SAVE_IMAGES
        // If we're supposed to save images, make file names that match the
        // format we need to read them back in again and save the images.
        std::ostringstream fileName;
        fileName << VBHMD_SAVE_IMAGES << "/";
        fileName << std::setfill('0') << std::setw(4) << m_imageNum++;
        fileName << ".tif";
        if (!cv::imwrite(fileName.str().c_str(), m_frame)) {
            std::cerr << "Could not write image to " << fileName.str()
                      << std::endl;
        }

#endif

#endif

#ifdef VBHMD_TIMING
        //==================================================================
        // Time our performance
        static struct timeval last = {0, 0};
        if (last.tv_sec == 0) {
            vrpn_gettimeofday(&last, NULL);
        }
        static unsigned count = 0;
        if (++count == 100) {
            struct timeval now;
            vrpn_gettimeofday(&now, NULL);
            double duration = vrpn_TimevalDurationSeconds(now, last);
            std::cout << "Video-based tracker: update rate " << count / duration
                      << " hz" << std::endl;
            count = 0;
            last = now;
        }
#endif

        //==================================================================
        // Keep track of when we got the image, since that is our
        // best estimate for when the tracker was at the specified
        // pose.
        // TODO: Back-date the aquisition time by the expected image
        // transfer time and perhaps by half the exposure time to say
        // when the photons actually arrived.
        OSVR_TimeValue timestamp;
        osvrTimeValueGetNow(&timestamp);

        //==================================================================
        // If we have an Oculus camera, then we need to reformat the
        // image pixels.
        if (m_type == OculusDK2) {
            m_imageGray = osvr::oculus_dk2::unscramble_image(m_frame);

            // Read any reports and discard them.  We do this to keep the
            // LED keepAlive going.
            m_dk2->poll();
        } else {
            //==================================================================
            // Convert the image into a format we can use.
            // TODO: Consider reading in the image in gray scale to begin with
            cv::cvtColor(m_frame, m_imageGray, CV_RGB2GRAY);
        }

        m_vbtracker.processImage(
            m_frame, m_imageGray,
            [&](OSVR_ChannelCount sensor, OSVR_Pose3 const &pose) {

                //==================================================================
                // Report the new pose, time-stamped with the time we
                // received the image from the camera.
                osvrDeviceTrackerSendPoseTimestamped(m_dev, m_tracker, &pose,
                                                     sensor, &timestamp);
            });

        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
#ifdef VBHMD_FAKE_IMAGES
    std::vector<cv::Mat> m_images;
    size_t m_currentImage;
#else
    CameraPtr m_camera;
#endif
#ifdef VBHMD_SAVE_IMAGES
    int m_imageNum = 1;
#endif
    int m_channel;
    cv::Mat m_frame;
    cv::Mat m_imageGray;

    osvr::vbtracker::VideoBasedTracker m_vbtracker;

    // What type of HMD are we tracking?
    enum { Unknown, OSVRHDK, OculusDK2, Fake } m_type;

    // In case we are using a DK2, we need a pointer to one.
    std::unique_ptr<osvr::oculus_dk2::Oculus_DK2_HID> m_dk2 = nullptr;
};

class HardwareDetection {
  public:
    HardwareDetection(int cameraID = 0, bool showDebug = false)
        : m_found(false) {
        m_cameraID = cameraID;
        m_showDebug = showDebug;
    }

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }
        CameraPtr cam;
#ifndef VBHMD_FAKE_IMAGES
#ifdef VBHMD_USE_DIRECTSHOW
        // Open a DirectShow camera and make sure we can read an
        // image from it. We now filter by path prefix to make sure it only
        // finds HDK cameras, not whatever random webcam comes up first.
        cam = getDirectShowHDKCamera();
        if (!cam || !cam->read_image_to_memory()) {
            return OSVR_RETURN_FAILURE;
        }
#else
        // Autodetect camera.
        cam.reset(new cv::VideoCapture(m_cameraID));
        if (!cap->isOpened()) {
            // Failed to find camera
            return OSVR_RETURN_FAILURE;
        }
#endif
#endif
        m_found = true;

        /// Create our device object, passing the context and moving the camera.
        std::cout << "Opening camera " << m_cameraID << std::endl;
        osvr::pluginkit::registerObjectForDeletion(
            ctx, new VideoBasedHMDTracker(ctx, std::move(cam), m_cameraID,
                                          m_showDebug));

        return OSVR_RETURN_SUCCESS;
    }

  private:
    /// @brief Have we found our device yet? (this limits the plugin to one
    /// instance, so that only one tracker will use this camera.)
    bool m_found;

    int m_cameraID;   //< Which OpenCV camera should we open?
    bool m_showDebug; //< Show windows with video to help debug?
};

class ConfiguredDeviceConstructor {
  public:
    /// @brief This is the required signature for a device instantiation
    /// callback.
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        // Read the JSON data from parameters.
        Json::Value root;
        if (params) {
            Json::Reader r;
            if (!r.parse(params, root)) {
                std::cerr << "Could not parse parameters!" << std::endl;
            }
        }

        // Read these parameters from a "params" field in the device Json
        // configuration file.

        // Using `get` here instead of `[]` lets us provide a default value.
        int cameraID = root.get("cameraID", 0).asInt();
        bool showDebug = root.get("showDebug", false).asBool();

        // OK, now that we have our parameters, create the device.
        osvr::pluginkit::PluginContext context(ctx);
        context.registerHardwareDetectCallback(
            new HardwareDetection(cameraID, showDebug));

        return OSVR_RETURN_SUCCESS;
    }
};

} // namespace

OSVR_PLUGIN(com_osvr_VideoBasedHMDTracker) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Tell the core we're available to create a device object.
    osvr::pluginkit::registerDriverInstantiationCallback(
        ctx, "VideoBasedHMDTracker", new ConfiguredDeviceConstructor);

    return OSVR_RETURN_SUCCESS;
}
