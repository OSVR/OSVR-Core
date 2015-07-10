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
#include <osvr/Common/ClientInterface.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/OriginalSource.h>
#include "InterfaceTree.h"
#include <osvr/Util/Verbosity.h>
#include <osvr/Common/CreateDevice.h>

#include <osvr/Common/EyeTrackerComponent.h>
#include <osvr/Common/Location2DComponent.h>
#include <osvr/Common/DirectionComponent.h>
#include <osvr/Common/RoutingConstants.h>
#include <osvr/Common/InterfaceState.h>

// Library/third-party includes
#include <boost/lexical_cast.hpp>
#include <boost/any.hpp>
#include <boost/variant/get.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <json/value.h>
#include <json/reader.h>

// Standard includes
#include <iostream>
#include <string>

namespace osvr {
namespace client {

    class EyeTrackerRemoteHandler : public RemoteHandler {
      public:
        struct Options {
            Options()
                : reportDirection(false), reportBasePoint(false),
                  reportLocation2D(false), reportBlink(false), dirIface() {}
            bool reportDirection;
            bool reportBasePoint;
            bool reportLocation2D;
            bool reportBlink;
            osvr::common::ClientInterfacePtr dirIface;
            osvr::common::ClientInterfacePtr locationIface;
            osvr::common::ClientInterfacePtr trackerIface;
            osvr::common::ClientInterfacePtr buttonIface;
        };

        EyeTrackerRemoteHandler(vrpn_ConnectionPtr const &conn,
                                std::string const &deviceName,
                                Options const &options,
                                boost::optional<OSVR_ChannelCount> sensor,
                                common::InterfaceList &ifaces)
            : m_dev(common::createClientDevice(deviceName, conn)),
              m_interfaces(ifaces), m_all(!sensor.is_initialized()),
              m_opts(options), m_sensor(sensor) {
            auto eyetracker = common::EyeTrackerComponent::create();
            m_dev->addComponent(eyetracker);
            eyetracker->registerEyeHandler(
                [&](common::OSVR_EyeNotification const &data,
                    util::time::TimeValue const &timestamp) {
                    m_handleEyeTracking(data, timestamp);
                });
            OSVR_DEV_VERBOSE("Constructed an Eye Handler for " << deviceName);
        }

        /// @brief Deleted assignment operator.
        EyeTrackerRemoteHandler &
        operator=(EyeTrackerRemoteHandler const &) = delete;

        virtual ~EyeTrackerRemoteHandler() {
            /// @todo do we need to unregister?
        }

        virtual void update() { m_dev->update(); }

      private:
        void m_handleEyeTracking3d(common::OSVR_EyeNotification const &data,
                                   util::time::TimeValue const &timestamp) {

            OSVR_EyeTracker3DReport report;
            report.sensor = data.sensor;
            report.state.directionValid = false;
            report.state.basePointValid = false;
            util::time::TimeValue timest = timestamp;
            if (m_opts.reportDirection) {
                report.state.directionValid =
                    m_opts.dirIface->getState<OSVR_DirectionReport>(
                        timest, report.state.direction);
            }
            if (m_opts.reportBasePoint) {
                report.state.basePointValid =
                    m_opts.trackerIface->getState<OSVR_PositionReport>(
                        timest, report.state.basePoint);
            }
            if (!(report.state.basePointValid || report.state.directionValid)) {
                return; // don't send an empty report.
            }

            /// @todo what timestamp do we use - the one from the notification
            /// or the ones from the original reports? At least right now
            /// they're theoretically the same, but...
            for (auto &iface : m_interfaces) {
                iface->triggerCallbacks(timestamp, report);
            }
        }

        void m_handleEyeTracking2d(common::OSVR_EyeNotification const &data,
                                   util::time::TimeValue const &timestamp) {

            if (!m_opts.reportLocation2D) {
                return;
            }
            OSVR_EyeTracker2DReport report;
            report.sensor = data.sensor;
            bool locationValid = false;
            util::time::TimeValue reportTime;

            locationValid =
                m_opts.locationIface->getState<OSVR_Location2DReport>(
                    reportTime, report.state);
            if (!locationValid) {
                return; // don't send an empty report.
            }

            /// @todo what timestamp do we use - the one from the notification
            /// or the one from the original report? At least right now they're
            /// the same, but...
            for (auto &iface : m_interfaces) {
                iface->triggerCallbacks(timestamp, report);
            }
        }

