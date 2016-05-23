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
#include "QuatToEuler.h"
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
#include <sstream>
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
            params_.streamBeaconDebugInfo = true;

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

        /// To get a time that matches the timestamp
        std::size_t getFrameCount() const { return frame_ + 1; }

        using FrameTimeUnit = std::chrono::microseconds;

      private:
        /// Substitute for performInitialImageProcessing that lets us get more
        /// of the innards: sets rawMeasurements_, undistortedMeasurements_, and
        /// leaves useful state in extractor_.
        ImageOutputDataPtr imageProc(cv::Mat const &frame);
        void logRow();

        /// @name Constants
        /// @{
        const int fps_ = 100;
        const FrameTimeUnit frameTime_ =
            std::chrono::duration_cast<FrameTimeUnit>(std::chrono::seconds(1)) /
            fps_;

        const BodyId bodyIdOfInterest = BodyId(0);
        const TargetId targetIdOfInterest = TargetId(0);
        /// @}

        /// Return a string with decimal seconds in it, that has never touched
        /// floating point.
        std::string carefullyFormatElapsedTime() const;

        /// Get the whole number of seconds passed in the simulation.
        std::chrono::seconds getSeconds() const;

        /// Get whatever time passed that's left after subtracting all whole
        /// seconds (value will be less than a second).
        FrameTimeUnit getFractionalRemainder() const;

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
        bool everHadPose_ = false;
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
        // time as the two-part time value
        row << cellGroup(currentTime_);
        // time as a pristinely-formatted decimal number of seconds
        row << cell("Time", carefullyFormatElapsedTime());

        auto hasPose = body_->hasPoseEstimate();
        everHadPose_ |= hasPose;

        row << cell("TrackerDropped", hasPose ? "" : "0");

        if (hasPose) {
            Eigen::Quaterniond quat = body_->getState().getQuaternion();
            Eigen::Vector3d xlate = body_->getState().position();
            row << cellGroup(xlate) << cellGroup(quat)
                << cellGroup<QuatAsEulerTag>(quat);
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

        auto getStatus = [&](TargetStatusMeasurement meas) {
            return target_->getInternalStatusMeasurement(meas);
        };
        row << cell("Rejects.Area", numArea)
            << cell("Rejects.CenterPointValue", numCenterPointValue)
            << cell("Rejects.Circularity", numCircularity)
            << cell("Rejects.Convexity", numConvexity)
            << cell("Leds", getStatus(TargetStatusMeasurement::NumUsableLeds))
            << cell("UsedLeds",
                    getStatus(TargetStatusMeasurement::NumUsedLeds));
        if (everHadPose_) {
            row << cell("PosErrorVariance",
                        getStatus(TargetStatusMeasurement::MaxPosErrorVariance))
                << cell("PosErrorVarianceBound",
                        getStatus(
                            TargetStatusMeasurement::PosErrorVarianceLimit));
        }
    }

    /// Takes a std::ratio like std::micro and counts the number of digits to
    /// the right of the decimal point we need to use.
    template <typename Rat> inline std::size_t countDigits() {
        auto denom = Rat::den;
        auto ret = std::size_t{0};
        while (denom > 1) {
            denom /= 10;
            ret++;
        }
        return ret;
    }

    inline std::string
    TrackerOfflineProcessing::carefullyFormatElapsedTime() const {
        std::ostringstream os;
        os << getSeconds().count() << ".";
        using FractionalType = decltype(getFractionalRemainder());
        using FractionalPeriod = FractionalType::period;
        static const auto numDigits = countDigits<FractionalPeriod>();
        os << std::setw(numDigits) << std::setfill('0')
           << getFractionalRemainder().count();
        return os.str();
    }

    inline std::chrono::seconds TrackerOfflineProcessing::getSeconds() const {
        return std::chrono::seconds(getFrameCount() / fps_);
    }

    inline TrackerOfflineProcessing::FrameTimeUnit
    TrackerOfflineProcessing::getFractionalRemainder() const {
        auto remainderFrames = getFrameCount() % fps_;
        auto ret = remainderFrames * frameTime_;
        BOOST_ASSERT_MSG(
            ret < std::chrono::seconds(1),
            "This is a remainder - it must be less than a second!");
        return ret;
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
    std::vector<std::string> videoNames;
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
                videoNames.push_back(arg);
            }
            return ret;
        });
        if (numVideoNames < 1) {
            std::cerr << "Must pass at least one video filename to this app!"
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

    for (auto &videoName : videoNames) {
        std::cout << "Processing input video " << videoName << std::endl;
        osvr::vbtracker::TrackerOfflineProcessing app(params);
        osvr::vbtracker::processAVI(videoName, app);

        {
            std::cout << "Processed a total of " << app.getFrameCount()
                      << " frames." << std::endl;
            auto outname = videoName + ".csv";
            std::cout << "Writing output data to: " << outname << std::endl;
            std::ofstream of(outname);
            app.outputCSV(of);
        }
        std::cout << "File finished!\n\n" << std::endl;
    }

    return 0;
}