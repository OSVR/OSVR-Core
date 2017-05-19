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
#include <osvr/Common/ApplyPathNodeVisitor.h>
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/CreateDevice.h>
#include <osvr/Common/JSONTransformVisitor.h>
#include <osvr/Common/OriginalSource.h>
#include <osvr/Common/ProcessArticulationSpec.h>
#include <osvr/Common/Transform.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/TreeNode.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/optional.hpp>
#include <boost/variant/get.hpp>
#include <json/value.h>

// Standard includes

namespace osvr {
namespace client {

    SkeletonRemoteHandler::SkeletonRemoteHandler(
        vrpn_ConnectionPtr const &conn, std::string const &deviceName,
        boost::optional<OSVR_ChannelCount> sensor,
        common::InterfaceList &ifaces, common::ClientContext *ctx)
        : m_dev(common::createClientDevice(deviceName, conn)),
          m_internals(ifaces), m_ctx(ctx), m_sensor(sensor),
          m_deviceName(deviceName), m_skeletonConf(nullptr),
          m_articulationSpec(Json::objectValue) {

        auto skeleton = common::SkeletonComponent::create("");
        m_skeleton = m_dev->addComponent(skeleton);

        m_skeleton->registerSkeletonHandler(
            [&](common::SkeletonNotification const &data,
                util::time::TimeValue const &timestamp) {
                m_handleSkeleton(data, timestamp);
            });
        m_skeleton->registerSkeletonSpecHandler(
            [&](common::SkeletonSpec const &data,
                util::time::TimeValue const &timestamp) {
                m_handleSkeletonSpec(data, timestamp);
            });

        OSVR_DEV_VERBOSE("Constructed a Skeleton Handler for " << deviceName);
    }

    void SkeletonRemoteHandler::m_handleSkeleton(
        common::SkeletonNotification const &data,
        util::time::TimeValue const &timestamp) {
        if (*m_sensor != data.sensor) {
            /// doesn't match our filter.
            return;
        }

        if (m_skeletonConf) {
            // get the latest joint and bone states
            m_skeletonConf->updateSkeletonPoses();
            // send skeleton report
            OSVR_SkeletonReport report;
            report.sensor = data.sensor;
            report.state.skeleton = m_skeletonConf.get();
            m_internals.setStateAndTriggerCallbacks(timestamp, report);
        }
    }

    void SkeletonRemoteHandler::m_handleSkeletonSpec(
        common::SkeletonSpec const &data,
        util::time::TimeValue const &timestamp) {
        // need to verify that sensor id and spec exist,
        // otherwise don't get anything
        if (!data.spec) {
            return;
        }
        // get the articulation spec for specified skeleton sensor
        Json::Value articSpec = data.spec[(*m_sensor)];
        // update only if there are changes to articulation spec
        if (m_articulationSpec != articSpec) {
            m_articulationSpec = data.spec[(*m_sensor)];
            osvr::common::PathTree articulationTree;
            articulationTree.reset();
            osvr::common::processArticulationSpecForPathTree(
                articulationTree, m_deviceName, articSpec);
            // skeleton config exists and needs to be updated
            if (m_skeletonConf) {
                m_skeletonConf->updateArticulationSpec(articulationTree);
            } else {
                try {
                    m_skeletonConf.reset(
                        new OSVR_SkeletonObject(m_ctx, articulationTree));
                } catch (std::exception &e) {
                    OSVR_DEV_VERBOSE(
                        "Error creating skeleton object : constructor "
                        "threw exception :"
                        << e.what());
                } catch (...) {
                    OSVR_DEV_VERBOSE(
                        "Error creating skeleton object : unknown exception");
                }
            }
        }
    }

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
