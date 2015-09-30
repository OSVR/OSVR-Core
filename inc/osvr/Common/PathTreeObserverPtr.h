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

#ifndef INCLUDED_PathTreeObserverPtr_h_GUID_4ECB1A3C_D7B5_41B1_6A74_E88657469ED2
#define INCLUDED_PathTreeObserverPtr_h_GUID_4ECB1A3C_D7B5_41B1_6A74_E88657469ED2

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/SharedPtr.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    class PathTreeObserver;
    typedef shared_ptr<PathTreeObserver> PathTreeObserverPtr;
    typedef weak_ptr<PathTreeObserver> PathTreeObserverWeakPtr;
} // namespace common
} // namespace osvr

#endif // INCLUDED_PathTreeObserverPtr_h_GUID_4ECB1A3C_D7B5_41B1_6A74_E88657469ED2
