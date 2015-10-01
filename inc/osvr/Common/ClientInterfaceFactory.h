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

#ifndef INCLUDED_ClientInterfaceFactory_h_GUID_B5A11486_6B0F_4284_D9E9_4A15C409742D
#define INCLUDED_ClientInterfaceFactory_h_GUID_B5A11486_6B0F_4284_D9E9_4A15C409742D

// Internal Includes
#include <osvr/Common/ClientInterfacePtr.h>
#include <osvr/Common/ClientContext_fwd.h>
#include <osvr/Common/Export.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>

namespace osvr {
namespace common {

    /// @brief A factory function type taking the client context and path, and
    /// returning a ClientInterfacePtr. The ClientContext will handle
    /// notifying its internals about the new interface before returning it.
    using ClientInterfaceFactory =
        std::function<ClientInterfacePtr(ClientContext &, const char[])>;

    /// @brief Returns a client interface factory suitable for standard client
    /// use.
    ClientInterfaceFactory getStandardClientInterfaceFactory();
} // namespace common
} // namespace osvr

#endif // INCLUDED_ClientInterfaceFactory_h_GUID_B5A11486_6B0F_4284_D9E9_4A15C409742D
