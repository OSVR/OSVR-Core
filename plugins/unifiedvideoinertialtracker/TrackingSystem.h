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

#ifndef INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50
#define INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50

// Internal Includes
#include "ConfigParams.h"

// Library/third-party includes
// - none

// Standard includes
#include <vector>
#include <memory>

namespace osvr {
namespace vbtracker {
    class TrackedBody;
    class TrackingSystem {
      public:
        TrackingSystem(ConfigParams const &params);
        ~TrackingSystem();
        TrackedBody *createTrackedBody();

        std::size_t getNumBodies() const { return m_bodies.size(); }
        TrackedBody &getBody(std::size_t i) { return *m_bodies.at(i); }

        /// @todo refactor;
        ConfigParams const &getParams() const { return m_params; }

      private:
        using BodyPtr = std::unique_ptr<TrackedBody>;
        ConfigParams m_params;
        std::vector<BodyPtr> m_bodies;

        /// private impl;
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_TrackingSystem_h_GUID_B94B2C23_321F_45B4_5167_CB32D2624B50
