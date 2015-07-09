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

#ifndef INCLUDED_GuardPtr_h_GUID_612A1083_3E2E_4790_3729_08D5AACEF5E6
#define INCLUDED_GuardPtr_h_GUID_612A1083_3E2E_4790_3729_08D5AACEF5E6

// Internal Includes
#include <osvr/Util/GuardInterface.h>
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    typedef unique_ptr<util::GuardInterface> GuardPtr;
} // namespace util
} // namespace osvr

#endif // INCLUDED_GuardPtr_h_GUID_612A1083_3E2E_4790_3729_08D5AACEF5E6
