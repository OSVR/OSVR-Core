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

// Internal Includes
#include "DisplayDescriptorSchema1.h"
#include <osvr/Common/JSONHelpers.h>
#include <osvr/Util/Verbosity.h>
#include <boost/units/io.hpp>

// Library/third-party includes
// - none

// Standard includes
#include <sstream>

namespace osvr {
namespace client {
    namespace display_schema_1 {
        /// @todo this should come from the tree and user-specific config
        static const double DEFAULT_IPD_METERS = 0.063; // 63mm

        DisplayDescriptor::DisplayDescriptor() {
            // do nothing
        }

        DisplayDescriptor::DisplayDescriptor(
            const std::string &display_description) {
            parse(display_description);
        }

        void DisplayDescriptor::parse(const std::string &display_description) {
            auto root = common::jsonParse(display_description);
            auto const &hmd = root["hmd"];
            {
                auto const &fov = hmd["field_of_view"];
                // Field of view
                m_monocularHorizontalFOV = util::Angle(
                    fov["monocular_horizontal"].asDouble() * util::degrees);
                m_monocularVerticalFOV = util::Angle(
                    fov["monocular_vertical"].asDouble() * util::degrees);
                m_OverlapPercent =
                    fov.get("overlap_percent", 100).asDouble() / 100.0;
                m_pitchTilt = util::Angle(fov.get("pitch_tilt", 0).asDouble() *
                                          util::degrees);
            }
            {
                auto const &devprops = hmd["device"];
                // Device properties
                m_vendor = devprops["vendor"].asString();
                m_model = devprops["model"].asString();
                m_version = devprops["Version"].asString();
                m_note = devprops["Note"].asString();
                // Note that this parameter is redundant and not used.
                m_NumDisplays = devprops.get("num_displays", 1).asInt();
            }
            {
                auto const &resolutions = hmd["resolutions"];
                if (resolutions.isNull()) {
                    OSVR_DEV_VERBOSE(
                        "DisplayDescriptor::parse(): ERROR: Couldn't "
                        "find resolutions array!");
                    throw DisplayDescriptorParseException(
                        "Couldn't find resolutions array.");
                }

                for (auto const &resolution : resolutions) {
                    m_processResolution(resolution);
                }

                if (m_resolutions.empty()) {
                    // We couldn't find any appropriate resolution entries
                    OSVR_DEV_VERBOSE(
                        "DisplayDescriptor::parse(): ERROR: Couldn't "
                        "find any appropriate resolutions.");
                    return;
                }
            }

            {
                auto const &rendering = hmd["rendering"];
                m_RightRoll = rendering.get("right_roll", 0).asDouble();
                m_LeftRoll = rendering.get("left_roll", 0).asDouble();
            }
            {
                auto const &distortion = hmd["distortion"];
                m_distort.k1_red = distortion.get("k1_red", 0).asDouble();
                m_distort.k1_green = distortion.get("k1_green", 0).asDouble();
                m_distort.k1_blue = distortion.get("k1_blue", 0).asDouble();
            }
            {
                auto const &eyes = hmd["eyes"];
                if (eyes.isNull()) {
                    OSVR_DEV_VERBOSE("DisplayDescriptor::parse(): ERROR: "
                                     "Couldn't find eyes array!");
                    throw DisplayDescriptorParseException(
                        "Couldn't find eyes array.");
                }
                for (auto const &eye : eyes) {
                    EyeInfo e;
                    e.m_CenterProjX = eye.get("center_proj_x", 0.5).asDouble();
                    e.m_CenterProjY = eye.get("center_proj_y", 0.5).asDouble();
                    if (eye.isMember("rotate_180")) {
                        e.m_rotate180 = (eye["rotate_180"].asInt() != 0);
                    }
                    m_eyes.push_back(e);
                }
            }
        }

        std::string DisplayDescriptor::getHumanReadableDescription() const {
            std::ostringstream os;
            bool needSpace = false;
            if (!m_vendor.empty()) {
                os << m_vendor;
                needSpace = true;
            }

            if (!m_model.empty()) {
                if (needSpace) {
                    os << " ";
                }
                os << m_model;
                needSpace = true;
            }

            if (!m_version.empty()) {
                if (needSpace) {
                    os << " ";
                }
                os << "(Version " << m_version << ")";
                needSpace = true;
            }

            if (!m_note.empty()) {
                if (needSpace) {
                    os << " ";
                }
                os << "[Display descriptor note: " << m_note << "]";
            }
            return os.str();
        }

