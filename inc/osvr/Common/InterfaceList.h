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

#ifndef INCLUDED_InterfaceList_h_GUID_3D7D3D63_4F9C_4895_ED70_357BBA8094EE
#define INCLUDED_InterfaceList_h_GUID_3D7D3D63_4F9C_4895_ED70_357BBA8094EE

// Internal Includes
#include <osvr/Common/ClientInterfacePtr.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace common {

    typedef std::vector<ClientInterfacePtr> InterfaceList;

} // namespace common
} // namespace osvr
#endif // INCLUDED_InterfaceList_h_GUID_3D7D3D63_4F9C_4895_ED70_357BBA8094EE
