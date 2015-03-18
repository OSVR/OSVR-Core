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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_DecomposeOriginalSource_h_GUID_B7D02DC3_ACFA_4BAF_85C5_8B70854700A7
#define INCLUDED_DecomposeOriginalSource_h_GUID_B7D02DC3_ACFA_4BAF_85C5_8B70854700A7

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathNode_fwd.h>
#include <osvr/Common/PathElementTypes_fwd.h>

// Library/third-party includes
#include <boost/optional.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {

    /// @brief The result of resolving a tree node to a device: either an
    /// original source to connect to, or nothing. Typically wrapped in
    /// boost::optional for return from functions.
    class OriginalSource {
      public:
        OSVR_COMMON_EXPORT OriginalSource();

        /// @brief Decompose a path node representing an original source into
        /// its device, interface, and sensor.
        OSVR_COMMON_EXPORT void decompose(PathNode &node);

        OSVR_COMMON_EXPORT bool isResolved() const;

        void setDevice(PathNode &device);

        void setInterface(PathNode &iface);

        void setSensor(PathNode &sensor);

        void setTransform(std::string const &transform);

        PathNode *getDevice() const;

        OSVR_COMMON_EXPORT elements::DeviceElement const &
        getDeviceElement() const;

        PathNode *getInterface() const;
        OSVR_COMMON_EXPORT std::string getInterfaceName() const;

        PathNode *getSensor() const;

        OSVR_COMMON_EXPORT boost::optional<int> getSensorNumber() const;

        OSVR_COMMON_EXPORT std::string getTransform() const;
        OSVR_COMMON_EXPORT bool hasTransform() const;

      private:
        PathNode *m_device;
        PathNode *m_interface;
        PathNode *m_sensor;
        std::string m_transform;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_DecomposeOriginalSource_h_GUID_B7D02DC3_ACFA_4BAF_85C5_8B70854700A7
