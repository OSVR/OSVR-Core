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

#ifndef INCLUDED_SetupSensors_h_GUID_7F0F52BD_57B4_4ABB_0834_C61DB7C22132
#define INCLUDED_SetupSensors_h_GUID_7F0F52BD_57B4_4ABB_0834_C61DB7C22132

// Internal Includes
#include "Types.h"
#include "VideoBasedTracker.h"
#include "HDKData.h"
#include "CameraParameters.h"
#include "HDKLedIdentifierFactory.h"

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <vector>
#include <iostream>
#include <fstream>

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

    static bool frontPanelFixedBeaconShared(int id) {
        return (id == 16) || (id == 17) || (id == 19) || (id == 20);
    }

    static bool backPanelFixedBeaconShared(int) { return true; }

    namespace messages {
        inline void loadedCalibFileSuccessfully(ConfigParams const &config) {
            std::cout << "Video-based tracker: Successfully loaded "
                         "beacon calibration file "
                      << config.calibrationFile << std::endl;
        }

        inline void calibFileSpecifiedButNotLoaded(ConfigParams const &config) {
            std::cout << "Video-based tracker: NOTE: Beacon calibration "
                         "filename "
                      << config.calibrationFile
                      << " was specified, but not found or could not "
                         "be loaded. This is not an error: This may just mean "
                         "you have not yet run the optional beacon "
                         "pre-calibration step."
                      << std::endl;
        }
    } // namespace messages

    using BeaconPredicate = std::function<bool(int)>;

    inline std::size_t getNumHDKFrontPanelBeacons() {
        return OsvrHdkLedLocations_SENSOR0.size();
    }

    inline std::size_t getNumHDKRearPanelBeacons() {
        return OsvrHdkLedLocations_SENSOR1.size();
    }
    /// distance between front and back panel target origins, in mm.
    inline double
    computeDistanceBetweenPanels(double headCircumference,
                                 double headToFrontBeaconOriginDistance) {
        return headCircumference / M_PI * 10. + headToFrontBeaconOriginDistance;
    }
    inline double computeDistanceBetweenPanels(ConfigParams const &config) {
        return computeDistanceBetweenPanels(
            config.headCircumference, config.headToFrontBeaconOriginDistance);
    }
    inline void addRearPanelBeaconLocations(double distanceBetweenPanels,
                                            Point3Vector &locations) {
        // For the back panel beacons: have to rotate 180 degrees
        // about Y, which is the same as flipping sign on X and Z
        // then we must translate along Z by head diameter +
        // distance from head to front beacon origins
        for (auto &pt : OsvrHdkLedLocations_SENSOR1) {
            locations.emplace_back(-pt.x, pt.y, -pt.z - distanceBetweenPanels);
        }
    }

    inline void addRearPanelBeaconLocations(ConfigParams const &config,
                                            Point3Vector &locations) {
        addRearPanelBeaconLocations(computeDistanceBetweenPanels(config),
                                    locations);
    }

    inline void setupSensorsIncludeRearPanel(
        VideoBasedTracker &vbtracker, ConfigParams const &config,
        bool attemptToLoadCalib = true,
        BeaconPredicate &&beaconFixedPredicate = &frontPanelFixedBeaconShared) {

        Point3Vector locations = OsvrHdkLedLocations_SENSOR0;
        Vec3Vector directions = OsvrHdkLedDirections_SENSOR0;
        std::vector<double> variances = OsvrHdkLedVariances_SENSOR0;

        // Have to rotate and translate the rear beacons
        addRearPanelBeaconLocations(config, locations);
        // Add variance for each location
        variances.insert(end(variances), OsvrHdkLedLocations_SENSOR1.size(),
                         config.backPanelMeasurementError);
        // Similarly, rotate the directions.
        for (auto &vec : OsvrHdkLedDirections_SENSOR1) {
            directions.emplace_back(-vec[0], vec[1], -vec[2]);
        }
        double autocalibScale = 1;
        if (attemptToLoadCalib) {
            auto calibLocations =
                tryLoadingArrayOfPointsFromFile(config.calibrationFile);
            bool gotCalib = false;
            if (calibLocations.size() == locations.size()) {
                /// This is the right size already, just take it as it is.
                gotCalib = true;
            } else if (calibLocations.size() == getNumHDKFrontPanelBeacons()) {
                // just need to add the rear beacons to the calibration file.
                // This is actually ideal, since we can use the
                // currently-configured head size, instead of calibration-time
                // head size, to estimate the rear panel location.
                addRearPanelBeaconLocations(config, calibLocations);
                gotCalib = true;
            }

            if (gotCalib) {
                messages::loadedCalibFileSuccessfully(config);
                locations = calibLocations;
                autocalibScale = BEACON_AUTOCALIB_ERROR_SCALE_IF_CALIBRATED;

            } else if (!config.calibrationFile.empty()) {
                messages::calibFileSpecifiedButNotLoaded(config);
            }
        }

        auto camParams = getHDKCameraParameters();
        vbtracker.addSensor(createHDKUnifiedLedIdentifier(), camParams,
                            locations, directions, variances,
                            beaconFixedPredicate, 4, 0, autocalibScale);
    }

    inline void
    setupSensorsWithoutRearPanel(VideoBasedTracker &vbtracker,
                                 ConfigParams const &config,
                                 bool attemptToLoadCalib = true,
                                 BeaconPredicate &&frontBeaconFixedPredicate =
                                     &frontPanelFixedBeaconShared,
                                 BeaconPredicate &&backBeaconFixedPredicate =
                                     &backPanelFixedBeaconShared) {

        auto camParams = getHDKCameraParameters();

        bool needFrontSensor = true;
        if (attemptToLoadCalib) {
            auto calibLocations =
                tryLoadingArrayOfPointsFromFile(config.calibrationFile);
            if (calibLocations.size() == getNumHDKFrontPanelBeacons() ||
                calibLocations.size() ==
                    getNumHDKFrontPanelBeacons() +
                        getNumHDKRearPanelBeacons()) {
                // Remove any extra (rear panel) "calibrated" locations
                calibLocations.resize(getNumHDKFrontPanelBeacons());
                messages::loadedCalibFileSuccessfully(config);
                needFrontSensor = false;
                vbtracker.addSensor(
                    createHDKLedIdentifier(0), camParams, calibLocations,
                    OsvrHdkLedDirections_SENSOR0, OsvrHdkLedVariances_SENSOR0,
                    frontBeaconFixedPredicate, 6, 0,
                    BEACON_AUTOCALIB_ERROR_SCALE_IF_CALIBRATED);
            } else if (!config.calibrationFile.empty()) {
                messages::calibFileSpecifiedButNotLoaded(config);
            }
        }
        if (needFrontSensor) {
            vbtracker.addSensor(
                createHDKLedIdentifier(0), camParams,
                OsvrHdkLedLocations_SENSOR0, OsvrHdkLedDirections_SENSOR0,
                OsvrHdkLedVariances_SENSOR0, frontBeaconFixedPredicate, 6, 0);
        }
        vbtracker.addSensor(
            createHDKLedIdentifier(1), camParams, OsvrHdkLedLocations_SENSOR1,
            OsvrHdkLedDirections_SENSOR1, backBeaconFixedPredicate, 4, 0);
    }
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_SetupSensors_h_GUID_7F0F52BD_57B4_4ABB_0834_C61DB7C22132
