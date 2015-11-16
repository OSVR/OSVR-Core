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

#include <osvr/Util/TreeNode.h>
#include <osvr/Common/PathTree.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/ProcessArticulationSpec.h>
#include <osvr/Common/ApplyPathNodeVisitor.h>
// Library/third-party includes
#include <boost/variant/get.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>

// Standard includes

#include <string>
#include <ostream>
#include <iostream>

namespace osvr {
namespace client {

    class SkeletonTraverser : public boost::static_visitor<> {
      public:
        /// @brief Constructor
        SkeletonTraverser() : boost::static_visitor<>() {}

        /// @brief ignore null element
        void operator()(osvr::common::PathNode const &,
                        osvr::common::elements::NullElement const &) {}

        /// @brief We might print something for a sensor element.
        void
        operator()(osvr::common::PathNode const &node,
                   osvr::common::elements::ArticulationElement const &elt) {
            std::cout << "Contained values: fullPath = "
                      << osvr::common::getFullPath(node)
                      << " Articulation Type = " << elt.getArticulationType()
                      << "; Tracker path = " << elt.getTrackerPath()
                      << "; Bone Name = " << elt.getBoneName() << std::endl;
        }

        /// @brief Catch-all for other element types.
        template <typename T>
        void operator()(osvr::common::PathNode const &node, T const &elt) {}

      private:
    };

    class SkeletonRemoteHandler : public RemoteHandler {
      public:
        SkeletonRemoteHandler(vrpn_ConnectionPtr const &conn,
                              std::string const &deviceName,
                              boost::optional<OSVR_ChannelCount> sensor,
                              common::InterfaceList &ifaces,
                              common::ClientContext *ctx)
            : m_dev(common::createClientDevice(deviceName, conn)),
              m_interfaces(ifaces), m_sensor(sensor), m_ctx(ctx),
              m_deviceName(deviceName) {

            auto skeleton = common::SkeletonComponent::create("");
            m_dev->addComponent(skeleton);

            skeleton->registerSkeletonHandler(
                [&](common::SkeletonNotification const &data,
                    util::time::TimeValue const &timestamp) {
                    m_handleSkeleton(data, timestamp);
                });
            skeleton->registerSkeletonSpecHandler(
                [&](common::SkeletonSpec const &data,
                    util::time::TimeValue const &timestamp) {
                    m_handleSkeletonSpec(data, timestamp);
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
            if (*m_sensor != data.sensor) {
                /// doesn't match our filter.
                return;
            }

            // send skeleton update callback
            OSVR_SkeletonReport report;
            report.sensor = data.sensor;
            report.state.dataAvailable = 1;
            common::ClientInterfacePtr anInterface;
            for (auto &iface : m_interfaces) {
                anInterface = iface;
                iface->triggerCallbacks(timestamp, report);
            }
        }

        void m_handleSkeletonSpec(common::SkeletonSpec const &data,
                                  util::time::TimeValue const &timestamp) {
            // need to verify that sensor id and spec exist,
            // otherwise don't get anything
            if (!data.spec) {
                return;
            }
            // get the articulation spec for specified skeleton sensor
            Json::Value articSpec = data.spec[m_sensor.value()];
            m_skeletonTree.reset();

            osvr::common::processArticulationSpecForPathTree(
                m_skeletonTree, m_deviceName, articSpec);

            /// Now traverse for output
            SkeletonTraverser printer{};
            osvr::util::traverseWith(
                m_skeletonTree.getRoot(),
                [&printer](osvr::common::PathNode const &node) {
                    osvr::common::applyPathNodeVisitor(printer, node);
                });
        }
        common::PathTree m_skeletonTree;
        common::BaseDevicePtr m_dev;
        common::InterfaceList &m_interfaces;
        common::ClientContext *m_ctx;
        boost::optional<OSVR_ChannelCount> m_sensor;
        std::string m_deviceName;
    };

    SkeletonRemoteFactory::SkeletonRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> SkeletonRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &ctx) {

        shared_ptr<RemoteHandler> ret;

        auto const &devElt = source.getDeviceElement();

        /// @todo find out why make_shared causes a crash here
        ret.reset(new SkeletonRemoteHandler(
            m_conns.getConnection(devElt), devElt.getFullDeviceName(),
            source.getSensorNumberAsChannelCount(), ifaces, &ctx));
        return ret;
    }

} // namespace client
} // namespace osvr
