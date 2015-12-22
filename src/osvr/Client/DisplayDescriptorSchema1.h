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

#ifndef INCLUDED_DisplayDescriptorSchema1_h_GUID_E976C3C5_C76D_4781_6930_A133E598D5B1
#define INCLUDED_DisplayDescriptorSchema1_h_GUID_E976C3C5_C76D_4781_6930_A133E598D5B1

// Internal Includes
#include <osvr/Util/Angles.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace osvr {
namespace client {
    namespace display_schema_1 {
        struct DisplayDescriptorParseException : public std::runtime_error {
            DisplayDescriptorParseException(const std::string &message)
                : std::runtime_error("Display descriptor parse error: " +
                                     message) {}
        };
        class DisplayDescriptor {
          public:
            enum DisplayMode {
                HORIZONTAL_SIDE_BY_SIDE,
                VERTICAL_SIDE_BY_SIDE,
                FULL_SCREEN
            };

            DisplayDescriptor();
            DisplayDescriptor(const std::string &display_description);

            void parse(const std::string &display_description);

            void print() const;

            /// @brief Returns a human-readable description of the device being
            /// used, assembled from properties.
            std::string getHumanReadableDescription() const;

            /// Read the property information.
            std::string getVendor() const;
            std::string getModel() const;
            std::string getVersion() const;
            std::string getNote() const;
            // Since the original "numDisplays" value in descriptors is
            // redundant and inconsistent, this "accessor" actually computes
            // what that value should be. Usually, you either want this computed
            // value, or activeResolution().videoInputs (which should generally
            // be in sync)
            int getNumDisplays() const;

            int getDisplayTop() const;
            int getDisplayLeft() const;
            int getDisplayWidth() const;
            int getDisplayHeight() const;
            DisplayMode getDisplayMode() const;

            util::Angle getVerticalFOV() const;
            util::Angle getHorizontalFOV() const;
#if 0
            double getVerticalFOV() const;
            double getVerticalFOVRadians() const;
            double getHorizontalFOV() const;
            double getHorizontalFOVRadians() const;
#endif
            double getOverlapPercent() const;
#if 0
            double getPitchTilt() const;
#endif
            util::Angle getPitchTilt() const;

            double getIPDMeters() const;

            struct DistortionParams {
                double k1_red = 0.;
                double k1_green = 0.;
                double k1_blue = 0.;
            };

            DistortionParams getDistortion() const { return m_distort; }

            /// Structure holding the information for one eye.
            class EyeInfo {
              public:
                double m_CenterProjX = 0.5;
                double m_CenterProjY = 0.5;
                bool m_rotate180 = false;

                void print() const;
            };

            std::vector<EyeInfo> const &getEyes() const;

            struct Resolution {
                int width;
                int height;
                int video_inputs = 1;
                DisplayMode display_mode;
            };

            Resolution const &activeResolution() const {
                return m_resolutions.at(m_activeResolution);
            }

          private:
            Resolution &activeResolution() {
                return m_resolutions.at(m_activeResolution);
            }

            void m_processResolution(Json::Value const &resolution);

            std::string m_vendor;
            std::string m_model;
            std::string m_version;
            std::string m_note;

            util::Angle m_monocularHorizontalFOV;
            util::Angle m_monocularVerticalFOV;
            double m_overlapPercent;
            util::Angle m_pitchTilt;

            std::vector<Resolution> m_resolutions;

            /// @brief Distortion coefficients
            DistortionParams m_distort;

            // Rendering
            double m_rightRoll = 0.;
            double m_leftRoll = 0.;

            // Eyes
            std::vector<EyeInfo> m_eyes;

            // Active resolution
            size_t m_activeResolution = 0;
        };
    } // namespace display_schema_1

} // namespace client
} // namespace osvr
#endif // INCLUDED_DisplayConfiguration_h_GUID_E976C3C5_C76D_4781_6930_A133E598D5B1
