/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_JSONTransformVisitor_h_GUID_35872365_9953_479F_C548_86F8BDAE030B
#define INCLUDED_JSONTransformVisitor_h_GUID_35872365_9953_479F_C548_86F8BDAE030B

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Common/Transform.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <json/value.h>

// Standard includes
#include <string>

namespace osvr {
namespace common {
    class JSONTransformData;
    class JSONTransformVisitor {
      public:
        /// @brief Parses json describing a transformation to compute the
        /// overall transformation.
        OSVR_COMMON_EXPORT JSONTransformVisitor(Json::Value const &root);
        OSVR_COMMON_EXPORT ~JSONTransformVisitor();

        OSVR_COMMON_EXPORT Transform const &getTransform() const;

        OSVR_COMMON_EXPORT Json::Value const &getLeaf() const;

      private:
        Transform m_transform;
        Json::Value m_leaf;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_JSONTransformVisitor_h_GUID_35872365_9953_479F_C548_86F8BDAE030B
