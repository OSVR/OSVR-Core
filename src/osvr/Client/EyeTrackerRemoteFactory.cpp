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
#include "osvr/Common/Location2DComponent.h"
#include "osvr/Common/DirectionComponent.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {

    class NetworkEyeTrackerRemoteHandler : public RemoteHandler {
      public:

		  struct Options {
			  Options()
				  : reportPosition(false), reportDirection(false),
				  reportBlink(false) {}
			  bool reportPosition;
			  bool reportDirection;
			  bool reportBlink;
		  };


        NetworkEyeTrackerRemoteHandler(vrpn_ConnectionPtr const &conn,
									Options const &options,
                                    std::string const &deviceName,
                                    boost::optional<OSVR_ChannelCount> sensor,
                                    common::InterfaceList &ifaces)
									: m_dev(common::createClientDevice(deviceName, conn)), m_opts(options),
              m_interfaces(ifaces), m_all(!sensor.is_initialized()),
              m_sensor(sensor) {

			/*
			std::cout << "report Direction is " <<  m_opts.reportDirection << std::endl;
			std::cout << "report Position is " << m_opts.reportPosition << std::endl;

			auto location = common::DirectionComponent::create();
			m_dev->addComponent(location);
			location->registerDirectionHandler(
				[&](common::DirectionData const &data,
				util::time::TimeValue const &timestamp) {
				
				common::EyeData iData;
				iData.gaze.gazeDirection3D = data.direction;
				//iData.gaze.gazeDirection2D = data.location;
				iData.sensor = data.sensor;
				
				m_handleEyeTracking(iData, timestamp);
			});

			OSVR_DEV_VERBOSE("Constructed an Eye Handler for "
				<< deviceName);
				*/
			
            auto eyetracker = common::EyeTrackerComponent::create();
			m_dev->addComponent(eyetracker);
            eyetracker->registerEyeHandler(
                [&](OSVR_Eye_Notification const data,
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
        void m_handleEyeTracking(OSVR_Eye_Notification const data,
                           util::time::TimeValue const &timestamp) {
			/*
			if (!m_all && *m_sensor != data.sensor) {
                /// doesn't match our filter.
                return;
            }
			*/
			std::cout << "Got eye of notification with " << data << std::endl;
            OSVR_EyeTrackerReport report;
			report.sensor = 99;
            
            common::ClientInterfacePtr anInterface;
            for (auto &iface : m_interfaces) {
                anInterface = iface;
                iface->triggerCallbacks(timestamp, report);
            }

        }

        common::BaseDevicePtr m_dev;
        common::InterfaceList &m_interfaces;
        bool m_all;
        boost::optional<OSVR_ChannelCount> m_sensor;
		Options m_opts;
    };

    EyeTrackerRemoteFactory::EyeTrackerRemoteFactory(
        VRPNConnectionCollection const &conns)
        : m_conns(conns) {}

    shared_ptr<RemoteHandler> EyeTrackerRemoteFactory::
    operator()(common::OriginalSource const &source,
               common::InterfaceList &ifaces) {

        shared_ptr<RemoteHandler> ret;

		NetworkEyeTrackerRemoteHandler::Options opts;

		auto interfaceType = source.getInterfaceName();
		
		if ("position2D" == interfaceType){
			opts.reportPosition = true;
		}
		else if ("direction" == interfaceType){
			opts.reportDirection = true;
		}
		else{
			std::cout << "Interface Type is " << interfaceType << std::endl;
			opts.reportPosition = true;
			opts.reportDirection = true;
			opts.reportBlink = true;
		}
        
        auto const &devElt = source.getDeviceElement();

        /// @todo find out why make_shared causes a crash here
        ret.reset(new NetworkEyeTrackerRemoteHandler(
            m_conns.getConnection(devElt), opts, devElt.getFullDeviceName(),
            source.getSensorNumberAsChannelCount(), ifaces));
        return ret;
    }

} // namespace client
} // namespace osvr