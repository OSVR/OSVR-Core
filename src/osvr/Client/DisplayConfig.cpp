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
#include <osvr/Client/DisplayConfig.h>
#include <osvr/Util/ProjectionMatrixFromFOV.h>
#include <osvr/Util/Verbosity.h>
#include "DisplayDescriptorSchema1.h"

// Library/third-party includes
#include <osvr/Util/EigenExtras.h>

// Standard includes
#include <stdexcept>

namespace osvr {
namespace client {
    /// @param eye 0 for left, 1 for right
    /// @todo handle swapeyes? - just for calls to this function
    inline static Viewport
    computeViewport(uint8_t eye,
                    display_schema_1::DisplayDescriptor &descriptor) {
        Viewport viewport;
        // Set up the viewport based on the display resolution and the
        // display configuration.
        switch (descriptor.getDisplayMode()) {
        case display_schema_1::DisplayDescriptor::FULL_SCREEN:
            viewport.bottom = viewport.left = 0;
            viewport.width = descriptor.getDisplayWidth();
            viewport.height = descriptor.getDisplayHeight();
            break;
        case display_schema_1::DisplayDescriptor::HORIZONTAL_SIDE_BY_SIDE:
            viewport.bottom = 0;
            viewport.height = descriptor.getDisplayHeight();
            viewport.width = descriptor.getDisplayWidth() / 2;
            // Eye 0 starts at the left, eye 1 starts in the middle.
            viewport.left = eye * viewport.width;
            break;
        case display_schema_1::DisplayDescriptor::VERTICAL_SIDE_BY_SIDE:
            viewport.left = 0;
            viewport.width = descriptor.getDisplayWidth();
            viewport.height = descriptor.getDisplayHeight() / 2;
            // Eye 0 in the top half, eye 1 at the bottom.
            if (eye == 0) {
                viewport.bottom = viewport.height;
            } else {
                viewport.bottom = 0;
            }
            break;
        default:
            throw std::logic_error("Unrecognized enum value for display mode");
        }

        return viewport;
    }

    inline static util::Rectd
    computeRect(display_schema_1::DisplayDescriptor &descriptor) {
        return util::computeSymmetricFOVRect(descriptor.getHorizontalFOV(),
                                             descriptor.getVerticalFOV());
    }

