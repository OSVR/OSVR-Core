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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_PassthruUniqueContainer_h_GUID_AD5E86CA_230D_4BBA_8DF0_F2C3811CFD00
#define INCLUDED_PassthruUniqueContainer_h_GUID_AD5E86CA_230D_4BBA_8DF0_F2C3811CFD00

// Internal Includes
#include <osvr/Util/ContainerWrapper.h>

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>
#include <functional>

namespace osvr {
namespace util {
    /// @brief Policies controlling behavior of UniqueContainer
    namespace unique_container_policies {
        /// @brief A basic policy for use with a vector or similar container,
        /// where you don't expect a lot of additions or removals, and thus
        /// search/insertion/removal speed is not critical.
        ///
        /// push_back is used for insertion, member .erase is used for removal,
        /// and std::find is used for search and presence detection.
        struct PushBack {
            template <typename Container> struct Specialized {
                using policy = Specialized<Container>;
                using iterator = typename Container::iterator;
                using const_iterator = typename Container::const_iterator;
                using value_type = typename Container::value_type;
                using reference = typename Container::reference;
                using rv_reference = typename Container::value_type &&;
                using const_reference = typename Container::const_reference;
                /// @brief Time complexity: O(find) = O(n)
                static bool contains(Container const &c, const_reference v) {
                    return policy::find(c, v) != end(c);
                }

                /// @brief Time complexity: O(find) + O(member erase), which is
                /// O(n) (+ amortized constant time) for a std::vector
                static bool remove(Container &c, const_reference v) {
                    auto it = policy::find(c, v);
                    if (end(c) != it) {
                        c.erase(it);
                        return true;
                    }
                    return false;
                }

                /// @brief Insert from a const reference. Time complexity:
                /// O(contains) + O(member push_back), which is O(n) (+
                /// amortized constant time) for a std::vector
                static bool insert(Container &c, const_reference v) {
                    if (!policy::contains(c, v)) {
                        c.push_back(v);
                        return true;
                    }
                    return false;
                }

                /// @brief Insert from an rvalue-reference (move-insert). Time
                /// complexity: O(contains) + O(member emplace_back), which is
                /// O(n) (+ amortized constant time) for a std::vector
                static bool insert(Container &c, rv_reference v) {
                    if (!policy::contains(c, v)) {
                        c.emplace_back(v); // move-construct
                        return true;
                    }
                    return false;
                }

                /// @brief Time complexity: O(std::find) = O(n)
                static auto find(Container const &c, const_reference v)
                    -> decltype(std::find(begin(c), end(c), v)) {
                    return std::find(begin(c), end(c), v);
                }

                /// @brief Time complexity: O(std::find) = O(n)
                static auto find(Container &c, const_reference v)
                    -> decltype(std::find(begin(c), end(c), v)) {
                    return std::find(begin(c), end(c), v);
                }
            };
        };

        /// @brief A basic policy for use with a vector or similar container,
        /// where you have a comparison operator and thus can keep it sorted,
        /// for much faster testing.
        ///
        /// Comparison operator defaults to and is described as <.
        /// Note that !(A < B) && !(B < A) is used to imply A == B
        ///
        /// lower_bound and member .insert is used for insertion, lower_bound
        /// and member .erase is used for removal, and binary_search is used for
        /// presence detection
        template <template <class X> class Comparison = std::less>
        struct SortedInsert {
            template <typename Container> struct Specialized {
                using policy = Specialized<Container>;
                using iterator = typename Container::iterator;
                using const_iterator = typename Container::const_iterator;
                using value_type = typename Container::value_type;
                using reference = typename Container::reference;
                using rv_reference = typename Container::value_type &&;
                using const_reference = typename Container::const_reference;
                using comparator = Comparison<value_type>;

                /// @brief Helper function to avoid vexing parse
                static comparator get_comparator() { return comparator{}; }

                /// @brief Helper function. Wraps both the range aspects and the
                /// comparator for easier use of the underlying
                /// std::lower_bound.
                /// For non-const containers.
                /// O(lg n) comparisons performed
                static auto lower_bound(Container &c, const_reference v)
                    -> decltype(std::lower_bound(begin(c), end(c), v,
                                                 policy::get_comparator())) {
                    return std::lower_bound(begin(c), end(c), v,
                                            policy::get_comparator());
                }

                /// @brief Helper function. Wraps both the range aspects and the
                /// comparator for easier use of the underlying
                /// std::lower_bound.
                /// For const containers.
                /// O(lg n) comparisons performed
                static auto lower_bound(Container const &c, const_reference v)
                    -> decltype(std::lower_bound(begin(c), end(c), v,
                                                 policy::get_comparator())) {
                    return std::lower_bound(begin(c), end(c), v,
                                            policy::get_comparator());
                }

                /// @brief Part of the interface expected by UniqueContainer.
                /// Time complexity: O(std::binary_search) = O(lg n)
                static bool contains(Container const &c, const_reference v) {
                    return std::binary_search(begin(c), end(c), v,
                                              policy::get_comparator());
                }

                /// @brief Part of the interface expected by UniqueContainer.
                /// Time complexity: O(find) + O(member erase), which is
                /// O(n) (+ amortized constant time) for a std::vector
                static bool remove(Container &c, const_reference v) {
                    auto it = policy::find(c, v);
                    if (end(c) != it) {
                        c.erase(it);
                        return true;
                    }
                    return false;
                }

                /// @brief Part of the interface expected by UniqueContainer.
                /// Insert from a const reference. Time complexity:
                /// O(lower_bound) + O(insert), which is O(lg n) +
                /// O(n), so O(n) (for sliding things out of the way) for a
                /// std::vector
                static bool insert(Container &c, const_reference v) {
                    auto it = policy::lower_bound(c, v);
                    if (!policy::iter_indicates_contains(c, it, v)) {
                        c.insert(it, v);
                        return true;
                    }
                    return false;
                }

                /// @brief Part of the interface expected by UniqueContainer.
                /// Insert from an rvalue-reference (move-insert). Time
                /// complexity: O(lower_bound) + O(insert), which is O(lg n) +
                /// O(n), so O(n) (for sliding things out of the way) for a
                /// std::vector
                static bool insert(Container &c, rv_reference v) {
                    auto it = policy::lower_bound(c, v);
                    if (!policy::iter_indicates_contains(c, it, v)) {
                        c.emplace(it, v); // move-construct
                        return true;
                    }
                    return false;
                }

                /// @brief Helper function. Time complexity: O(std::lower_bound)
                /// = O(lg n)
                /// Based on lower_bound with an additional check for
                /// equivalence.
                static auto find(Container const &c, const_reference v)
                    -> decltype(policy::lower_bound(c, v)) {
                    auto it = policy::lower_bound(c, v);
                    return policy::iter_indicates_contains(c, it, v) ? it
                                                                     : end(c);
                }

                /// @brief Helper function. Time complexity: O(std::lower_bound)
                /// = O(lg n)
                /// Based on lower_bound with an additional check for
                /// equivalence.
                static auto find(Container &c, const_reference v)
                    -> decltype(policy::lower_bound(c, v)) {
                    auto it = policy::lower_bound(c, v);
                    return policy::iter_indicates_contains(c, it, v) ? it
                                                                     : end(c);
                }

                /// @brief Helper function. !(A < B) && !(B < A) is used to
                /// imply A == B
                /// O(1) comparisons performed
                static bool check_equiv(const_reference a, const_reference b) {
                    auto comp = policy::get_comparator();
                    return !(comp(a, b)) && !(comp(b, a));
                }

                /// @brief Helper function. Look at an iterator, for instance,
                /// from lower_bound, and determine if it implies the container
                /// contains the value provided.
                template <typename IteratorType>
                static bool iter_indicates_contains(Container const &c,
                                                    IteratorType const &it,
                                                    const_reference v) {
                    if (end(c) == it) {
                        return false;
                    }
                    return check_equiv(*it, v);
                }

                /// @brief Helper function.
                static void sort(Container &c) {
                    std::sort(begin(c), end(c), policy::get_comparator());
                }
            };
        };
#if 0
        struct SortedInsert {
            template <typename Container>
            using Specialized = SortedInsertCustomCompare<
                std::less>::template Specialized<Container>;
        };
#endif
    } // namespace unique_container_policies

