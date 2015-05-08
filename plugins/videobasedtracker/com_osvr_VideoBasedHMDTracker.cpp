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
#include <sstream>
#include <memory>

#define VBHMD_DEBUG

// Anonymous namespace to avoid symbol collision
namespace {

class VideoBasedHMDTracker : boost::noncopyable {
  public:
    VideoBasedHMDTracker(OSVR_PluginRegContext ctx, int cameraNum = 0, int channel = 0)
    {
        // Initialize things from parameters and from defaults.  Do it here rather than
        // in an initialization list so that we're independent of member order declaration.
        m_camera = cameraNum;
        m_channel = channel;
        m_type = Unknown;
        m_dk2 = nullptr;
        m_estimator = nullptr;

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
            // TODO: Fill these in when they are known
            m_identifier = nullptr;
            m_estimator = nullptr;

            // Set Oculus' camera capture parameters as described
            // in Oliver Kreylos' OculusRiftDK2VideoDevice.cpp program.  Thank you for
            // him for sharing this with us, used with permission.
            if (m_type == OculusDK2) {
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
                double fx = 300.0;
                double fy = fx;
                std::vector< std::vector<double> > m;
                m.push_back({  fx, 0.0,  cx });
                m.push_back({ 0.0,  fy,  cy });
                m.push_back({ 0.0, 0.0, 1.0 });
                std::vector<double> d;
                d.push_back(0); d.push_back(0); d.push_back(0); d.push_back(0); d.push_back(0);
                m_identifier = new osvr::vbtracker::OsvrHdkLedIdentifier();
                m_estimator = new osvr::vbtracker::BeaconBasedPoseEstimator(m, d);
            }
            break;

        default:    // Also handles the "Unknown" case.
            // We've already got a NULL identifier and estimator, so nothing to do.
            break;
        }
    }

    ~VideoBasedHMDTracker() {
        // It is okay to delete NULL pointers, so we don't check here.
        delete m_estimator;
        delete m_identifier;
    }

    OSVR_ReturnCode update()
    {
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
        cv::cvtColor(m_frame, m_imageGray, CV_RGB2GRAY);

        //================================================================
        // Tracking the points

        // Threshold the image based on the brightness value that is halfway between
        // the darkest and brightest pixel in the image.
        double minVal, maxVal;
        cv::minMaxLoc(m_imageGray, &minVal, &maxVal);
        double thresholdValue = minVal + (maxVal - minVal) * 0.8;
        cv::threshold(m_imageGray, m_thresholdImage, thresholdValue, 255, CV_THRESH_BINARY);

        // Construct a blob detector and find the blobs in the image.
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
        std::vector<cv::KeyPoint> keyPoints;
        detector.detect(m_imageGray, keyPoints);

        // Draw detected blobs as red circles.
        cv::drawKeypoints(m_frame, keyPoints, m_imageWithBlobs,
            cv::Scalar(0, 0, 255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        //if (keyPoints.size() > 0) { std::cout << "First keypoint location: " << keyPoints[0].pt.x << ", " << keyPoints[0].pt.y << std::endl; }

        // TODO: Consider computing the center of mass of a dilated bounding
        // rectangle around each keypoint to produce a more precise subpixel
        // localization of each LED.  The moments() function may be helpful
        // with this.

        // TODO: Estimate the summed brightness of each blob so that we can
        // detect when they are getting brighter and dimmer.  Pass this as
        // the brightness parameter to the Led class when adding a new one
        // or augmenting with a new frame.

        // Locate the closest blob from this frame to each LED found
        // in the previous frame.  If it is close enough to the nearest neighbor from last
        // time, we assume that it is the same LED and update it.  If not, we
        // delete the LED from the list.  Once we have matched a blob to an
        // LED, we remove it from the list.  If there are any blobs leftover,
        // we create new LEDs from them.
        // TODO: Include motion estimate based on Kalman filter along with
        // model of the projection once we have one built.  Note that this will
        // require handling the lens distortion appropriately.
        std::list<osvr::vbtracker::Led>::iterator led = m_leds.begin();
        while (led != m_leds.end()) {
            double TODO_BLOB_MOVE_THRESHOLD = 10;
            std::vector<cv::KeyPoint>::iterator nearest;
            nearest = led->nearest(keyPoints, TODO_BLOB_MOVE_THRESHOLD);
            if (nearest == keyPoints.end()) {
                // We have no blob corresponding to this LED, so we need
                // to delete this LED.
                led = m_leds.erase(led);
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
            osvr::vbtracker::Led newLed(m_identifier,
                keyPoints.begin()->pt, keyPoints.begin()->size);
            m_leds.push_back(newLed);
            keyPoints.erase(keyPoints.begin());
        }

        // Label the keypoints with their IDs.
        for (led = m_leds.begin(); led != m_leds.end(); led++) {
            std::ostringstream label;
            label << led->getID();
            cv::Point where = led->getLocation();
            where.x += 1;
            where.y += 1;
            cv::putText(m_imageWithBlobs, label.str(), where,
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255));
        }

        //==================================================================
        // Compute the pose of the HMD w.r.t. the camera frame of reference.
        OSVR_PoseState pose;
        osvrPose3SetIdentity(&pose);
        // XXX Compute pose here.

#ifdef VBHMD_DEBUG
        if (m_camera.isOpened()) {
            cv::imshow("Debug window", m_frame);
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
#endif

        //==================================================================
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
    cv::Mat m_imageGray;
    cv::Mat m_thresholdImage;
    cv::Mat m_imageWithBlobs;
#ifdef VBHMD_DEBUG
    cv::Mat *m_shownImage = &m_frame;
#endif

    // What type of HMD are we tracking?
    enum { Unknown, OSVRHDK, OculusDK2 } m_type;

    // Structures needed to do the tracking.
    osvr::vbtracker::LedIdentifier *m_identifier;
    std::list<osvr::vbtracker::Led> m_leds;
    osvr::vbtracker::BeaconBasedPoseEstimator *m_estimator;

    // In case we are using a DK2, we need a pointer to one.
    std::unique_ptr<osvr::oculus_dk2::Oculus_DK2_HID> m_dk2;
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

