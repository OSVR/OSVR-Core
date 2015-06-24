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
#include "LED.h"
#include "BeaconBasedPoseEstimator.h"

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
#include <iomanip>
#include <sstream>
#include <memory>

// Define the constant below to provide debugging (window showing video and
// behavior, printing tracked positions)
#define VBHMD_DEBUG

// Define the constant below to read from a set of files with names
// 0001.tif and above; specify the directory name to read from
//#define VBHMD_FAKE_IMAGES "F:/taylorr/Personal/Work/consulting/sensics/OSVR/src/OSVR-Core/plugins/videobasedtracker/simulated_images/animation_from_fake"
//#define VBHMD_FAKE_IMAGES "F:/taylorr/Personal/Work/consulting/sensics/OSVR/src/OSVR-Core/plugins/videobasedtracker/HDK_random_images"

// Anonymous namespace to avoid symbol collision
namespace {

class VideoBasedHMDTracker : boost::noncopyable {
  public:
    VideoBasedHMDTracker(OSVR_PluginRegContext ctx, int cameraNum = 0, int channel = 0)
#ifndef VBHMD_FAKE_IMAGES
        : m_camera(cameraNum)
#endif
    {
        // Initialize things from parameters and from defaults.  Do it here rather than
        // in an initialization list so that we're independent of member order declaration.
        m_channel = channel;
        m_type = Unknown;
        m_dk2 = nullptr;

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
#ifdef VBHMD_DEBUG
            std::cout << "Trying to read image from " << fileName.str() << std::endl;
#endif
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
        // The aspect ratio of the camera in Blender is determined by two parameters:
        // the focal length(which defaults to 35mm) and the sensor size (which is set
        // on the camera control panel, and defaults to 32 in my version). This is the
        // horizontal size of the image sensor in mm, according to the pop - up info
        // box. The model in OpenCV only has the focal length as a parameter. So the
        // question becomes how to set the sensor size in Blender to match what OpenCV
        // is expecting.
        // The basic issue is that the camera matrix in OpenCV stores the focal length
        // in pixel units, not millimeters.  For the default image sensor width of 32mm,
        // and an image resolution of 640x480, we compute the OpenCV focal length as
        // follows: 35mm * (640 pixels / 32 mm) = 700 pixels. The resulting camera
        // matrix would be: { {700, 0, 320}, {0, 700, 240}, {0, 0, 1} }

        double cx = width / 2.0;
        double cy = height / 2.0;
        double fx = 700;
        double fy = fx;
        std::vector< std::vector<double> > m;
        m.push_back({  fx, 0.0,  cx });
        m.push_back({ 0.0,  fy,  cy });
        m.push_back({ 0.0, 0.0, 1.0 });
        std::vector<double> d;
        d.push_back(0); d.push_back(0); d.push_back(0); d.push_back(0); d.push_back(0);
//        m_identifiers.push_back(new osvr::vbtracker::OsvrHdkLedIdentifier(osvr::vbtracker::OsvrHdkLedIdentifier_RANDOM_IMAGES_PATTERNS));
        m_identifiers.push_back(new osvr::vbtracker::OsvrHdkLedIdentifier(osvr::vbtracker::OsvrHdkLedIdentifier_SENSOR0_PATTERNS));
        m_identifiers.push_back(new osvr::vbtracker::OsvrHdkLedIdentifier(osvr::vbtracker::OsvrHdkLedIdentifier_SENSOR1_PATTERNS));
        m_estimators.push_back(new osvr::vbtracker::BeaconBasedPoseEstimator(m, d,
            osvr::vbtracker::OsvrHdkLedLocations_SENSOR0));
        m_estimators.push_back(new osvr::vbtracker::BeaconBasedPoseEstimator(m, d,
            osvr::vbtracker::OsvrHdkLedLocations_SENSOR1));
        std::list<osvr::vbtracker::Led> empty_led_group;
        m_led_groups.push_back(empty_led_group);
        m_led_groups.push_back(empty_led_group);
#else
        if (m_camera.isOpened()) {
            height = static_cast<int>(m_camera.get(CV_CAP_PROP_FRAME_HEIGHT));
            width = static_cast<int>(m_camera.get(CV_CAP_PROP_FRAME_WIDTH));

            // See if this is an Oculus camera by checking the dimensions of
            // the image.  This camera type improperly describes its format
            // as being a color format when it is in fact a mono format.
            bool isOculusCamera = (width == 376) && (height == 480);
            if (isOculusCamera) {
                m_type = OculusDK2;
            }

            // TODO: Check to see if the resolution/name matches the OSVR HDK camera
            else {
                m_type = OSVRHDK;
            }
    #ifdef VBHMD_DEBUG
            std::cout << "Got image of size " << width << "x" << height
                << ", Format " << m_camera.get(CV_CAP_PROP_FORMAT)
                << ", Mode " << m_camera.get(CV_CAP_PROP_MODE) << std::endl;
            if (m_type == OculusDK2) {
                std::cout << "Is Oculus camera, reformatting to mono" << std::endl;
                m_dk2.reset(new osvr::oculus_dk2::Oculus_DK2_HID());
            }
    #endif
        }

        //===============================================
        // Configure objects and set up data structures and devices based on the
        // type of device we have.

        switch (m_type) {
        case OculusDK2:
            {
                // TODO: Fill these in when they are known
                //m_identifiers.push_back(XXX);
                //m_estimator.push_back(XXX);
                std::list<osvr::vbtracker::Led> empty_led_group;
                m_led_groups.push_back(empty_led_group);

                // Set Oculus' camera capture parameters as described
                // in Oliver Kreylos' OculusRiftDK2VideoDevice.cpp program.  Thank you for
                // him for sharing this with us, used with permission.

                // Trying to find the closest matches to what was being done
                // in OculusRiftDK2VideoDevice.cpp, but I don't think we're going to
                // be able to set everything we need to.  In fact, these don't seem
                // to be doing anything (gain does not change the brightness, for
                // example) and all but the gain setting fails (we must not have the
                // XIMEA interface).
                //  TODO: There is no OS-independent way to set these parameters on
                // the camera, so we're not going to be able to use it.
                //  TODO: Would like to set a number of things, but since these are not working,
                // giving up.
            }
            break;

        case OSVRHDK:
            {
                // TODO: Come up with actual estimates for camera and distortion
                // parameters by calibrating them in OpenCV.
                double cx = width / 2.0;
                double cy = height / 2.0;
                double fx = 700.0;   // XXX This needs to be in pixels, not mm
                double fy = fx;
                std::vector< std::vector<double> > m;
                m.push_back({  fx, 0.0,  cx });
                m.push_back({ 0.0,  fy,  cy });
                m.push_back({ 0.0, 0.0, 1.0 });
                std::vector<double> d;
                d.push_back(0); d.push_back(0); d.push_back(0); d.push_back(0); d.push_back(0);
                m_identifiers.push_back(new osvr::vbtracker::OsvrHdkLedIdentifier(osvr::vbtracker::OsvrHdkLedIdentifier_SENSOR0_PATTERNS));
                m_identifiers.push_back(new osvr::vbtracker::OsvrHdkLedIdentifier(osvr::vbtracker::OsvrHdkLedIdentifier_SENSOR1_PATTERNS));
                m_estimators.push_back(new osvr::vbtracker::BeaconBasedPoseEstimator(m, d, osvr::vbtracker::OsvrHdkLedLocations_SENSOR0));
                m_estimators.push_back(new osvr::vbtracker::BeaconBasedPoseEstimator(m, d, osvr::vbtracker::OsvrHdkLedLocations_SENSOR1));
                std::list<osvr::vbtracker::Led> empty_led_group;
                m_led_groups.push_back(empty_led_group);
                m_led_groups.push_back(empty_led_group);
            }
            break;

        default:    // Also handles the "Unknown" case.
            // We've already got a NULL identifier and estimator, so nothing to do.
            break;
        }
#endif
    }

