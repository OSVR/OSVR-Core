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

#ifndef INCLUDED_LoadRows_h_GUID_7FC43F97_8922_448D_7CE8_2D9EAB669BBD
#define INCLUDED_LoadRows_h_GUID_7FC43F97_8922_448D_7CE8_2D9EAB669BBD

// Internal Includes
#include "CSVTools.h"
#include <ImageProcessing.h>
#include <LedMeasurement.h>
#include <MakeHDKTrackingSystem.h>

#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <opencv2/core/core.hpp>

// Standard includes
#include <cstddef>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace osvr {
namespace vbtracker {

    using osvr::util::time::TimeValue;
    static const cv::Size IMAGE_SIZE = {640, 480};

    inline cv::Mat const &getGray() {
        static const cv::Mat instance = cv::Mat(IMAGE_SIZE, CV_8UC1);
        return instance;
    }

    inline cv::Mat const &getColor() {
        static const cv::Mat instance = cv::Mat(IMAGE_SIZE, CV_8UC3);
        return instance;
    }

    struct TimestampedMeasurements {
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        TimeValue tv;
        Eigen::Vector3d xlate;
        Eigen::Quaterniond rot;
        LedMeasurementVec measurements;
        bool ok = false;
    };

    class LoadRow {
      public:
        LoadRow(csvtools::FieldParserHelper &helper,
                TimestampedMeasurements &row)
            : helper_(helper), row_(row) {}
        ~LoadRow() {
            if (!measurementPieces_.empty()) {
                std::cerr << "Leftover measurement pieces on loadrow "
                             "destruction, suggests a parsing error!"
                          << std::endl;
            }
        }
        bool operator()(std::string const &line, std::size_t beginPos,
                        std::size_t endPos) {
            field_++;
            csvtools::StringField strField(line, beginPos, endPos);
            // std::cout << strField.beginPos() << ":" <<
            // strField.virtualEndPos() << std::endl;
            if (field_ <= 3) {
                // refx/y/z
                bool success = false;
                double val;
                std::tie(success, val) = helper_.getFieldAs<double>(strField);
                if (!success) {
                    return false;
                }
                row_.xlate[field_] = val;
                return true;
            }
            if (field_ <= 7) {
                // refqw/qx/qy/qz
                bool success = false;
                double val;
                std::tie(success, val) = helper_.getFieldAs<double>(strField);
                if (!success) {
                    return false;
                }
                switch (field_) {
                case 4:
                    row_.rot.w() = val;
                    break;
                case 5:
                    row_.rot.x() = val;
                    break;
                case 6:
                    row_.rot.y() = val;
                    break;
                case 7:
                    row_.rot.z() = val;
                    break;
                }
                return true;
            }
            if (field_ == 8) {
                // sec
                auto success = helper_.getField(strField, row_.tv.seconds);
                return success;
            }
            if (field_ == 9) {
                // usec
                auto success = helper_.getField(strField, row_.tv.microseconds);
                if (success) {
                    row_.ok = true;
                }
                return success;
            }
            // Now, we are looping through x, y, size for every blob.

            bool success = false;
            float val;
            std::tie(success, val) = helper_.getFieldAs<float>(strField);
            if (!success) {
                return false;
            }
            measurementPieces_.push_back(val);
            if (measurementPieces_.size() == 3) {
                // that's a new LED!
                row_.measurements.emplace_back(
                    measurementPieces_[0], measurementPieces_[1],
                    measurementPieces_[2], IMAGE_SIZE);
                measurementPieces_.clear();
            }
            return true;
        }

      private:
        csvtools::FieldParserHelper &helper_;
        TimestampedMeasurements &row_;
        std::size_t field_ = 0;
        std::vector<float> measurementPieces_;
    };

    inline std::vector<std::unique_ptr<TimestampedMeasurements>>
    loadData(std::string const &fn) {
        std::vector<std::unique_ptr<TimestampedMeasurements>> ret;
        std::ifstream csvFile(fn);
        if (!csvFile) {
            std::cerr << "Could not open csvFile " << fn << std::endl;
            return ret;
        }
        {
            auto headerRow = csvtools::getCleanLine(csvFile);
            if (headerRow.empty() || !csvFile) {
                /// @todo this is pretty crude precondition handling, but
                /// probably good enough for now.
                std::cerr << "Header row was empty, that's not a good sign!"
                          << std::endl;
                return ret;
            }
        }
        csvtools::FieldParserHelper helper;

        std::string dataLine = csvtools::getCleanLine(csvFile);
        while (csvFile) {
            std::unique_ptr<TimestampedMeasurements> newRow(
                new TimestampedMeasurements);
            csvtools::iterateFields(LoadRow(helper, *newRow), dataLine);
            // std::cout << "Done with iterate fields" << std::endl;
            if (newRow->ok) {
#if 0
                std::cout << "Row has " << newRow->measurements.size()
                          << " blobs" << std::endl;
#endif
                ret.emplace_back(std::move(newRow));
            } else {
                std::cerr << "Something went wrong parsing that row: "
                          << dataLine << std::endl;
            }

            dataLine = csvtools::getCleanLine(csvFile);
        }
        std::cout << "Total of " << ret.size() << " rows" << std::endl;
        return ret;
    }

    ImageOutputDataPtr
    makeImageOutputDataFromRow(TimestampedMeasurements const &row,
                               CameraParameters const &camParams) {
        ImageOutputDataPtr ret(new ImageProcessingOutput);
        ret->tv = row.tv;
        ret->ledMeasurements = row.measurements;
        ret->camParams = camParams;
        ret->frame = getColor();
        ret->frameGray = getGray();
        return ret;
    }

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_LoadRows_h_GUID_7FC43F97_8922_448D_7CE8_2D9EAB669BBD
