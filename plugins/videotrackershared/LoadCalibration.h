/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_LoadCalibration_h_GUID_50FCDBB2_367E_4589_A034_FCFDF13C8714
#define INCLUDED_LoadCalibration_h_GUID_50FCDBB2_367E_4589_A034_FCFDF13C8714

// Internal Includes
// - none

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>
#include <opencv2/core/core.hpp>

// Standard includes
#include <fstream>
#include <iostream>
#include <vector>

namespace osvr {
namespace vbtracker {

    /// @name Helper functions for loading calibration files
    /// @{
    inline cv::Point3f parsePoint(Json::Value const &jsonArray) {
        return cv::Point3f(jsonArray[0].asFloat(), jsonArray[1].asFloat(),
                           jsonArray[2].asFloat());
    }

    inline std::vector<cv::Point3f>
    parseArrayOfPoints(Json::Value const &jsonArray) {
        /// in case of error, we just return an empty array.
        std::vector<cv::Point3f> ret;
        if (!jsonArray.isArray()) {
            return ret;
        }
        for (auto &entry : jsonArray) {

            if (!entry.isArray() || entry.size() != 3) {
                ret.clear();
                return ret;
            }
            ret.emplace_back(parsePoint(entry));
        }
        return ret;
    }

    inline std::vector<cv::Point3f>
    tryLoadingArrayOfPointsFromFile(std::string const &filename) {
        std::vector<cv::Point3f> ret;
        if (filename.empty()) {
            return ret;
        }
        Json::Value root;
        {
            std::ifstream calibfile(filename);
            if (!calibfile.good()) {
                return ret;
            }
            Json::Reader reader;
            if (!reader.parse(calibfile, root)) {
                return ret;
            }
        }
        ret = parseArrayOfPoints(root);
        return ret;
    }
    /// @}

    /// Loading a calibration file means our beacon locations are better
    /// known than we might otherwise expect.
    static const double BEACON_AUTOCALIB_ERROR_SCALE_IF_CALIBRATED = 0.1;

    namespace messages {
        inline void loadedCalibFileSuccessfully(std::string const &filename) {
            std::cout << "Video-based tracker: Successfully loaded "
                         "beacon calibration file "
                      << filename << std::endl;
        }

        inline void
        calibFileSpecifiedButNotLoaded(std::string const &filename) {
            std::cout << "Video-based tracker: NOTE: Beacon calibration "
                         "filename "
                      << filename
                      << " was specified, but not found or could not "
                         "be loaded. This is not an error: This may just mean "
                         "you have not run the optional beacon "
                         "pre-calibration step."
                      << std::endl;
        }
    } // namespace messages

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_LoadCalibration_h_GUID_50FCDBB2_367E_4589_A034_FCFDF13C8714
