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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "EyeTrackerRemoteFactory.h"
#include "VRPNConnectionCollection.h"
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/OriginalSource.h>
#include "InterfaceTree.h"
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/EyeTrackerComponent.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {

    class NetworkEyeTrackerRemoteHandler : public RemoteHandler {
      public:
        NetworkEyeTrackerRemoteHandler(vrpn_ConnectionPtr const &conn,
                                    std::string const &deviceName,
                                    boost::optional<OSVR_ChannelCount> sensor,
                                    common::InterfaceList &ifaces)
            : m_dev(common::createClientDevice(deviceName, conn)),
              m_interfaces(ifaces), m_all(!sensor.is_initialized()),
              m_sensor(sensor) {
            auto eyetracker = common::EyeTrackerComponent::create();
            m_dev->addComponent(eyetracker);
            eyetracker->registerEyeHandler(
                [&](common::EyeData const &data,
                    util::time::TimeValue const &timestamp) {
                    m_handleEyeTracking(data, timestamp);
                });
            OSVR_DEV_VERBOSE("Constructed an Eye Handler for "
                             << deviceName);
        }

        /// @brief Deleted assignment operator.
        NetworkEyeTrackerRemoteHandler &
        operator=(NetworkEyeTrackerRemoteHandler const &) = delete;

        virtual ~NetworkEyeTrackerRemoteHandler() {
            /// @todo do we need to unregister?
        }

        virtual void update() { m_dev->update(); }

      private:
        void m_handleEyeTracking(common::EyeData const &data,
                           util::time::TimeValue const &timestamp) {
            if (!m_all && *m_sensor != data.sensor) {
                /// doesn't match our filter.
                return;
            }

            OSVR_EyeTrackerReport report;
            report.sensor = data.sensor;
            report.state.gaze = data.gaze;
            common::ClientInterfacePtr anInterface;
            for (auto &iface : m_interfaces) {
                anInterface = iface;
                iface->triggerCallbacks(timestamp, report);
            }
            /*
            if (anInterface) {
                anInterface->getContext().acquireObject(data.buffer);
            }
            */
        }

        common::BaseDevicePtr m_dev;
        common::InterfaceList &m_interfaces;
        bool m_all;
        boost::optional<OSVR_ChannelCount> m_sensor;
    };

    EyeTrackerRemoteFactory::EyeTrackerRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> EyeTrackerRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces) {

        shared_ptr<RemoteHandler> ret;

        if (source.hasTransform()) {
            OSVR_DEV_VERBOSE(
                "Ignoring transform found on route for Eye Tracker data!");
        }

        /// @todo This is where we'd take a different path for IPC imaging data.
        auto const &devElt = source.getDeviceElement();

        /// @todo find out why make_shared causes a crash here
        ret.reset(new NetworkEyeTrackerRemoteHandler(
            m_conns.getConnection(devElt), devElt.getFullDeviceName(),
            source.getSensorNumberAsChannelCount(), ifaces));
        return ret;
    }

} // namespace client
} // namespace osvr