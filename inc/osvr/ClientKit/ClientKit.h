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

#ifndef INCLUDED_ClientKit_h_GUID_DD0155F5_61A4_4A76_8C2E_D9614C7A9EBD
#define INCLUDED_ClientKit_h_GUID_DD0155F5_61A4_4A76_8C2E_D9614C7A9EBD

#include <osvr/ClientKit/Context.h>
#include <osvr/ClientKit/Interface.h>

namespace osvr {

/** @brief Contains C++ header-only wrappers for the ClientKit C API.
    @ingroup ClientKit

    Use of these wrappers is optional: they duplicate functionality found in the
    C API as they are implemented in headers on top of the C API. However, they
    also provide a much cleaner, low-boilerplate interface for the most common
    operations.

    Some functionality may not be "wrapped" in a C++ API, however, it is
    always possible to extract the C entities being wrapped and use them with
    the C API in those cases.
*/
namespace clientkit {} // namespace clientkit
} // namespace osvr

#endif // INCLUDED_ClientKit_h_GUID_DD0155F5_61A4_4A76_8C2E_D9614C7A9EBD
