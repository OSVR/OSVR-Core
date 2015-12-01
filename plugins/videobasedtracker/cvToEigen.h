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

#ifndef INCLUDED_cvToEigen_h_GUID_3A0994D8_A408_44B1_411A_C86F3E9332AB
#define INCLUDED_cvToEigen_h_GUID_3A0994D8_A408_44B1_411A_C86F3E9332AB

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/eigen.hpp>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    inline Eigen::Quaterniond cvRotVecToQuat(cv::Mat const &vec) {
        // Inspection of the OpenCV source suggests that the rotation vector
        // manipulated by the Rodrigues function is of the type where the
        // magnitude is the angle, and the normalized vector is the axis.
        // However, converting this way resulted in faulty (incredibly small)
        // rotations, so the roundabout way of using the Rodrigues OpenCV
        // function then converting the matrix to a quaternion was used instead.
        cv::Mat rot;
        cv::Rodrigues(vec, rot);
        Eigen::Matrix3d rotMat;
        cv::cv2eigen(rot, rotMat);
        return Eigen::Quaterniond(rotMat);
    }

    inline cv::Mat eiQuatToRotVec(Eigen::Quaterniond const &q) {
        cv::Mat rotMatrix;
        Eigen::Matrix3d eiRotMatrix = q.toRotationMatrix();
        cv::eigen2cv(eiRotMatrix, rotMatrix);
        cv::Mat rot;
        cv::Rodrigues(rotMatrix, rot);
        return rot;
    }

    inline Eigen::Vector3f cvToVector(cv::Point3f const &point) {
        return Eigen::Vector3f(point.x, point.y, point.z);
    }

    inline Eigen::Vector2f cvToVector(cv::Point2f const &point) {
        return Eigen::Vector2f(point.x, point.y);
    }

    inline Eigen::Vector3d cvToVector3d(cv::InputArray const &vec) {
        cv::Mat src = vec.getMat();
        CV_Assert(src.type() == CV_64FC1);
        Eigen::Vector3d ret;
        cv::cv2eigen(src, ret);
        return ret;
    }
    inline cv::Point3f vec3dToCVPoint3f(Eigen::Vector3d const &vec) {
        return cv::Point3f(vec.x(), vec.y(), vec.z());
    }

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_cvToEigen_h_GUID_3A0994D8_A408_44B1_411A_C86F3E9332AB
