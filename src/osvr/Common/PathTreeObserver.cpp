/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/PathTreeObserver.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    void PathTreeObserver::notifyEvent(
        PathTreeEvents e, PathTreeObserver::callback_argument arg) const {
        auto it = m_handlers.find(e);
        if (end(m_handlers) != it) {
            auto &callback = (*it).second;
            if (callback) {
                callback(arg);
            }
        }
    }

    void PathTreeObserver::setEventCallback(
        PathTreeEvents e, PathTreeObserver::callback_type const &callback) {
        m_handlers[e] = callback;
    }

} // namespace common
} // namespace osvr
