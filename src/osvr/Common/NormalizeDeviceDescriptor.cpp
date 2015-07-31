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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <boost/type_traits/remove_cv.hpp>
#include <osvr/Common/NormalizeDeviceDescriptor.h>

// Library/third-party includes
// - none

// Standard includes
#include <json/value.h>
#include <json/reader.h>
#include <boost/noncopyable.hpp>
#include <boost/variant/get.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

namespace osvr {
namespace common {

    static const char INTERFACES_KEY[] = "interfaces";
    static const char EYETRACKER_KEY[] = "eyetracker";
    static const char LOCATION2D_KEY[] = "location2D";
    static const char DIRECTION_KEY[] = "direction";
    static const char TRACKER_KEY[] = "tracker";
    static const char BUTTON_KEY[] = "button";
    static const char COUNT_KEY[] = "count";
    static const char POSITION_KEY[] = "position";
    static const char ORIENTATION_KEY[] = "orientation";
    static const char BOUNDED_KEY[] = "bounded";

    /// @todo when appending interfaces you might encounter that there are
    /// independent interfaces that are same as subinterfaces
    /// for example, eyetracker device and tracker device plugin
    /// eyetracker breaks down into 4 interfaces including tracker
    /// so to avoid replacing tracker data, we need to safely merge them
    void mergeIdenticalInterfaces(Json::Value &existingIface,
                                  Json::Value &newIface,
                                  std::string const &detail) {}

    /// @brief appends json value for a given string
    void appendCurrentIface(Json::Value &augInterface,
                            Json::Value &currInterface) {

        for (auto &detail : currInterface.getMemberNames()) {
            Json::Value const &obj = augInterface[detail];
            if (obj.isObject()) {
                /// @todo mergeIdenticalInterfaces(currInterface, augInterface,
                /// detail)
            } else {
                augInterface[detail] = currInterface[detail];
            }
        }
    }

    /// @brief For eyetracker, it will add the following interfaces to the
    /// descriptor provided that they are set to true:
    /// OSVR_Direction, OSVR_Location2D, OSVR_Tracker, OSVR_Button
    void normalizeForEyeTracker(Json::Value &descriptor,
                                std::string const &ifaceName) {

        // step into the interfaces object
        Json::Value const &iface = descriptor[INTERFACES_KEY][ifaceName];

        // hold the count for eyetracker sensors to initialize
        int count = descriptor[INTERFACES_KEY][ifaceName][COUNT_KEY].asInt();

        // if we couldn't find count then return
        if (!count) {
            return;
        }

        Json::Value augInterfaces(Json::objectValue);
        // go thru details of interface and
        for (auto &subIface : iface.getMemberNames()) {
            // check if subinterface is set to true
            bool enabled =
                descriptor[INTERFACES_KEY][ifaceName][subIface].asBool();

            if (enabled) {

                if (boost::iequals(subIface, LOCATION2D_KEY)) {
                    augInterfaces[LOCATION2D_KEY][COUNT_KEY] = count;
                } else if (boost::iequals(subIface, DIRECTION_KEY)) {
                    augInterfaces[DIRECTION_KEY][COUNT_KEY] = count;
                } else if (boost::iequals(subIface, TRACKER_KEY)) {

                    augInterfaces[TRACKER_KEY][POSITION_KEY] = true;
                    augInterfaces[TRACKER_KEY][ORIENTATION_KEY] = false;
                    augInterfaces[TRACKER_KEY][BOUNDED_KEY] = true;
                    augInterfaces[TRACKER_KEY][COUNT_KEY] = count;
                } else if (boost::iequals(subIface, BUTTON_KEY)) {
                    augInterfaces[BUTTON_KEY][COUNT_KEY] = count;
                } else {
                    continue;
                }
            }
        }

        if (augInterfaces.size() > 0) {
            Json::Value &currInterfaces = descriptor[INTERFACES_KEY];

            appendCurrentIface(augInterfaces, currInterfaces);
            descriptor[INTERFACES_KEY] = augInterfaces;
        }
    }

    /// @todo process for tracker interface

    std::string normalizeDeviceDescriptor(std::string const &jsonDescriptor) {

        Json::Value descriptor;
        {
            Json::Reader reader;
            if (!reader.parse(jsonDescriptor, descriptor)) {
                /// if can't parse as json then leave unchanged, err will be
                /// handler later
                return jsonDescriptor;
            }
        }
        /// no interfaces member so don't chanage anything
        if (!descriptor.isMember(INTERFACES_KEY)) {
            return jsonDescriptor;
        }

        Json::Value const &ifaceNames = descriptor[INTERFACES_KEY];

        // interfaces member isn't an object
        if (!ifaceNames.isObject()) {
            return jsonDescriptor;
        }

        for (auto const &ifaceName : ifaceNames.getMemberNames()) {

            if (boost::iequals(ifaceName, EYETRACKER_KEY)) {
                normalizeForEyeTracker(descriptor, ifaceName);
            } else if (boost::iequals(ifaceName, TRACKER_KEY)) {
                /// @todo for tracker
            } else {
                /// @todo for future interfaces
            }
        }

        const std::string normalizedDescriptor = descriptor.toStyledString();
        return normalizedDescriptor;
    }

} // namespace common
} // namespace osvr
