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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_VrpnButtonServer_h_GUID_264EFE6E_C328_45AE_B296_10980DD053AE
#define INCLUDED_VrpnButtonServer_h_GUID_264EFE6E_C328_45AE_B296_10980DD053AE

// Internal includes
#include "DeviceConstructionData.h"
#include <osvr/Connection/ButtonServerInterface.h>

// Library/third-party includes
#include <vrpn_Button.h>

// Standard includes
#include <cmath>

namespace osvr {
namespace connection {
    class VrpnButtonServer : public vrpn_Button_Filter,
                             public ButtonServerInterface {
      public:
        typedef vrpn_Button_Filter Base;
        VrpnButtonServer(DeviceConstructionData &init)
            : vrpn_Button_Filter(init.getQualifiedName().c_str(), init.conn) {
            m_setNumChannels(
                std::min(*init.obj.getButtons(),
                         OSVR_ChannelCount(vrpn_BUTTON_MAX_BUTTONS)));
            // Initialize data
            memset(Base::buttons, 0, sizeof(Base::buttons));
            memset(Base::lastbuttons, 0, sizeof(Base::lastbuttons));

            // Report interface out.
            init.obj.returnButtonInterface(*this);
        }

        virtual bool setValue(value_type val, OSVR_ChannelCount chan,
                              util::time::TimeValue const &tv) {
            if (chan >= m_getNumChannels()) {
                return false;
            }
            Base::buttons[chan] = val;
            m_reportChanges(tv);
            return true;
        }

        virtual void setValues(value_type val[], OSVR_ChannelCount chans,
                               util::time::TimeValue const &tv) {
            if (chans > m_getNumChannels()) {
                chans = m_getNumChannels();
            }
            for (OSVR_ChannelCount i = 0; i < chans; ++i) {
                Base::buttons[i] = val[i];
            }
            m_reportChanges(tv);
        }

      private:
        OSVR_ChannelCount m_getNumChannels() {
            return static_cast<OSVR_ChannelCount>(Base::num_buttons);
        }
        void m_setNumChannels(OSVR_ChannelCount chans) {
            Base::num_buttons = chans;
        }
        void m_reportChanges(util::time::TimeValue const &tv) {
            util::time::toStructTimeval(Base::timestamp, tv);
            Base::report_changes();
        }
    };

} // namespace connection
} // namespace osvr

#endif // INCLUDED_VrpnButtonServer_h_GUID_264EFE6E_C328_45AE_B296_10980DD053AE
