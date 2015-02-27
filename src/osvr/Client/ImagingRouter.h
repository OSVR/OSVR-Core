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

#ifndef INCLUDED_ImagingRouter_h_GUID_0AADFE14_859E_40B7_B16B_832DFC411536
#define INCLUDED_ImagingRouter_h_GUID_0AADFE14_859E_40B7_B16B_832DFC411536

// Internal Includes
#include "VRPNContext.h"
#include <osvr/Client/ClientInterface.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/ImagingComponent.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/lexical_cast.hpp>

// Standard includes
#include <string>
#include <vector>

namespace osvr {
namespace client {
    class ImagingRouter : public RouterEntry {
      public:
        /// @todo this is a workaround for bind not working right, not sure why.
        class Callback {
          public:
            Callback(ImagingRouter &router) : m_router(&router) {}

            void operator()(common::ImageData const &data,
                            util::time::TimeValue const &timestamp) {
                m_router->handleImage(data, timestamp);
            }

          private:
            ImagingRouter *m_router;
        };
        ImagingRouter(ClientContext *ctx, vrpn_ConnectionPtr const &conn,
                      std::string const &deviceName,
                      std::vector<std::string> &components,
                      std::string const &dest)
            : RouterEntry(ctx, dest),
              m_dev(common::createClientDevice(deviceName, conn)) {

            if (components.size() == 1) {
                try {
                    m_sensor = boost::lexical_cast<OSVR_ChannelCount>(
                        components.back());
                } catch (boost::bad_lexical_cast &) {
                    OSVR_DEV_VERBOSE(
                        "Could not convert Imaging sensor specification '"
                        << components.back()
                        << "' to a number, skipping report filtering!");
                }
            } else if (!components.empty()) {
                OSVR_DEV_VERBOSE("Not sure what to do with multiple "
                                 "components, expected a single Imaging sensor "
                                 "number, skipping report filtering!");
            }
            auto imaging = common::ImagingComponent::create();
            m_dev->addComponent(imaging);
            imaging->registerImageHandler(Callback(*this));
        }

        void handleImage(common::ImageData const &data,
                         util::time::TimeValue const &timestamp) {
            if (m_sensor.is_initialized() && *m_sensor != data.sensor) {
                /// doesn't match our filter.
                return;
            }
            bool passData = false;
            OSVR_ImagingReport report;
            report.sensor = data.sensor;
            report.state.metadata = data.metadata;
            report.state.data = data.buffer.get();
            for (auto const &iface : getContext()->getInterfaces()) {
                if (iface->getPath() == getDest()) {
                    iface->triggerCallbacks(timestamp, report);
                }
            }
            if (passData) {
                getContext()->acquireObject(data.buffer);
            }
        }
        void operator()() { m_dev->update(); }

      private:
        common::BaseDevicePtr m_dev;
        boost::optional<OSVR_ChannelCount> m_sensor;
    };
} // namespace client
} // namespace osvr
#endif // INCLUDED_ImagingRouter_h_GUID_0AADFE14_859E_40B7_B16B_832DFC411536