    static const char HEAD_PATH[] = "/me/head";
    DisplayConfigPtr DisplayConfigFactory::create(OSVR_ClientContext ctx) {
        DisplayConfigPtr cfg(new DisplayConfig);
        try {
            auto const descriptorString = ctx->getStringParameter("/display");

            auto desc = display_schema_1::DisplayDescriptor(descriptorString);
            cfg->m_viewers.container().emplace_back(Viewer(ctx, HEAD_PATH));
            auto &viewer = cfg->m_viewers.container().front();
            auto eyesDesc = desc.getEyes();

            /// Set up stereo vs mono
            std::vector<uint8_t> eyeIndices;
            Eigen::Vector3d offset;
            if (eyesDesc.size() == 2) {
                // stereo
                offset = desc.getIPDMeters() / 2. * Eigen::Vector3d::UnitX();
                eyeIndices = {0, 1};
            } else {
                // if (eyesDesc.size() == 1)
                // mono
                offset = Eigen::Vector3d::Zero();
                eyeIndices = {0};
            }

            /// Handle radial distortion parameters
            boost::optional<OSVR_RadialDistortionParameters> distort;
            auto k1 = desc.getDistortion();
            if (k1.k1_red != 0 || k1.k1_green != 0 || k1.k1_blue != 0) {
                OSVR_RadialDistortionParameters params;
                params.k1.data[0] = k1.k1_red;
                params.k1.data[1] = k1.k1_green;
                params.k1.data[2] = k1.k1_blue;
                distort = params;
            }

            /// Compute angular offset about Y of the optical (view) axis
            util::Angle axisOffset = 0. * util::radians;
            {
                auto overlapPct = desc.getOverlapPercent();

                if (overlapPct < 1.) {
                    const auto hfov = desc.getHorizontalFOV();
                    const auto angularOverlap = hfov * overlapPct;
                    axisOffset = (hfov - angularOverlap) / 2.;
                }
            }

            /// Infer the number of display inputs and their association with
            /// eyes (actually surfaces) based on the descriptor.
            std::vector<OSVR_DisplayInputCount> displayInputIndices;
            if (eyesDesc.size() == 2 &&
                display_schema_1::DisplayDescriptor::FULL_SCREEN ==
                    desc.getDisplayMode()) {
                // two eyes, full screen - that means two screens.

                displayInputIndices = {0, 1};

                cfg->m_displayInputs.push_back(DisplayInput(
                    desc.getDisplayWidth(), desc.getDisplayHeight()));
                cfg->m_displayInputs.push_back(DisplayInput(
                    desc.getDisplayWidth(), desc.getDisplayHeight()));
            } else {
                // everything else, assume 1 screen.
                // Note that it's OK that displayInputIndices.size() >=
                // eyesDesc.size(), we'll just not end up using the second
                // entry.
                displayInputIndices = {0, 0};

                cfg->m_displayInputs.push_back(DisplayInput(
                    desc.getDisplayWidth(), desc.getDisplayHeight()));
            }
            BOOST_ASSERT_MSG(displayInputIndices.size() >= eyesDesc.size(),
                             "Must have at least as many indices as eyes");

            /// Create the actual eye (with implied surface) objects
            for (auto eye : eyeIndices) {
                // This little computation turns 0 into -1 and 1 into 1, used as
                // a coefficient to make the two eyes do opposite things.
                // Doesn't affect mono, which has a zero offset vector.
                double offsetFactor = (2. * eye) - 1.;

                // Set up per-eye distortion parameters, if needed
                boost::optional<OSVR_RadialDistortionParameters> distortEye(
                    distort);
                if (distortEye) {
                    distortEye->centerOfProjection.data[0] =
                        eyesDesc[eye].m_CenterProjX;
                    distortEye->centerOfProjection.data[1] =
                        eyesDesc[eye].m_CenterProjY;
                }

                // precompute translation offset for this eye
                auto xlateOffset = (offsetFactor * offset).eval();

                // precompute the optical axis rotation for this eye
                // here, the left eye should get a positive offset since it's a
                // positive rotation about y, hence the -1 factor.
                auto eyeAxisOffset = axisOffset * -1. * offsetFactor;

                // Look up the display index for this eye.
                auto displayInputIdx = displayInputIndices[eye];

                /// Create the ViewerEye[Surface] and add it to the container.
                viewer.container().emplace_back(ViewerEye(
                    ctx, xlateOffset, HEAD_PATH, computeViewport(eye, desc),
                    computeRect(desc), eyesDesc[eye].m_rotate180,
                    desc.getPitchTilt().value(), distortEye, displayInputIdx,
                    eyeAxisOffset));
            }

            OSVR_DEV_VERBOSE("Display: " << desc.getHumanReadableDescription());
            return cfg;
        } catch (std::exception const &e) {
            OSVR_DEV_VERBOSE(
                "Couldn't create a display config internally! Exception: "
                << e.what());
            return DisplayConfigPtr{};
        } catch (...) {
            OSVR_DEV_VERBOSE("Couldn't create a display config internally! "
                             "Unknown exception!");
            return DisplayConfigPtr{};
        }
    }
    DisplayConfig::DisplayConfig() {}

    bool DisplayConfig::isStartupComplete() const {
        for (auto const &viewer : m_viewers) {
            if (!viewer.hasPose()) {
                return false;
            }
            for (auto const &eye : viewer) {
                if (!eye.hasPose()) {
                    return false;
                }
            }
        }
        return true;
    }

} // namespace client
} // namespace osvr
