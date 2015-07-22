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

#ifndef INCLUDED_GeneralizedTransform_h_GUID_BEA0F0A8_AC82_4093_60F5_34597B06F3A9
#define INCLUDED_GeneralizedTransform_h_GUID_BEA0F0A8_AC82_4093_60F5_34597B06F3A9

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Util/ContainerWrapper.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
#include <vector>
#include <algorithm>

namespace osvr {
namespace common {
    /// @brief Container for easy manipulation of nested transforms.
    ///
    /// Stores the levels of the transform in outer-inner order, with the
    /// "child" key/value removed and only re-created in get().
    class GeneralizedTransform
        : public util::ContainerWrapper<std::vector<Json::Value>,
                                        util::container_policies::iterators,
                                        util::container_policies::size> {
      public:
#if 0
        /// @name Type definitions
        /// @{
        typedef Json::Value value_type;
        typedef value_type &reference;
        typedef value_type const &const_reference;
        typedef std::vector<value_type> container;
        typedef container::size_type size_type;
        typedef container::iterator iterator;
        typedef container::const_iterator const_iterator;
        using Base = util::ContainerWrapper<std::vector<Json::Value>,
                                            util::container_policies::iterators,
                                            util::container_policies::size>;
        /// @}

#endif

        /// @brief Empty transformation
        OSVR_COMMON_EXPORT GeneralizedTransform();

        /// @brief Construct from a normalized alias/transform as Json. Must be
        /// one or more levels of nested objects (under the "child" key) with
        /// other arbitrary data in the object describing the transformation
        /// taking place at that level.
        OSVR_COMMON_EXPORT GeneralizedTransform(Json::Value const &transform);

        /// @brief Process the given transform as "inner" to any existing
        /// transform.
        OSVR_COMMON_EXPORT void nest(Json::Value const &transform);

        /// @brief Wrap a single new layer of transform around the existing
        /// ones, if any.
        OSVR_COMMON_EXPORT void wrap(Json::Value const &transform);

        /// @brief Is this an empty transform?
        OSVR_COMMON_EXPORT bool empty() const;

        /// @brief Get a "reconstituted" version of the transformation.
        OSVR_COMMON_EXPORT Json::Value get() const;

        /// @brief Get the reconstituted transformation with the specified leaf
        /// as the final child.
        ///
        /// If leaf is empty, behaves like get()
        OSVR_COMMON_EXPORT Json::Value get(std::string const &leaf) const;

        /// @brief A shrinking resize.
        void resize(const_iterator newEnd) {
            container().resize(newEnd - begin());
        }

      private:
        void pushLevelBack(Json::Value level);
        Json::Value recompose(Json::Value leaf = Json::nullValue) const;
    };

    /// @brief Remove levels from a generalized transform as dictated by an
    /// arbitrary predicate.
    ///
    /// Acts just like std::remove_if except that it also resizes the container.
    template <typename UnaryPredicate>
    inline void remove_if(GeneralizedTransform &transform,
                          UnaryPredicate pred) {
        auto newEnd = std::remove_if(transform.begin(), transform.end(), pred);
        transform.resize(newEnd);
    }

} // namespace common
} // namespace osvr
#endif // INCLUDED_GeneralizedTransform_h_GUID_BEA0F0A8_AC82_4093_60F5_34597B06F3A9
