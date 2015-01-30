/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_VrpnAnalogServer_h_GUID_16F2A08F_2ECF_4EBB_D7B0_A2B87ACC9272
#define INCLUDED_VrpnAnalogServer_h_GUID_16F2A08F_2ECF_4EBB_D7B0_A2B87ACC9272

// Internal Includes
#include "DeviceConstructionData.h"
#include <osvr/Connection/AnalogServerInterface.h>

// Library/third-party includes
#include <vrpn_Analog.h>

// Standard includes
#include <cmath>

namespace osvr {
namespace connection {
    class VrpnAnalogServer : public vrpn_Analog, public AnalogServerInterface {
      public:
        typedef vrpn_Analog Base;
        VrpnAnalogServer(DeviceConstructionData &init)
            : Base(init.getQualifiedName().c_str(), init.conn) {
            m_setNumChannels(std::min(*init.obj.getAnalogs(),
                                      OSVR_ChannelCount(vrpn_CHANNEL_MAX)));
            // Initialize data
            memset(Base::channel, 0, sizeof(Base::channel));
            memset(Base::last, 0, sizeof(Base::last));
        }

        static const vrpn_uint32 CLASS_OF_SERVICE = vrpn_CONNECTION_LOW_LATENCY;

        virtual bool setValue(value_type val, OSVR_ChannelCount chan,
                              util::time::TimeValue const &timestamp) {
            if (chan >= m_getNumChannels()) {
                return false;
            }
            Base::channel[chan] = val;
            m_reportChanges(timestamp);
            return true;
        }
        virtual void setValues(value_type val[], OSVR_ChannelCount chans,
                               util::time::TimeValue const &timestamp) {
            if (chans > m_getNumChannels()) {
                chans = m_getNumChannels();
            }
            for (OSVR_ChannelCount i = 0; i < chans; ++i) {
                Base::channel[i] = val[i];
            }
            m_reportChanges(timestamp);
        }

      private:
        OSVR_ChannelCount m_getNumChannels() {
            return static_cast<OSVR_ChannelCount>(Base::num_channel);
        }
        void m_setNumChannels(OSVR_ChannelCount chans) {
            Base::num_channel = chans;
        }
        void m_reportChanges(util::time::TimeValue const &timestamp) {
            struct timeval t;
            util::time::toStructTimeval(t, timestamp);
            Base::report_changes(CLASS_OF_SERVICE, t);
        }
    };

} // namespace connection
} // namespace osvr
#endif // INCLUDED_VrpnAnalogServer_h_GUID_16F2A08F_2ECF_4EBB_D7B0_A2B87ACC9272
