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

#ifndef INCLUDED_Viewer_h_GUID_40CF2CEC_DB9F_4D94_3F1C_329AD0489EF7
#define INCLUDED_Viewer_h_GUID_40CF2CEC_DB9F_4D94_3F1C_329AD0489EF7

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Client/ViewerEye.h>
#include <osvr/Client/InternalInterfaceOwner.h>

// Library/third-party includes
// - none

// Standard includes
#include <utility>

namespace osvr {
namespace client {
    class DisplayConfigFactory;
    class Viewer {
      public:
        Viewer(Viewer const &) = delete;
        Viewer &operator=(Viewer const &) = delete;
        Viewer(Viewer &&other)
            : m_head(std::move(other.m_head)), m_eyes(std::move(other.m_eyes)) {
        }

        inline OSVR_EyeCount size() const {
            return static_cast<OSVR_EyeCount>(m_eyes.size());
        }

        inline ViewerEye &operator[](OSVR_EyeCount index) {
            return m_eyes[index];
        }

        inline ViewerEye const &operator[](OSVR_EyeCount index) const {
            return m_eyes[index];
        }

        OSVR_CLIENT_EXPORT OSVR_Pose3 getPose() const;

      private:
        friend class DisplayConfigFactory;
        Viewer(OSVR_ClientContext ctx, const char path[]);
        InternalInterfaceOwner m_head;
        std::vector<ViewerEye> m_eyes;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_Viewer_h_GUID_40CF2CEC_DB9F_4D94_3F1C_329AD0489EF7
