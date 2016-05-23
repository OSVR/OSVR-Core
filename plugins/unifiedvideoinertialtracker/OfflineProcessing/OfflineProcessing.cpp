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

#define OSVR_HAVE_BOOST

// Internal Includes
#include "../ConfigParams.h"
#include "../ConfigurationParser.h"
#include "../MakeHDKTrackingSystem.h"
#include "../TrackedBodyTarget.h"
#include "CSVCellGroup.h"
#include <CameraParameters.h>
#include <EdgeHoleBasedLedExtractor.h>
#include <UndistortMeasurements.h>
#include <osvr/Util/CSV.h>
#include <osvr/Util/MiniArgsHandling.h>
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
#include <boost/algorithm/string/predicate.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Standard includes
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace osvr {
namespace vbtracker {

    namespace {
        struct BodyIdOrdering {
            bool operator()(BodyId const &lhs, BodyId const &rhs) const {
                return lhs.value() < rhs.value();
            }
        };
    } // namespace
    class TrackerOfflineProcessing {
      public:
        TrackerOfflineProcessing(ConfigParams const &initialParams)
            : camParamsDistorted_(osvr::vbtracker::getHDKCameraParameters()),
              camParams_(camParamsDistorted_.createUndistortedVariant()),
              params_(initialParams), extractor_(initialParams.extractParams) {
            params_.performingOptimization = true;
            params_.silent = true;
            params_.debug = false;

            params_.offsetToCentroid = false;
            params_.includeRearPanel = false;
            params_.imu.path = "";
            params_.imu.useOrientation = false;

            system_ = makeHDKTrackingSystem(params_);
            body_ = &(system_->getBody(bodyIdOfInterest));
            target_ = body_->getTarget(targetIdOfInterest);
        }

        void processFrame(cv::Mat const &frame);

        void outputCSV(std::ostream &os) { csv_.output(os); }

      private:
        /// Substitute for performInitialImageProcessing that lets us get more
        /// of the innards: sets rawMeasurements_, undistortedMeasurements_, and
        /// leaves useful state in extractor_.
        ImageOutputDataPtr imageProc(cv::Mat const &frame);
        void logRow();

        /// @name Constants
        /// @{
        const int fps_ = 100;
        const std::chrono::microseconds frameTime_ =
            std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::seconds(1)) /
            fps_;

        const BodyId bodyIdOfInterest = BodyId(0);
        const TargetId targetIdOfInterest = TargetId(0);
        /// @}

        const CameraParameters camParamsDistorted_;
        const CameraParameters camParams_;
        ConfigParams params_;
        EdgeHoleBasedLedExtractor extractor_;
        std::unique_ptr<TrackingSystem> system_;
        TrackedBody *body_ = nullptr;
        TrackedBodyTarget *target_ = nullptr;
        util::time::TimeValue currentTime_ = {};
        LedMeasurementVec rawMeasurements_;
        LedMeasurementVec undistortedMeasurements_;
        util::CSV csv_;
        std::size_t frame_ = 0;
    };

    void TrackerOfflineProcessing::processFrame(cv::Mat const &frame) {
        if ((frame_ % 100) == 0) {
            std::cout << "Processing frame " << frame_ << std::endl;
        }
        /// Advance the clock
        currentTime_.microseconds += frameTime_.count();
        osvrTimeValueNormalize(&currentTime_);

        /// Image processing.
        auto imageData = imageProc(frame);

        /// Hand off the image processing results
        auto indices = system_->updateBodiesFromVideoData(std::move(imageData));
        logRow();
        frame_++;
    }

    ImageOutputDataPtr
    TrackerOfflineProcessing::imageProc(cv::Mat const &frame) {
        ImageOutputDataPtr ret(new ImageProcessingOutput);
        ret->tv = currentTime_;
        ret->frame = frame;
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        ret->frameGray = gray;
        ret->camParams = camParams_; // undistorted!
        rawMeasurements_ = extractor_(gray, params_.blobParams, false);
        undistortedMeasurements_ =
            undistortLeds(rawMeasurements_, camParamsDistorted_);
        ret->ledMeasurements = undistortedMeasurements_;
        return ret;
    }

    void TrackerOfflineProcessing::logRow() {
        using namespace osvr::util;
        auto row = csv_.row();
        row << cellGroup(currentTime_)
            << cell("hasPose", body_->hasPoseEstimate() ? "TRUE" : "FALSE");
        if (body_->hasPoseEstimate()) {
            Eigen::Quaterniond quat = body_->getState().getQuaternion();
            Eigen::Vector3d xlate = body_->getState().position();
            row << cellGroup(xlate) << cellGroup(quat);
        }
        row << cell("Measurements", rawMeasurements_.size());

        std::size_t numArea = 0;
        std::size_t numCenterPointValue = 0;
        std::size_t numCircularity = 0;
        std::size_t numConvexity = 0;
        for (auto &reject : extractor_.getRejectList()) {
            RejectReason reason = std::get<1>(reject);

            switch (reason) {
            case RejectReason::Area:
                numArea++;
                break;
            case RejectReason::CenterPointValue:
                numCenterPointValue++;
                break;
            case RejectReason::Circularity:
                numCircularity++;
                break;
            case RejectReason::Convexity:
                numConvexity++;
                break;
            default:
                break;
            }
        }
        row << cell("Rejects.Area", numArea)
            << cell("Rejects.CenterPointValue", numCenterPointValue)
            << cell("Rejects.Circularity", numCircularity)
            << cell("Rejects.Convexity", numConvexity);
    }

    void processAVI(std::string const &fn, TrackerOfflineProcessing &app) {
        cv::VideoCapture capture;
        capture.open(fn);
        if (!capture.isOpened()) {
            std::cerr << "Could not open video file " << fn << std::endl;
            return;
        }
        cv::Mat frame;
        capture >> frame;
        while (capture.read(frame)) {
            app.processFrame(frame);
        }
    }

} // namespace vbtracker
} // namespace osvr