    ~VideoBasedHMDTracker() {
        // It is okay to delete NULL pointers, so we don't check here.
        for (size_t i = 0; i < m_estimators.size(); i++) {
            delete m_estimators[i];
        }
        for (size_t i = 0; i < m_identifiers.size(); i++) {
            delete m_identifiers[i];
        }
    }

    OSVR_ReturnCode update()
    {
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
        vrpn_SleepMsecs(1000/120);
#else
        if (!m_camera.isOpened()) {
            // Couldn't open the camera.  Failing silently for now. Maybe the
            // camera will be plugged back in later.
            return OSVR_RETURN_SUCCESS;
        }

        //==================================================================
        // Trigger a camera grab.
        if (!m_camera.grab()) {
            // No frame available.
            return OSVR_RETURN_SUCCESS;
        }
        if (!m_camera.retrieve(m_frame, m_channel)) {
            return OSVR_RETURN_FAILURE;
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
            m_frame = osvr::oculus_dk2::unscramble_image(m_frame);

            // Read any reports and discard them.  We do this to keep the
            // LED keepAlive going.
            m_dk2->poll();
        }

        //==================================================================
        // Convert the image into a format we can use.
        // TODO: Consider reading in the image in gray scale to begin with
        cv::cvtColor(m_frame, m_imageGray, CV_RGB2GRAY);

        //================================================================
        // Tracking the points

        // Threshold the image based on the brightness value that is between
        // the darkest and brightest pixel in the image.
        double minVal, maxVal;
        cv::minMaxLoc(m_imageGray, &minVal, &maxVal);
        double thresholdValue = minVal + (maxVal - minVal) * 0.8;
        cv::threshold(m_imageGray, m_thresholdImage, thresholdValue, 255, CV_THRESH_BINARY);

        // Construct a blob detector and find the blobs in the image.
        // XXX Make it so we don't have to have a blown-out image to track.
        // If the light is dimmer in the simulated image, so the brightest
        // pixel is not 255 saturated across the blobs, we don't find any
        // blobs.
        // TODO: Determine the maximum size of a trackable blob by seeing
        // when we're so close that we can't view at least four in the
        // camera.
        cv::SimpleBlobDetector::Params params;
        params.filterByColor = true;    // Look for bright blobs
        params.blobColor = static_cast<uchar>(maxVal);
        params.filterByInertia = true;  // Look for non-elongated blobs
        params.minInertiaRatio = 0.5;
        params.maxInertiaRatio = 1.0;
        cv::SimpleBlobDetector detector(params);
        std::vector<cv::KeyPoint> foundKeyPoints;
        detector.detect(m_imageGray, foundKeyPoints);

        // TODO: Consider computing the center of mass of a dilated bounding
        // rectangle around each keypoint to produce a more precise subpixel
        // localization of each LED.  The moments() function may be helpful
        // with this.

        // TODO: Estimate the summed brightness of each blob so that we can
        // detect when they are getting brighter and dimmer.  Pass this as
        // the brightness parameter to the Led class when adding a new one
        // or augmenting with a new frame.

        // We allow multiple sets of LEDs, each corresponding to a different
        // sensor, to be located in the same image.  We construct a new set
        // of LEDs for each and try to find them.  It is assumed that they all
        // have unique ID patterns across all sensors.
        for (size_t sensor = 0; sensor < m_identifiers.size(); sensor++) {
            osvrPose3SetIdentity(&m_pose);
            std::vector<cv::KeyPoint> keyPoints = foundKeyPoints;

            // Locate the closest blob from this frame to each LED found
            // in the previous frame.  If it is close enough to the nearest neighbor from last
            // time, we assume that it is the same LED and update it.  If not, we
            // delete the LED from the list.  Once we have matched a blob to an
            // LED, we remove it from the list.  If there are any blobs leftover,
            // we create new LEDs from them.
            // TODO: Include motion estimate based on Kalman filter along with
            // model of the projection once we have one built.  Note that this will
            // require handling the lens distortion appropriately.
            std::list<osvr::vbtracker::Led>::iterator led = m_led_groups[sensor].begin();
            while (led != m_led_groups[sensor].end()) {
                double TODO_BLOB_MOVE_THRESHOLD = 10;
                std::vector<cv::KeyPoint>::iterator nearest;
                nearest = led->nearest(keyPoints, TODO_BLOB_MOVE_THRESHOLD);
                if (nearest == keyPoints.end()) {
                    // We have no blob corresponding to this LED, so we need
                    // to delete this LED.
                    led = m_led_groups[sensor].erase(led);
                }
                else {
                    // Update the values in this LED and then go on to the
                    // next one.  Remove this blob from the list of potential
                    // matches.
                    led->addMeasurement(nearest->pt, nearest->size);
                    keyPoints.erase(nearest);
                    led++;
                }
            }
            // If we have any blobs that have not been associated with an
            // LED, then we add a new LED for each of them.
            //std::cout << "Had " << Leds.size() << " LEDs, " << keyPoints.size() << " new ones available" << std::endl;
            while (keyPoints.size() > 0) {
                osvr::vbtracker::Led newLed(m_identifiers[sensor],
                    keyPoints.begin()->pt, keyPoints.begin()->size);
                m_led_groups[sensor].push_back(newLed);
                keyPoints.erase(keyPoints.begin());
            }

            //==================================================================
            // Compute the pose of the HMD w.r.t. the camera frame of reference.
            // TODO: Keep track of whether we already have a good pose and, if so,
            // have the algorithm initialize using it so we do less work on average.
            bool gotPose = false;
            if (m_estimators[sensor]) {
                OSVR_PoseState pose;
                if (m_estimators[sensor]->EstimatePoseFromLeds(m_led_groups[sensor], pose)) {
                    m_pose = pose;
                    gotPose = true;
                }
            }

#ifdef VBHMD_DEBUG
            // Draw detected blobs as red circles.
            cv::drawKeypoints(m_frame, keyPoints, m_imageWithBlobs,
                cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

            // Label the keypoints with their IDs.
            for (led = m_led_groups[sensor].begin(); led != m_led_groups[sensor].end(); led++) {
                std::ostringstream label;
                int id = led->getID();
                if (id >= 0) { id++; } //Print 1-based LED ID for actual LEDs
                label << id;
                cv::Point where = led->getLocation();
                where.x += 1;
                where.y += 1;
                cv::putText(m_imageWithBlobs, label.str(), where,
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
            }

            // If we have a transform, reproject all of the points from the
            // model space (the LED locations) back into the image and
            // display them on the blob image in green.
            if (gotPose) {
                std::vector<cv::Point2f> imagePoints;
                m_estimators[sensor]->ProjectBeaconsToImage(imagePoints);
                for (int i = 0; i < imagePoints.size(); i++) {
                    std::ostringstream label;
                    int id = i;
                    if (id >= 0) { id++; } //Print 1-based LED ID for actual LEDs
                    label << id;   // Print 1-based LED ID
                    cv::Point where = imagePoints[i];
                    where.x += 1;
                    where.y += 1;
                    cv::putText(m_imageWithBlobs, label.str(), where,
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0));
                }
            }

            // Pick which image to show and show it.
            if (m_frame.data) {
                std::ostringstream windowName;
                windowName << "Sensor" << sensor;
                cv::imshow(windowName.str().c_str(), *m_shownImage);
                int key = cv::waitKey(1);
                switch (key) {
                case 'i':
                    // Show the input image.
                    m_shownImage = &m_frame;
                    break;

                case 't':
                    // Show the thresholded image.
                    m_shownImage = &m_thresholdImage;
                    break;

                case 'b':
                    // Show the blob image.
                    m_shownImage = &m_imageWithBlobs;
                    break;
                }
            }

            // Report the pose, if we got one
            if (gotPose) {
                std::cout << "Pos (sensor " << sensor << "): "
                    << m_pose.translation.data[0] << ", "
                    << m_pose.translation.data[1] << ", "
                    << m_pose.translation.data[2] << std::endl;
            }
#endif

            //==================================================================
            /// Report the new pose, time-stamped with the time we
            // received the image from the camera.
            osvrDeviceTrackerSendPoseTimestamped(m_dev, m_tracker,
                &m_pose, static_cast<OSVR_ChannelCount>(sensor), &timestamp);
        }
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_TrackerDeviceInterface m_tracker;
#ifdef VBHMD_FAKE_IMAGES
    std::vector<cv::Mat> m_images;
    size_t m_currentImage;
#else
    cv::VideoCapture m_camera;
#endif
    int m_channel;
    cv::Mat m_frame;
    cv::Mat m_imageGray;
    cv::Mat m_thresholdImage;
    cv::Mat m_imageWithBlobs;
#ifdef VBHMD_DEBUG
    cv::Mat *m_shownImage = &m_imageWithBlobs;
#endif

    // What type of HMD are we tracking?
    enum { Unknown, OSVRHDK, OculusDK2, Fake } m_type;

    // Structures needed to do the tracking.
    std::vector<osvr::vbtracker::LedIdentifier *> m_identifiers;
    std::vector<std::list<osvr::vbtracker::Led> > m_led_groups;
    std::vector<osvr::vbtracker::BeaconBasedPoseEstimator *> m_estimators;

    // In case we are using a DK2, we need a pointer to one.
    std::unique_ptr<osvr::oculus_dk2::Oculus_DK2_HID> m_dk2;

    // The pose that we report
    OSVR_PoseState m_pose;
};

class HardwareDetection {
  public:
    HardwareDetection() : m_found(false) {}

    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx) {
        if (m_found) {
            return OSVR_RETURN_SUCCESS;
        }

#ifndef VBHMD_FAKE_IMAGES
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
#endif
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

