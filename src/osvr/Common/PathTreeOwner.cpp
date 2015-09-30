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
#include <osvr/Common/PathTreeOwner.h>
#include <osvr/Common/PathTreeObserver.h>
#include <osvr/Common/PathTreeSerialization.h>

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>
#include <iterator>

namespace osvr {
namespace common {
    namespace {
        /// @brief Class for the sole purpose of being able to use
        /// make_shared with PathTreeObserver.
        class PathTreeObserverConcrete : public PathTreeObserver {
          public:
            PathTreeObserverConcrete() = default;
        };
    } // namespace

    /// Given a container of weak ptrs to some type and a functor that operates
    /// on references to that type, go through all elements of the container in
    /// a single pass, calling the functor on those elements still valid, and
    /// removing those invalid ones. (Uses member function resize with two
    /// iterators)
    template <typename T, typename F>
    inline void for_each_cleanup_pointers(T &c, F &&f) {
        using weak_ptr_type = typename T::value_type;
        auto newEnd =
            std::remove_if(begin(c), end(c), [&](weak_ptr_type const &ptr) {
                auto fullPtr = ptr.lock();
                if (fullPtr) {
                    f(*fullPtr);
                    return false; // don't remove this one
                }
                return true; // this one we couldn't lock - expired observer.
            });
        c.erase(newEnd, end(c));
    }

    PathTreeObserverPtr PathTreeOwner::makeObserver() {
        auto ret = PathTreeObserverPtr{make_shared<PathTreeObserverConcrete>()};
        m_observers.push_back(ret);
        return ret;
    }

    void PathTreeOwner::replaceTree(Json::Value const &nodes) {
        for_each_cleanup_pointers(
            m_observers, [&](PathTreeObserver const &observer) {
                observer.notifyEvent(PathTreeEvents::AboutToUpdate, m_tree);
            });

        m_tree.reset();

        common::jsonToPathTree(m_tree, nodes);

        m_valid = true;

        for_each_cleanup_pointers(
            m_observers, [&](PathTreeObserver const &observer) {
                observer.notifyEvent(PathTreeEvents::AfterUpdate, m_tree);
            });
    }
} // namespace common
} // namespace osvr
