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

#ifndef INCLUDED_PathTreeObserver_h_GUID_229F0106_AC97_4997_0239_0900C9CB9A54
#define INCLUDED_PathTreeObserver_h_GUID_229F0106_AC97_4997_0239_0900C9CB9A54

// Internal Includes
#include <osvr/Common/PathTree_fwd.h>
#include <osvr/Common/PathTreeObserverPtr.h>
#include <osvr/Common/Export.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <map>
#include <functional>

namespace osvr {
namespace common {
    class PathTreeOwner;
    enum class PathTreeEvents : std::size_t { AboutToUpdate, AfterUpdate };
    class PathTreeObserver : public boost::noncopyable {
      public:
        using callback_argument = PathTree &;
        void notifyEvent(PathTreeEvents e, callback_argument arg) const;

        using callback_type = std::function<void(callback_argument)>;
        OSVR_COMMON_EXPORT void setEventCallback(PathTreeEvents e,
                                                 callback_type const &callback);

      protected:
        PathTreeObserver() = default;

      private:
        std::map<PathTreeEvents, callback_type> m_handlers;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_PathTreeObserver_h_GUID_229F0106_AC97_4997_0239_0900C9CB9A54