        void m_handleEyeBlink(common::OSVR_EyeNotification const &data,
                              util::time::TimeValue const &timestamp) {

            if (!m_opts.reportBlink) {
                return;
            }
            OSVR_EyeTrackerBlinkReport report;
            report.sensor = data.sensor;
            bool haveBlink = false;
            util::time::TimeValue blinkTimestamp;

            haveBlink = m_opts.buttonIface->getState<OSVR_ButtonReport>(
                blinkTimestamp, report.state);
            if (!haveBlink) {
                return; // don't send an empty report.
            }
            /// @todo what timestamp do we use - the one from the notification
            /// or the one from the original report? At least right now they're
            /// the same, but...
            for (auto &iface : m_interfaces) {
                iface->triggerCallbacks(timestamp, report);
            }
        }

        void m_handleEyeTracking(common::OSVR_EyeNotification const &data,
                                 util::time::TimeValue const &timestamp) {
            if (!m_all && *m_sensor != data.sensor) {
                /// doesn't match our filter.
                return;
            }

            m_handleEyeTracking3d(data, timestamp);
            m_handleEyeTracking2d(data, timestamp);
            m_handleEyeBlink(data, timestamp);
        }

        common::BaseDevicePtr m_dev;
        common::InterfaceList &m_interfaces;
        bool m_all;
        Options m_opts;
        boost::optional<OSVR_ChannelCount> m_sensor;
    };

    EyeTrackerRemoteFactory::EyeTrackerRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> EyeTrackerRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces, common::ClientContext &ctx) {

        shared_ptr<RemoteHandler> ret;

        EyeTrackerRemoteHandler::Options opts;

        auto const &myDescriptor = source.getDeviceElement().getDescriptor();
        auto devicePath = source.getDevicePath();

        if (myDescriptor["interfaces"]["eyetracker"].isMember("direction")) {
            opts.reportDirection = true;
            const std::string iface = devicePath + "/direction";
            /// @todo need to append sensor number here!
            // boost::lexical_cast<std::string>(source.getSensorNumberAsChannelCount())
            opts.dirIface = ctx.getInterface(iface.c_str());
        }
        if (myDescriptor["interfaces"]["eyetracker"].isMember("tracker")) {
            opts.reportBasePoint = true;
            const std::string iface = devicePath + "/tracker/";
            /// @todo need to append sensor number here!
            // boost::lexical_cast<std::string>(source.getSensorNumberAsChannelCount())
            opts.trackerIface = ctx.getInterface(iface.c_str());
        }

        if (myDescriptor["interfaces"]["eyetracker"].isMember("location2D")) {
            opts.reportLocation2D = true;
            const std::string iface = devicePath + "/location2D/";
            /// @todo need to append sensor number here!
            // boost::lexical_cast<std::string>(source.getSensorNumberAsChannelCount())
            opts.locationIface = ctx.getInterface(iface.c_str());
        }
        if (myDescriptor["interfaces"]["eyetracker"].isMember("button")) {
            opts.reportBlink = true;
            const std::string iface = devicePath + "/button/";
            /// @todo need to append sensor number here!
            // boost::lexical_cast<std::string>(source.getSensorNumberAsChannelCount())
            opts.buttonIface = ctx.getInterface(iface.c_str());
        }

        if (source.hasTransform()) {
            /// @todo we actually should be using it on direction and origin
            OSVR_DEV_VERBOSE(
                "Ignoring transform found on route for Eye Tracker data!");
        }

        auto const &devElt = source.getDeviceElement();

        ret.reset(new EyeTrackerRemoteHandler(
            m_conns.getConnection(devElt), devElt.getFullDeviceName(), opts,
            source.getSensorNumberAsChannelCount(), ifaces));
        return ret;
    }

} // namespace client
} // namespace osvr