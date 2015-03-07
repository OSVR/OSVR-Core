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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ConnectionWrapper_h_GUID_84960342_5DC0_40B2_E9D0_1E479E85D15B
#define INCLUDED_ConnectionWrapper_h_GUID_84960342_5DC0_40B2_E9D0_1E479E85D15B

// Internal Includes
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <vrpn_ConnectionPtr.h>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    class ConnectionWrapper {
      public:
        ConnectionWrapper(vrpn_ConnectionPtr conn);

      private:
        vrpn_ConnectionPtr m_conn;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_ConnectionWrapper_h_GUID_84960342_5DC0_40B2_E9D0_1E479E85D15B
