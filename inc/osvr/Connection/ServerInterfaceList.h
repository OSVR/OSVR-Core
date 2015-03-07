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

#ifndef INCLUDED_ServerInterfaceList_h_GUID_E759FA5E_EF04_4FAA_8F2D_38887C043140
#define INCLUDED_ServerInterfaceList_h_GUID_E759FA5E_EF04_4FAA_8F2D_38887C043140

// Internal Includes
#include <osvr/Connection/BaseServerInterface.h>

// Library/third-party includes
// - none

// Standard includes
#include <vector>

namespace osvr {
namespace connection {
    typedef std::vector<ServerInterfacePtr> ServerInterfaceList;
} // namespace connection
} // namespace osvr
#endif // INCLUDED_ServerInterfaceList_h_GUID_E759FA5E_EF04_4FAA_8F2D_38887C043140
