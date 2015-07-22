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
#include <osvr/Common/GeneralizedTransform.h>
#include <osvr/Common/RoutingKeys.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    GeneralizedTransform::GeneralizedTransform() {}
    GeneralizedTransform::GeneralizedTransform(Json::Value const &transform) {
        nest(transform);
    }

    void GeneralizedTransform::nest(Json::Value const &transform) {
        if (!transform.isObject()) {
            /// We only nest objects, sorry.
            return;
        }
        auto currentPtr = &transform;
        while (currentPtr->isObject() &&
               currentPtr->isMember(routing_keys::child())) {
            auto const &current = *currentPtr;
            pushLevelBack(current);
            currentPtr = &(current[routing_keys::child()]);
        }
    }

    void GeneralizedTransform::wrap(Json::Value const &transform) {
        Json::Value newLayer{transform};
        if (newLayer.isObject()) {
            container().insert(begin(), newLayer);
        }
    }

    bool GeneralizedTransform::empty() const { return container().empty(); }

    Json::Value GeneralizedTransform::get() const { return recompose(); }

    Json::Value GeneralizedTransform::get(std::string const &leaf) const {
        return recompose(leaf.empty() ? Json::nullValue : Json::Value(leaf));
    }

    void GeneralizedTransform::pushLevelBack(Json::Value level) {
        if (level.isMember(routing_keys::child())) {
            level.removeMember(routing_keys::child());
        }
        container().emplace_back(std::move(level));
    }
    namespace {
        template <typename Container> class ReverseFacade {
          public:
            ReverseFacade(Container const &c) : m_container(c) {}
            // nonassignable
            ReverseFacade &operator=(ReverseFacade const &) = delete;

            using const_iterator = typename Container::const_reverse_iterator;
            const_iterator begin() const { return m_container.rbegin(); }
            const_iterator end() const { return m_container.rend(); }

          private:
            Container const &m_container;
        };
        template <typename Container>
        inline ReverseFacade<Container> reverse(Container const &c) {
            return ReverseFacade<Container>(c);
        }
    } // namespace
    Json::Value GeneralizedTransform::recompose(Json::Value leaf) const {
        if (container().empty()) {
            return leaf;
        }
        Json::Value state{leaf};
        for (auto const &level : reverse(container())) {
            Json::Value nextLevel{level};
            if (!state.isNull()) {
                nextLevel[routing_keys::child()] = state;
            }
            state = std::move(nextLevel);
        }
        return state;
    }
} // namespace common
} // namespace osvr