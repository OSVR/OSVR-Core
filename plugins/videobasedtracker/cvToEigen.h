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

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    inline Eigen::Quaterniond cvRotVecToQuat(cv::Mat const &vec) {
        // Inspection of the OpenCV source suggests that the rotation vector
        // manipulated by the Rodrigues function is of the type where the
        // magnitude is the angle, and the normalized vector is the axis.
        Eigen::Vector3d rot{vec.at<double>(0), vec.at<double>(1),
                            vec.at<double>(2)};
        auto sqNorm = rot.squaredNorm();
        if (sqNorm < DBL_EPSILON) {
            return Eigen::Quaterniond::Identity();
        }
        auto angle = std::sqrt(sqNorm);
        return Eigen::Quaterniond(Eigen::AngleAxisd(angle, rot / sqNorm));
    }
    inline Eigen::Vector3d cvFloatToVector3d(cv::InputArray const &vec) {
        cv::Mat src = vec.getMat();
        CV_Assert(src.type() == CV_32FC1);
        return Eigen::Vector3d(src.at<float>(0), src.at<float>(1),
                               src.at<float>(2));
    }
    inline Eigen::Vector3f cvToVector(cv::Point3f const &point) {
        return Eigen::Vector3f(point.x, point.y, point.z);
    }

    inline Eigen::Vector3d cvToVector3d(cv::InputArray const &vec) {
        cv::Mat src = vec.getMat();
        CV_Assert(src.type() == CV_64FC1);
        return Eigen::Vector3d(src.at<double>(0), src.at<double>(1),
                               src.at<double>(2));
    }
    inline cv::Point3f vec3dToCVPoint3f(Eigen::Vector3d const &vec) {
        return cv::Point3f(vec.x(), vec.y(), vec.z());
    }
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_cvToEigen_h_GUID_3A0994D8_A408_44B1_411A_C86F3E9332AB
