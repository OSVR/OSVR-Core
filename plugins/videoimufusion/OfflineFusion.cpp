/** @file
    @brief Implementation

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

#ifdef OSVR_FPE
#include <FPExceptionEnabler.h>
#endif

// Internal Includes
#include "VideoIMUFusion.h"
#include <osvr/Util/EigenInterop.h>
#include <osvr/Common/JSONEigen.h>
#include <osvr/Common/JSONTimestamp.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <string>
#include <iostream>
#include <fstream>

namespace ei = osvr::util::eigen_interop;

static const auto IMU_PATH =
    "/com_osvr_Multiserver/OSVRHackerDevKit0/semantic/hmd";
static const auto VIDEO_PATH =
    "/com_osvr_VideoBasedHMDTracker/TrackedCamera0_0/semantic/hmd/front";
void processReports(Json::Value const &log) {
    VideoIMUFusion fusion;
    auto reportNumber = std::size_t{0};
    bool gotRunning = false;
    auto firstRunningReport = std::size_t{0};
    bool gotOri = false;
    OSVR_OrientationReport ori;
    for (auto &report : log) {
        std::cout << "\n***********************\nReport #" << reportNumber;
        if (gotRunning) {
            std::cout << " (report #" << (reportNumber - firstRunningReport)
                      << " in running state)";
        }
        auto timestamp = osvr::common::timevalueFromJson(report["timestamp"]);
        Eigen::Quaterniond quat =
            osvr::common::quatFromJson(report["rotation"]);
        auto &path = report["path"];
        if (path == IMU_PATH) {
            std::cout << " - IMU" << std::endl;
            gotOri = true;
            ei::map(ori.rotation) = quat;
            fusion.handleIMUData(timestamp, ori);
        } else if (path == VIDEO_PATH) {
            std::cout << " - Video-based tracker" << std::endl;
            OSVR_PoseReport pose;
            ei::map(pose.pose).translation() =
                osvr::common::vec3FromJson(report["translation"]);
            ei::map(pose.pose).rotation() = quat;
            if (fusion.running()) {
                fusion.handleVideoTrackerDataWhileRunning(timestamp, pose);
            } else {
                if (!gotOri) {
                    std::cout
                        << "Video data before IMU data, skipping this one."
                        << std::endl;
                    continue;
                }
                fusion.handleVideoTrackerDataDuringStartup(timestamp, pose,
                                                           ori.rotation);
            }
        } else {
            std::cerr << "Unrecognized path: " << path.toStyledString()
                      << std::endl;
            throw std::runtime_error("Unrecognized path: " +
                                     path.toStyledString());
        }
        if (fusion.running()) {
            if (!gotRunning) {
                gotRunning = true;
                firstRunningReport = reportNumber;
                std::cout << "---- Fusion just switched to running state!"
                          << std::endl;
            }
            std::cout << "Error Covariance:\n"
                      << fusion.getErrorCovariance().diagonal() << std::endl;
            if ((fusion.getErrorCovariance().diagonal().array() < 0.).any()) {
                std::cout << std::flush;
                std::cerr << std::flush
                          << "Got a negative variance (diagonal of state "
                             "covariance matrix) - bailing out!"
                          << std::endl;
                throw std::runtime_error("Negative variance!");
            }
        }
        reportNumber++;
    }
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Must pass the path to the input file!" << std::endl;
        return -1;
    }
    Json::Value log;
    {
        auto fn = std::string{argv[1]};
        std::ifstream dataFile(fn);
        if (!dataFile) {
            std::cerr << "Couldn't open data file '" << fn << "''" << std::endl;
            return -1;
        }
        Json::Reader reader;
        if (!reader.parse(dataFile, log)) {
            std::cerr << "Couldn't parse data file '" << fn
                      << "' as JSON! Error(s): "
                      << reader.getFormattedErrorMessages() << std::endl;
            return -1;
        }
    }
    try {
        processReports(log);
    } catch (std::exception const &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -2;
    }
    std::cout << "Reached end of log!" << std::endl;
    return 0;
}
