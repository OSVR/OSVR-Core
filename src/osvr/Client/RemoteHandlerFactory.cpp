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
#include <osvr/Client/RemoteHandlerFactory.h>
#include "AnalogRemoteFactory.h"
#include "ButtonRemoteFactory.h"
#include "DirectionRemoteFactory.h"
#include "EyeTrackerRemoteFactory.h"
#include "ImagingRemoteFactory.h"
#include "Location2DRemoteFactory.h"
#include "LocomotionRemoteFactory.h"
#include "SkeletonRemoteFactory.h"
#include "TrackerRemoteFactory.h"

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    void populateRemoteHandlerFactory(RemoteHandlerFactory &factory,
                                      VRPNConnectionCollection const &conns) {
        /// Register all the factories.
        AnalogRemoteFactory(conns).registerWith(factory);
        ButtonRemoteFactory(conns).registerWith(factory);
        EyeTrackerRemoteFactory(conns).registerWith(factory);
        ImagingRemoteFactory(conns).registerWith(factory);
        Location2DRemoteFactory(conns).registerWith(factory);
        LocomotionRemoteFactory(conns).registerWith(factory);
        DirectionRemoteFactory(conns).registerWith(factory);
		SkeletonRemoteFactory(conns).registerWith(factory);
		TrackerRemoteFactory(conns).registerWith(factory);
    }

} // namespace client
} // namespace osvr
