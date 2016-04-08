/** @file
    @brief Implementation

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

// Internal Includes
#include "CSVTools.h"
#include "ConfigParams.h"
#include "LedMeasurement.h"
#include "MakeHDKTrackingSystem.h"
#include "TrackedBodyTarget.h"
#include "newuoa.h"
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
#include <fstream>
#include <sstream>

using osvr::util::time::TimeValue;
static const cv::Size IMAGE_SIZE = {640, 480};

/// Friendlier wrapper around newuoa
template <typename Function, typename Vec>
inline double ei_newuoa(long npt, Vec &x, std::pair<double, double> rho,
                        long maxfun, Function &&f) {

    double rhoBeg, rhoEnd;
    std::tie(rhoBeg, rhoEnd) = rho;
    if (rhoEnd > rhoBeg) {
        std::swap(rhoBeg, rhoEnd);
    }
    long n = x.size();
    auto workingSpaceNeeded = (npt + 13) * (npt + n) + 3 * n * (n + 3) / 2;
    Eigen::VectorXd workingSpace(workingSpaceNeeded);
    return newuoa(std::forward<Function>(f), n, npt, x.data(), rhoBeg, rhoEnd,
                  maxfun, workingSpace.data());
}

using ParamVec = Eigen::Vector4d;

namespace osvr {
namespace vbtracker {

    void updateConfigFromVec(ConfigParams &params, ParamVec const &paramVec) {
        /// positional noise
        params.processNoiseAutocorrelation[0] =
            params.processNoiseAutocorrelation[1] =
                params.processNoiseAutocorrelation[2] = paramVec[0];
        /// rotational noise
        params.processNoiseAutocorrelation[3] =
            params.processNoiseAutocorrelation[4] =
                params.processNoiseAutocorrelation[5] = paramVec[1];

        params.beaconProcessNoise = paramVec[2];

        params.measurementVarianceScaleFactor = paramVec[3];
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
            //std::cout << strField.beginPos() << ":" << strField.virtualEndPos() << std::endl;
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
            //std::cout << "Done with iterate fields" << std::endl;
            if (newRow->ok) {
                std::cout << "Row has " << newRow->measurements.size() << " blobs" << std::endl;
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

    ParamVec runOptimizer(std::string const &fn) {
        // initial values.
        ParamVec x = {4.14e-6, 1e-2, 0, 5e-2};
        auto npt = x.size() * 2; // who knows?

        auto ret = ei_newuoa(
            npt, x, {1e-8, 1e-4}, 10, [&](long n, double *x) -> double {
                using namespace osvr::vbtracker;
                ConfigParams params;
                updateConfigFromVec(params, ParamVec::Map(x));
                auto system = makeHDKTrackingSystem(params);
                auto &target =
                    *(system->getBody(BodyId(0)).getTarget(TargetId(0)));

                /// @todo

                return 0;
            });

        std::cout << "Optimizer returned " << ret
                  << " and these parameter values:" << std::endl;
        std::cout << x.transpose() << std::endl;
        return x;
    }
}
}

int main() {
    //osvr::vbtracker::runOptimizer("augmented-blobs.csv");
    osvr::vbtracker::loadData("augmented-blobs.csv");
    std::cin.ignore();
    return 0;
}
