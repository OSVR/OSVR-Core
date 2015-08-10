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

#ifndef INCLUDED_DisplayConfig_h_GUID_70A7B58A_338C_44A7_7BF9_6945EA2D3B30
#define INCLUDED_DisplayConfig_h_GUID_70A7B58A_338C_44A7_7BF9_6945EA2D3B30

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Client/Viewer.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Client/InternalInterfaceOwner.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace client {
    class DisplayConfig;
    typedef unique_ptr<DisplayConfig> DisplayConfigPtr;
    class DisplayConfigFactory {
      public:
        OSVR_CLIENT_EXPORT DisplayConfigPtr create(OSVR_ClientContext ctx);
    };

    class DisplayConfig {
      public:
        inline OSVR_ViewerCount size() const {
            return static_cast<OSVR_ViewerCount>(m_viewers.size());
        }
        inline Viewer &operator[](OSVR_ViewerCount index) {
            return m_viewers[index];
        }
        inline Viewer const &operator[](OSVR_ViewerCount index) const {
            return m_viewers[index];
        }

      private:
        friend class DisplayConfigFactory;
        DisplayConfig(OSVR_ClientContext ctx);
        InternalInterfaceOwner m_head;
        std::vector<Viewer> m_viewers;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_DisplayConfig_h_GUID_70A7B58A_338C_44A7_7BF9_6945EA2D3B30
