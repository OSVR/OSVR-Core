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

#ifndef INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A
#define INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A

// Internal Includes
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

struct OSVR_ClientInterfaceObject;
namespace osvr {
namespace common {
    typedef ::OSVR_ClientInterfaceObject ClientInterface;
    /// @brief Pointer for holding ClientInterface objects safely.
    typedef shared_ptr<ClientInterface> ClientInterfacePtr;
    /// @brief Pointer for observing ClientInterface objects safely and weakly.
    typedef weak_ptr<ClientInterface> ClientInterfaceWeakPtr;
} // namespace common
} // namespace osvr

#endif // INCLUDED_ClientInterfacePtr_h_GUID_8B87240D_2BBD_46BC_8D74_BC3436FAAF3A