    /// @brief A policy-based generic "Unique Container", that wraps
    /// ContainerWrapper (and thus an underlying container) with set-like
    /// semantics/invariant: at most one instance of a value is in the container
    /// at a time.
    ///
    /// Intended as a base class, provides all its functionality in protected
    /// members.
    ///
    /// @tparam Container underlying container (such as std::vector<value_type>)
    /// @tparam Policy a type from osvr::util::unique_container_policies,
    /// defaulting to osvr::util::unique_container_policies::PushBack, that
    /// affects container interaction and performance, and may add additional
    /// invariants (such as ordering).
    /// @tparam WrapperArgs optional arguments to forward to
    /// osvr::util::ContainerWrapper
    ///
    /// Blocks direct non-const access to the underlying container to maintain
    /// the class invariants.
    ///
    /// For all methods, time complexity depends on the policy selected.
    ///
    /// @sa osvr::util::unique_container_policies, osvr::util::ContainerWrapper,
    /// osvr::util::container_policies
    template <typename Container,
              typename Policy = unique_container_policies::PushBack,
              typename... WrapperArgs>
    class UniqueContainerBase
        : public ContainerWrapper<Container, WrapperArgs...> {
        typedef typename Policy::template Specialized<Container> policy;
        typedef ContainerWrapper<Container, WrapperArgs...> base;

      protected:
        using value_type = typename Container::value_type;
        using reference = typename Container::reference;
        using rv_reference = typename Container::value_type &&;
        using const_reference = typename Container::const_reference;
        /// @brief Returns true iff the underlying container contains an
        /// instance of the given value.
        bool contains(const_reference v) {
            return policy::contains(container(), v);
        }
        bool insert(const_reference v) {
            return policy::insert(container(), v);
        }
        bool insert(rv_reference v) { return policy::insert(container(), v); }
        bool remove(const_reference v) {
            return policy::remove(container(), v);
        }

        /// @brief Const access to the container is permitted.
        Container const &container() const { return base::container(); }

      private:
        /// @brief Non-const access to the container, even from the derived
        /// class, is forbidden to maintain class invariant.
        Container &container() { return base::container(); }
    };

    /// @brief A "Unique Container" designed for composition, not inheritance.
    ///
    /// @sa UniqueContainerBase
    template <typename Container,
              typename Policy = unique_container_policies::PushBack,
              typename... WrapperArgs>
    class UniqueContainer
        : public UniqueContainerBase<Container, Policy, WrapperArgs...> {
        typedef UniqueContainerBase<Container, Policy, WrapperArgs...> base;

      public:
        using rv_reference = typename Container::value_type &&;
        using const_reference = typename Container::const_reference;
        bool contains(const_reference v) { return base::contains(v); }
        bool insert(const_reference v) { return base::insert(v); }
        bool insert(rv_reference v) { return base::insert(v); }
        bool remove(const_reference v) { return base::remove(v); }

        /// @brief Const access to the container is permitted.
        Container const &container() const { return base::container(); }
    };

} // namespace util
} // namespace osvr
#endif // INCLUDED_PassthruUniqueContainer_h_GUID_AD5E86CA_230D_4BBA_8DF0_F2C3811CFD00
