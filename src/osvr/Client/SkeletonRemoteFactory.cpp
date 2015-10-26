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
#include "SkeletonRemoteFactory.h"
#include "VRPNConnectionCollection.h"
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/OriginalSource.h>
#include <osvr/Client/InterfaceTree.h>
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/Transform.h>
#include <osvr/Common/JSONTransformVisitor.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/SkeletonComponent.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {

    class SkeletonRemoteHandler : public RemoteHandler {
      public:
        SkeletonRemoteHandler(vrpn_ConnectionPtr const &conn,
                              std::string const &deviceName,
                              boost::optional<OSVR_ChannelCount> sensor,
                              common::InterfaceList &ifaces)
            : m_dev(common::createClientDevice(deviceName, conn)),
              m_interfaces(ifaces), m_all(!sensor.is_initialized()),
              m_sensor(sensor) {

            auto skeleton = common::SkeletonComponent::create();
            m_dev->addComponent(skeleton);

            skeleton->registerSkeletonHandler(
                [&](common::SkeletonNotification const &data,
                    util::time::TimeValue const &timestamp) {
                    m_handleSkeleton(data, timestamp);
                });

            /**/
            OSVR_DEV_VERBOSE("Constructed a Skeleton Handler for "
                             << deviceName);
        }

        /// @brief Deleted assignment operator.
        SkeletonRemoteHandler &
        operator=(SkeletonRemoteHandler const &) = delete;

        virtual ~SkeletonRemoteHandler() {
            /// @todo do we need to unregister?
        }

        virtual void update() { m_dev->update(); }

      private:
        void m_handleSkeleton(common::SkeletonNotification const &data,
                              util::time::TimeValue const &timestamp) {
            if (!m_all && *m_sensor != data.sensor) {
                /// doesn't match our filter.
                return;
            }
        }

        common::BaseDevicePtr m_dev;
        common::InterfaceList &m_interfaces;
        bool m_all;
        boost::optional<OSVR_ChannelCount> m_sensor;
    };

    SkeletonRemoteFactory::SkeletonRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> SkeletonRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &ctx) {

        shared_ptr<RemoteHandler> ret;

        common::Transform xform{};
        if (source.hasTransform()) {
            common::JSONTransformVisitor xformParse(source.getTransformJson());
            xform = xformParse.getTransform();
        }

        auto const &devElt = source.getDeviceElement();

        /// @todo find out why make_shared causes a crash here
        ret.reset(new SkeletonRemoteHandler(
            m_conns.getConnection(devElt), devElt.getFullDeviceName(),
            source.getSensorNumberAsChannelCount(), ifaces));
        return ret;
    }

} // namespace client
} // namespace osvr