using namespace osvr::util::args;
int main(int argc, char *argv[]) {

    bool gotParams = false;
    osvr::vbtracker::ConfigParams params;
    std::string videoName;
    auto args = makeArgList(argc, argv);
    try {
        /// parse json file arguments.
        auto numJson = handle_arg(args, [&](std::string const &arg) {
            if (!boost::iends_with(arg, ".json")) {
                return false;
            }
            std::ifstream configFile(arg);
            if (!configFile) {
                std::cerr << "Tried to load " << arg
                          << " as a config file but could not open it!"
                          << std::endl;
                throw std::invalid_argument(
                    "Could not open json config file passed");
            }
            Json::Value root;
            Json::Reader reader;
            if (!reader.parse(configFile, root)) {
                std::cerr << "Could not parse " << arg << " as JSON! "
                          << reader.getFormattedErrorMessages() << std::endl;
                throw std::runtime_error(
                    "Config file could not be parsed as JSON!");
            }
            params = osvr::vbtracker::parseConfigParams(root);
            return true;
        });
        if (numJson > 1) {
            std::cerr << "At most one .json config file passed to this app!"
                      << std::endl;
            return -1;
        }

        /// Get video filename
        auto numVideoNames = handle_arg(args, [&](std::string const &arg) {
            auto ret = boost::iends_with(arg, ".avi");
            if (ret) {
                videoName = arg;
            }
            return ret;
        });
        if (numVideoNames != 1) {
            std::cerr << "Must pass exactly one video filename to this app!"
                      << std::endl;
            return -1;
        }

        if (!args.empty()) {
            std::cerr
                << "Unrecognized arguments left after parsing command line!"
                << std::endl;
            return -1;
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    osvr::vbtracker::TrackerOfflineProcessing app(params);
    osvr::vbtracker::processAVI(videoName, app);

    {
        std::cout << "Writing output data" << std::endl;
        std::ofstream of("outdata.csv");
        app.outputCSV(of);
    }
    return 0;
}