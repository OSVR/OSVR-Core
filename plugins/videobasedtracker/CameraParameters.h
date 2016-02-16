/** @file
    @brief Header

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_CameraParameters_h_GUID_5BBC6151_6C1F_44B1_14FC_34CB6BF601E9
#define INCLUDED_CameraParameters_h_GUID_5BBC6151_6C1F_44B1_14FC_34CB6BF601E9

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <opencv2/core/core.hpp>

// Standard includes
#include <initializer_list>
#include <vector>

namespace osvr {
namespace vbtracker {
    struct CameraParameters {
        /// Separate focal lengths, distortion specified.
        CameraParameters(double fx, double fy, cv::Size size,
                         std::initializer_list<double> distortionParams)
            : cameraMatrix(cv::Matx33d::eye()),
              distortionParameters(distortionParams), imageSize(size) {
            cameraMatrix(0, 0) = fx;
            cameraMatrix(1, 1) = fy;
            cameraMatrix(0, 2) = size.width / 2.;
            cameraMatrix(1, 2) = size.height / 2.;
            normalizeDistortionParameters();
        }

        /// Single focal length, distortion specified.
        CameraParameters(double focalLength, cv::Size size,
                         std::initializer_list<double> distortionParams)
            : CameraParameters(focalLength, focalLength, size,
                               distortionParams) {}

        /// The zero-distortion constructor, separate focal lengths
        CameraParameters(double fx, double fy, cv::Size size)
            : CameraParameters(fx, fy, size, {0., 0., 0., 0., 0.}) {}

        /// The zero-distortion constructor, single focal length
        CameraParameters(double focalLength = 700,
                         cv::Size size = cv::Size(640, 480))
            : CameraParameters(focalLength, focalLength, size) {}

        /// Copy-constructs, with zero distortion parameters.
        CameraParameters createUndistortedVariant() const {
            // copy
            auto ret = *this;
            // zero the distortion
            ret.distortionParameters.clear();
            ret.normalizeDistortionParameters();
            return ret;
        }

        double focalLengthX() const { return cameraMatrix(0, 0); }
        double focalLengthY() const { return cameraMatrix(1, 1); }
        double focalLength() const { return focalLengthX(); }
        double k1() const { return distortionParameters[0]; }
        double k2() const { return distortionParameters[1]; }
        double k3() const { return distortionParameters[2]; }
        cv::Point2d principalPoint() const {
            return cv::Point2d(cameraMatrix(0, 2), cameraMatrix(1, 2));
        }

        Eigen::Vector2d eiPrincipalPoint() const {
            return Eigen::Vector2d(cameraMatrix(0, 2), cameraMatrix(1, 2));
        }

        cv::Matx33d cameraMatrix;
        std::vector<double> distortionParameters;
        cv::Size imageSize;

      private:
        void normalizeDistortionParameters() {
            /// @todo handle higher order distortion?
            distortionParameters.resize(5, 0.);
        }
    };

    inline CameraParameters getSimulatedHDKCameraParameters() {

        // The fake tracker uses real LED positions and a
        // simulated camera, whose parameters we describe here.
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
        return CameraParameters(700, cv::Size(640, 480));
    }

    inline CameraParameters getHDKCameraParameters() {
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
        double fx = 452.9; // 700.0; // XXX This needs to be in pixels, not mm
        /// @todo Make it possible to read these values from the config file
        /// for the camera
        double k1 = 1.252916e-06;
        double k2 = -1.752020e-11;
        double k3 = 6.405327e-17;
        double p1 = 0;
        double p2 = 0;

        return CameraParameters(fx, cv::Size(640, 480), {k1, k2, p1, p2, k3});
    }
} // End namespace vbtracker
} // End namespace osvr
#endif // INCLUDED_CameraParameters_h_GUID_5BBC6151_6C1F_44B1_14FC_34CB6BF601E9
