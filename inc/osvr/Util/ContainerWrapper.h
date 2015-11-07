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
#include <osvr/TypePack/T.h>
#include <osvr/TypePack/List.h>
#include <osvr/TypePack/If.h>
#include <osvr/TypePack/Contains.h>
#include <osvr/TypePack/And.h>
#include <osvr/TypePack/Fold.h>
#include <osvr/TypePack/Not.h>

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
        /// @brief Request operator[] to be publicly accessible and forwarded to
        /// the container. (implies const_subscript)
        struct subscript;
        /// @brief Request the const overload only of operator[] to be publicly
        /// accessible and forwarded to the container.
        struct const_subscript;
        /// @brief Request the empty() method be publicly accessible and
        /// forwarded to the container.
        struct empty;
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
            Container const &ccontainer() const { return m_container; }

          private:
            Container m_container;
        };

        /// @brief Container wrapper for consumers that want const iterator
        /// methods.
        template <typename Container, typename Base>
        class ContainerWrapperConstIterators : public Base {
          public:
            typedef typename Container::const_iterator const_iterator;
            const_iterator begin() const { return Base::container().begin(); }
            const_iterator cbegin() const { return Base::container().cbegin(); }
            const_iterator end() const { return Base::container().end(); }
            const_iterator cend() const { return Base::container().cend(); }
        };

        /// @brief Container wrapper for consumers that want iterator methods,
        /// including also const-iterator methods.
        template <typename Container, typename Base>
        class ContainerWrapperIterators
            : public ContainerWrapperConstIterators<Container, Base> {
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

        /// @brief Container wrapper mixin for consumers that want size.
        template <typename Container, typename Base>
        class ContainerWrapperEmpty : public Base {
          public:
            bool empty() const { return Base::container().empty(); }
        };

        /// @brief Container wrapper mixin for consumers that want const array
        /// subscript operator methods.
        template <typename Container, typename Base>
        class ContainerWrapperConstSubscript : public Base {
          public:
            typedef typename Container::size_type size_type;
            typedef typename Container::const_reference const_reference;
            const_reference operator[](size_type index) const {
                return Base::container()[index];
            }
        };

        /// @brief Container wrapper mixin for consumers that want the array
        /// subscript operator, including also const-array subscript methods.
        template <typename Container, typename Base>
        class ContainerWrapperSubscript : public Base {
          public:
            typedef typename Container::size_type size_type;
            typedef typename Container::reference reference;
            typedef typename Container::const_reference const_reference;
            reference operator[](size_type index) {
                return Base::container()[index];
            }
            const_reference operator[](size_type index) const {
                return Base::container()[index];
            }
        };

        /// @brief Holds the result of evaluating a condition along with the
        /// mixin template to use (as an alias class) if that condition is true.
        template <typename Condition,
                  template <class, class> class ContainerMixin>
        struct Option {
            using condition = Condition;
            template <typename Container, typename Base>
            using apply = ContainerMixin<Container, Base>;
        };

        /// @brief Alias class to use in fold when computing a container wrapper
        template <typename Container> struct ContainerWrapperFold {
            template <typename Base, typename Option> struct apply {
                using condition = typename Option::condition;
                using type = typepack::if_<
                    condition,
                    typepack::apply<Option, Container, Base>,
                    Base>;
            };
        };

        /// @brief Main metafunction used to compute the full type of a
        /// container wrapper.
        template <typename Container, typename ArgList>
        struct ComputeContainerWrapper {
            // Determine if we want iterators or const iterators
            using wants_iterators =
                typepack::contains<ArgList, container_policies::iterators>;
            using wants_const_iterators = typepack::and_<
                typepack::not_<wants_iterators>,
                typepack::contains<ArgList,
                                   container_policies::const_iterators>>;

            // Check for subscript operator requests
            using wants_subscript =
                typepack::contains<ArgList, container_policies::subscript>;
            using wants_const_subscript = typepack::and_<
                typepack::not_<wants_subscript>,
                typepack::contains<ArgList,
                                   container_policies::const_subscript>>;

            // Check for size requests
            using wants_size =
                typepack::contains<ArgList, container_policies::size>;

            // Check for empty requests
            using wants_empty =
                typepack::contains<ArgList, container_policies::empty>;

            // List of conditions and their associated mixin class templates
            using option_list = typepack::list<
                Option<wants_const_iterators, ContainerWrapperConstIterators>,
                Option<wants_iterators, ContainerWrapperIterators>,
                Option<wants_const_subscript, ContainerWrapperConstSubscript>,
                Option<wants_subscript, ContainerWrapperSubscript>,
                Option<wants_size, ContainerWrapperSize>,
                Option<wants_empty, ContainerWrapperEmpty>>;
            using type =
                typepack::fold<option_list, ContainerWrapperBase<Container>,
                               ContainerWrapperFold<Container>>;
        };

        template <typename Container, typename... Args>
        using ContainerWrapper_t = typepack::t_<detail::ComputeContainerWrapper<
            Container, typepack::list<Args...>>>;
    } // namespace detail

    /// @brief Parent class to inherit from to get a container with some
    /// functionality exposed.
    /// @tparam Container the underlying container you want, reference
    /// accessible via Base::container()
    /// @tparam Args 0 or more tag types from osvr::util::container_policies
    /// indicating the features you want publicly exposed
    template <typename Container, typename... Args>
    using ContainerWrapper = detail::ContainerWrapper_t<Container, Args...>;

} // namespace util
} // namespace osvr

#endif // INCLUDED_ContainerWrapper_h_GUID_87C9C3C2_1A4E_4BA8_A160_96145B60CEDB
