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

#ifndef INCLUDED_TrackerSensorInfo_h_GUID_AEDB8ECA_7CAC_4A84_ACAB_CF05A681B260
#define INCLUDED_TrackerSensorInfo_h_GUID_AEDB8ECA_7CAC_4A84_ACAB_CF05A681B260

// Internal Includes
#include <osvr/Common/OriginalSource.h>
#include <osvr/Common/PathElementTypes.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    /// fully-resolved data about a tracker sensor. Defaults per the schema.
    struct TrackerSensorInfo {
        bool reportsPosition = true;
        bool reportsOrientation = true;
        bool reportsLinearVelocity = false;
        bool reportsAngularVelocity = false;
        bool reportsLinearAcceleration = false;
        bool reportsAngularAcceleration = false;
    };

    /// Given a fully-parsed tracker source, determines what messages and
    /// subsets thereof it reports.
    inline TrackerSensorInfo
    getTrackerSensorInfo(OriginalSource const &source) {
        BOOST_ASSERT_MSG(
            source.isResolved(),
            "Can't pass an unresolved source to getTrackerSensorInfo!");

        BOOST_ASSERT_MSG(
            source.getInterfaceName() == "tracker",
            "Can't pass a non-tracker source to getTrackerSensorInfo!");

        auto &devDescriptor =
            source.getDeviceElement().getDescriptor()["interfaces"]["tracker"];
        /// Get the named value from the object, using the given value reference
        /// as both a default value and a destination.
        auto getBool = [](Json::Value const &root, const char key[],
                          bool &val) { val = root.get(key, val).asBool(); };
        auto setupFromJson = [&getBool](TrackerSensorInfo &info,
                                        Json::Value const &root) {
            if (root.isNull()) {
                return;
            }
            getBool(root, "position", info.reportsPosition);
            getBool(root, "orientation", info.reportsOrientation);
            getBool(root, "linearVelocity", info.reportsLinearVelocity);
            getBool(root, "angularVelocity", info.reportsAngularVelocity);
            getBool(root, "linearAcceleration", info.reportsLinearAcceleration);
            getBool(root, "angularAcceleration",
                    info.reportsAngularAcceleration);
        };
        TrackerSensorInfo ret;

        /// First see if there are device-global properties
        setupFromJson(ret, devDescriptor);

        /// Now check for a traits array - overrides device-global propertes
        auto sensor = source.getSensorNumber();
        if (sensor) {
            setupFromJson(ret, devDescriptor["traits"][*sensor]);
        }

        /// @todo Apply modifications based on transforms.
        /// translation causes report of position in orientation trackers, etc.
        return ret;
    }
} // namespace common
} // namespace osvr

#endif // INCLUDED_TrackerSensorInfo_h_GUID_AEDB8ECA_7CAC_4A84_ACAB_CF05A681B260
