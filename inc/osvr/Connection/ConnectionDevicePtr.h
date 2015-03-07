/** @file
    @brief Header forward-declaring ConnectionDevice and specifying the desired
   pointer to hold a ConnectionDevice in.

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

#ifndef INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60
#define INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60

// Internal Includes
#include <osvr/Util/SharedPtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace connection {
    class ConnectionDevice;
    /// @brief How to hold on to a ConnectionDevice
    typedef shared_ptr<ConnectionDevice> ConnectionDevicePtr;
} // namespace connection
} // namespace osvr

#endif // INCLUDED_ConnectionDevicePtr_h_GUID_6D454990_4426_4DAC_27C4_BB2571DF7F60
