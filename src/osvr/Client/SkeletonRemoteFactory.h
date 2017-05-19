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

#ifndef INCLUDED_SkeletonRemoteFactory_h_GUID_FFA967D3_4A04_44AF_841E_BF0866185E22
#define INCLUDED_SkeletonRemoteFactory_h_GUID_FFA967D3_4A04_44AF_841E_BF0866185E22

// Internal Includes
#include "RemoteHandlerInternals.h"
#include "VRPNConnectionCollection.h"
#include <osvr/Client/Export.h>
#include <osvr/Client/RemoteHandler.h>
#include <osvr/Client/Skeleton.h>
#include <osvr/Common/ClientContext.h>
#include <osvr/Common/InterfaceList.h>
#include <osvr/Common/OriginalSource.h>
#include <osvr/Common/SkeletonComponent.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace client {

    class SkeletonRemoteHandler : public RemoteHandler {
      public:
        SkeletonRemoteHandler(vrpn_ConnectionPtr const &conn,
                              std::string const &deviceName,
                              boost::optional<OSVR_ChannelCount> sensor,
                              common::InterfaceList &ifaces,
                              common::ClientContext *ctx);

        /// @brief Deleted assignment operator.
        SkeletonRemoteHandler &
        operator=(SkeletonRemoteHandler const &) = delete;

        virtual ~SkeletonRemoteHandler() {
            /// @todo do we need to unregister?
        }

        virtual void update() { m_dev->update(); }

      private:
        void m_handleSkeleton(common::SkeletonNotification const &data,
                              util::time::TimeValue const &timestamp);
        void m_handleSkeletonSpec(common::SkeletonSpec const &data,
                                  util::time::TimeValue const &timestamp);

        common::BaseDevicePtr m_dev;
        common::ClientContext *m_ctx;
        common::SkeletonComponent *m_skeleton;
        RemoteHandlerInternals m_internals;
        boost::optional<OSVR_ChannelCount> m_sensor;
        std::string m_deviceName;
        std::unique_ptr<OSVR_SkeletonObject> m_skeletonConf;
        Json::Value m_articulationSpec;
    };

    class SkeletonRemoteFactory {
      public:
        SkeletonRemoteFactory(VRPNConnectionCollection const &conns);

        template <typename T> void registerWith(T &factory) const {
            factory.addFactory("skeleton", *this);
        }

        shared_ptr<RemoteHandler>
        operator()(common::OriginalSource const &source,
                   common::InterfaceList &ifaces, common::ClientContext &ctx);

      private:
        VRPNConnectionCollection m_conns;
    };

} // namespace client
} // namespace osvr

#endif // INCLUDED_SkeletonRemoteFactory_h_GUID_FFA967D3_4A04_44AF_841E_BF0866185E22
