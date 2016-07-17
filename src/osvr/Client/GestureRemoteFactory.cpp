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
#include "GestureRemoteFactory.h"
#include "RemoteHandlerInternals.h"
#include "VRPNConnectionCollection.h"
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/OriginalSource.h>
#include <osvr/Client/InterfaceTree.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/GestureComponent.h>
#include <osvr/Common/RegisteredStringMap.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {

    class GestureRemoteHandler : public RemoteHandler {
      public:
        GestureRemoteHandler(vrpn_ConnectionPtr const &conn,
                             std::string const &deviceName,
                             boost::optional<OSVR_ChannelCount> sensor,
                             common::InterfaceList &ifaces,
                             common::ClientContext *ctx)
            : m_dev(common::createClientDevice(deviceName, conn)),
              m_internals(ifaces), m_all(!sensor.is_initialized()),
              m_sensor(sensor), m_ctx(ctx) {

            m_sysComponent = m_ctx->getSystemComponent();
            m_gestureNameMap = m_sysComponent->getGestureMap();
            auto gesture = common::GestureComponent::create(*m_sysComponent);
            m_dev->addComponent(gesture);

            gesture->registerGestureHandler(
                [&](common::GestureData const &data,
                    util::time::TimeValue const &timestamp) {
                    m_handleGesture(data, timestamp);
                });

            /**/
            OSVR_DEV_VERBOSE("Constructed an Gesture Handler for "
                             << deviceName);
        }

        /// @brief Deleted assignment operator.
        GestureRemoteHandler &operator=(GestureRemoteHandler const &) = delete;

        virtual ~GestureRemoteHandler() {
            /// @todo do we need to unregister?
        }

        virtual void update() { m_dev->update(); }

      private:
        void m_handleGesture(common::GestureData const &data,
                             util::time::TimeValue const &timestamp) {
            if (!m_all && *m_sensor != data.sensor) {
                /// doesn't match our filter.
                return;
            }

            OSVR_GestureReport report;
            util::StringID id = m_gestureNameMap->corrMap.convertPeerToLocalID(
                util::PeerStringID(data.gestureID.value()));
            if (id.empty()) {
                // could not find a peer to local mapping, discarding report
                return;
            }

            report.sensor = data.sensor;
            report.state = data.gestureState;
            report.gestureID = id.value();
            m_internals.setStateAndTriggerCallbacks(timestamp, report);
        }

        common::BaseDevicePtr m_dev;
        RemoteHandlerInternals m_internals;
        bool m_all;
        boost::optional<OSVR_ChannelCount> m_sensor;
        // map to keep track of gesture map and server to local ID map
        common::GestureDataPtr m_gestureNameMap;
        common::ClientContext *m_ctx;
        common::SystemComponent *m_sysComponent;
    };

    GestureRemoteFactory::GestureRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> GestureRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &ctx) {

        shared_ptr<RemoteHandler> ret;

        if (source.hasTransform()) {
            OSVR_DEV_VERBOSE(
                "Ignoring transform found on route for Gesture data!");
        }

        auto const &devElt = source.getDeviceElement();

        /// @todo find out why make_shared causes a crash here
        ret.reset(new GestureRemoteHandler(
            m_conns.getConnection(devElt), devElt.getFullDeviceName(),
            source.getSensorNumberAsChannelCount(), ifaces, &ctx));
        return ret;
    }

} // namespace client
} // namespace osvr
