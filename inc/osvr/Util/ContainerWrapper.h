/** @file
    @brief Header providing a class template suitable for inheritance that wraps
   an arbitrary STL-like container and providing

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

#ifndef INCLUDED_ContainerWrapper_h_GUID_87C9C3C2_1A4E_4BA8_A160_96145B60CEDB
#define INCLUDED_ContainerWrapper_h_GUID_87C9C3C2_1A4E_4BA8_A160_96145B60CEDB

// Internal Includes
#include <osvr/Util/TypePack.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Flags or arguments to specify what features you want
    /// ContainerWrapper to provide publicly. Pass as template args to
    /// ContainerWrapper to request a feature.
    namespace container_policies {
        /// @brief Request non-const begin() and end() (as well as their const
        /// counterparts: implies const_iterators) publicly accessible and
        /// forwarded to the container.
        struct iterators;
        /// @brief Request const begin() and end() (as well as cbegin() and
        /// cend())
        /// publicly accessible and forwarded to the container.
        struct const_iterators;
        /// @brief Request size() to be publicly accessible and forwarded to the
        /// container.
        struct size;
    } // namespace container_policies

    namespace detail {
        /// @brief Terminal base class for container wrapper functionality.
        template <typename Container> class ContainerWrapperBase {
          public:
            typedef typename Container::value_type value_type;
            typedef typename Container::reference reference;
            typedef typename Container::const_reference const_reference;
            typedef typename Container::size_type size_type;
            typedef typename Container::iterator iterator;
            typedef typename Container::const_iterator const_iterator;

          protected:
            Container &container() { return m_container; }
            Container const &container() const { return m_container; }

          private:
            Container m_container;
        };

        /// @brief Container wrapper for consumers that want const iterator
        /// methods.
        template <typename Container>
        class ContainerWrapperConstIterators
            : public ContainerWrapperBase<Container> {
          private:
            typedef ContainerWrapperBase<Container> Base;

          public:
            typedef typename Container::const_iterator const_iterator;
            const_iterator begin() const { return Base::container().begin(); }
            const_iterator cbegin() const { return Base::container().cbegin(); }
            const_iterator end() const { return Base::container().end(); }
            const_iterator cend() const { return Base::container().cend(); }
        };

        /// @brief Container wrapper for consumers that want iterator methods,
        /// implies also const-iterator methods.
        template <typename Container>
        class ContainerWrapperIterators
            : public ContainerWrapperConstIterators<Container> {
          private:
            typedef ContainerWrapperConstIterators<Container> Base;

          public:
            typedef typename Container::iterator iterator;
            iterator begin() { return Base::container().begin(); }
            iterator end() { return Base::container().end(); }
        };

        /// @brief Container wrapper mixin for consumers that want size.
        template <typename Container, typename Base>
        class ContainerWrapperSize : public Base {
          public:
            typedef typename Container::size_type size_type;
            size_type size() const { return Base::container().size(); }
        };

        /// @brief Helper metafunction for computing which class the container
        /// wrapper should derive from to get the desired features.
        template <typename Container, typename ArgList>
        struct ComputeContainerWrapper {
            // Determine if we want iterators or const iterators, or neither, as
            // the base class
            using wants_iterators =
                typepack::contains<ArgList, container_policies::iterators>;
            using wants_const_iterators =
                typepack::contains<ArgList,
                                   container_policies::const_iterators>;

            // Evaluates to one of ContainerWrapperIterators,
            // ContainerWrapperConstIterators, or ContainerWrapperBase, which
            // act as a base for any further functionality.
            using iterator_base_type = typepack::if_<
                wants_iterators, ContainerWrapperIterators<Container>,
                typepack::if_<wants_const_iterators,
                              ContainerWrapperConstIterators<Container>,
                              ContainerWrapperBase<Container>>>;

            // Determine if we stick size on the inheritance list as well.
            using wants_size =
                typepack::contains<ArgList, container_policies::size>;
            // Either evaluates to a ContainerWrapperSize inheriting from
            // iterator_base_type, or iterator_base_type itself.
            using type = typepack::if_<
                wants_size, ContainerWrapperSize<Container, iterator_base_type>,
                iterator_base_type>;
        };

        template <typename Container, typename... Args>
        using ContainerWrapper_t = typepack::t_<detail::ComputeContainerWrapper<
            Container, typepack::list<Args...>>>;
    } // namespace detail

    /// @brief Parent class to inherit from to get a container with some
    /// functionality exposed.
    /// @tparam Container the underlying container you want, reference
    /// accessible via Base::container()
    /// @tparam Args 0 or more tag types from container_policies indicating the
    /// features you want publicly exposed
    template <typename Container, typename... Args>
    using ContainerWrapper = detail::ContainerWrapper_t<Container, Args...>;

} // namespace util
} // namespace osvr

#endif // INCLUDED_ContainerWrapper_h_GUID_87C9C3C2_1A4E_4BA8_A160_96145B60CEDB
