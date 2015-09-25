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

#ifndef INCLUDED_DisplayInput_h_GUID_66C8D271_F035_499C_6BD9_DCA114E2C9AF
#define INCLUDED_DisplayInput_h_GUID_66C8D271_F035_499C_6BD9_DCA114E2C9AF

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/RenderingTypesC.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    class DisplayConfigFactory;
    class DisplayInput {
      public:
        DisplayInput(OSVR_DisplayDimension width, OSVR_DisplayDimension height);

        OSVR_CLIENT_EXPORT OSVR_DisplayDimension getDisplayWidth() const;
        OSVR_CLIENT_EXPORT OSVR_DisplayDimension getDisplayHeight() const;

      private:
        /// @todo stick these in a struct?
        uint32_t m_width;
        uint32_t m_height;
        friend class DisplayConfigFactory;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_DisplayInput_h_GUID_66C8D271_F035_499C_6BD9_DCA114E2C9AF