        void
        DisplayDescriptor::m_processResolution(Json::Value const &resolution) {
            // if there is more than 1 input, display descriptor right now
            // specifies one resolution value for both inputs. that may be
            // changed in the future
            Resolution res;
            res.video_inputs = resolution.get("video_inputs", 1).asInt();

            // Window bounds
            res.width = resolution["width"].asInt();
            res.height = resolution["height"].asInt();

            // Display mode - Default to horiz side by side unless we have
            // multiple video inputs, then default to full screen (? seems
            // logical but not strictly what the json schema specifies)
            res.display_mode =
                (res.video_inputs > 1 ? FULL_SCREEN : HORIZONTAL_SIDE_BY_SIDE);

            auto const &display_mode = resolution["display_mode"];
            if (display_mode.isString()) {
                const std::string display_mode_str = display_mode.asString();
                if ("horz_side_by_side" == display_mode_str) {
                    res.display_mode = HORIZONTAL_SIDE_BY_SIDE;
                } else if ("vert_side_by_side" == display_mode_str) {
                    res.display_mode = VERTICAL_SIDE_BY_SIDE;
                } else if ("full_screen" == display_mode_str) {
                    res.display_mode = FULL_SCREEN;
                } else {
                    OSVR_DEV_VERBOSE("DisplayDescriptor::parse(): WARNING: "
                                     "Unknown display mode string: "
                                     << display_mode_str << " (using default)");
                }
            }

            m_resolutions.push_back(res);
        }

        void DisplayDescriptor::print() const {
            std::cout << "Monocular horizontal FOV: "
                      << m_monocularHorizontalFOV << std::endl;
            std::cout << "Monocular vertical FOV: " << m_monocularVerticalFOV
                      << std::endl;
            std::cout << "Overlap percent: " << m_OverlapPercent << "%"
                      << std::endl;
            std::cout << "Pitch tilt: " << m_pitchTilt << std::endl;
            std::cout << "Resolution: " << activeResolution().width << " x "
                      << activeResolution().height << std::endl;
            std::cout << "Video inputs: " << activeResolution().video_inputs
                      << std::endl;
            std::cout << "Display mode: " << activeResolution().display_mode
                      << std::endl;
            std::cout << "Right roll: " << m_RightRoll << std::endl;
            std::cout << "Left roll: " << m_LeftRoll << std::endl;
            std::cout << "Number of eyes: " << m_eyes.size() << std::endl;
            for (std::vector<EyeInfo>::size_type i = 0; i < m_eyes.size();
                 ++i) {
                std::cout << "Eye " << i << ": " << std::endl;
                m_eyes[i].print();
            }
        }

        std::string DisplayDescriptor::getVendor() const { return m_vendor; }

        std::string DisplayDescriptor::getModel() const { return m_model; }

        std::string DisplayDescriptor::getVersion() const { return m_version; }

        std::string DisplayDescriptor::getNote() const { return m_note; }

        int DisplayDescriptor::getNumDisplays() const { return m_NumDisplays; }

        int DisplayDescriptor::getDisplayTop() const { return 0; }

        int DisplayDescriptor::getDisplayLeft() const { return 0; }

        int DisplayDescriptor::getDisplayWidth() const {
            return activeResolution().width;
        }

        int DisplayDescriptor::getDisplayHeight() const {
            return activeResolution().height;
        }

        DisplayDescriptor::DisplayMode
        DisplayDescriptor::getDisplayMode() const {
            return activeResolution().display_mode;
        }

        util::Angle DisplayDescriptor::getVerticalFOV() const {
            return m_monocularVerticalFOV;
        }

        util::Angle DisplayDescriptor::getHorizontalFOV() const {
            return m_monocularHorizontalFOV;
        }

        double DisplayDescriptor::getFOVAspectRatio() const {
            return (m_monocularVerticalFOV / m_monocularHorizontalFOV).value();
        }

        double DisplayDescriptor::getOverlapPercent() const {
            return m_OverlapPercent;
        }

        util::Angle DisplayDescriptor::getPitchTilt() const {
            return m_pitchTilt;
        }

        double DisplayDescriptor::getIPDMeters() const {
            return DEFAULT_IPD_METERS;
        }

        std::vector<DisplayDescriptor::EyeInfo> const &
        DisplayDescriptor::getEyes() const {
            return m_eyes;
        }

        void DisplayDescriptor::EyeInfo::print() const {
            std::cout << "Center of projection (X): " << m_CenterProjX
                      << std::endl;
            std::cout << "Center of projection (Y): " << m_CenterProjY
                      << std::endl;
            std::cout << "Rotate by 180: " << std::boolalpha << m_rotate180
                      << std::endl;
        }

    } // namespace display_schema_1
} // namespace client
} // namespace osvr
