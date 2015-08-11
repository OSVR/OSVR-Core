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

#ifndef INCLUDED_Policies_h_GUID_84C007F4_C919_479B_5766_E7AEB55FFB8F
#define INCLUDED_Policies_h_GUID_84C007F4_C919_479B_5766_E7AEB55FFB8F

// Internal Includes
#include <osvr/Util/TypePack.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace util {
    /// @brief Base class templates for the use of "policies" - easy ways to
    /// allow varied behavior to be specified for compile time but passed as an
    /// argument. Policies themselves are purely compile-time structures.
    ///
    /// A "Policy Class" refers to one or more policies that might be logically
    /// combined - you'll create a PolicyClass tag type (an undefined struct
    /// will do) each place you use Policies, so different uses of policies
    /// don't mix.
    ///
    /// Each Policy you create is an empty struct that derives from
    /// SinglePolicyBase. When instantiated, it can be added to other policies
    /// with the same PolicyClass tag.
    ///
    /// Function templates that want to accept policies should have one template
    /// parameter (generally called `Derived`) and take an unnamed argument
    /// `osvr::util::policies::PolicyBase<Derived, YourPolicyClass> const &`.
    /// Operations may then be performed on the `Derived` type to investigate
    /// what policies are passed.
    ///
    /// If you want default behavior without having to pass a policy, you might
    /// consider specifying a default argument of type
    /// `EmptyPolicy<YourPolicyClass>`.
    namespace policies {
        /// @brief CRTP base class for all policies, for generic code
        template <typename Derived, typename PolicyClass> struct PolicyBase {
            using derived = Derived;
            using policy_class = PolicyClass;
        };
        namespace detail {
            /// @brief Base for all policy composites
            ///
            /// @tparam PolicyClass a tag type used to indicate policy classes.
            /// @tparam Policies a typepack list of individual policies
            template <typename PolicyClass, typename Policies>
            struct PolicyCompositeBase
                : PolicyBase<PolicyCompositeBase<PolicyClass, Policies>,
                             PolicyClass> {
                using policy_list = Policies;
            };

            template <typename PolicyClass, typename Policy>
            using SinglePolicyComposite =
                PolicyCompositeBase<PolicyClass, typepack::list<Policy>>;

            /// @brief The empty policy composite type for a given policy-class.
            template <typename PolicyClass>
            using EmptyPolicy =
                PolicyCompositeBase<PolicyClass, typepack::list<>>;
        } // namespace detail
        template <typename Derived, typename PolicyClass>
        struct SinglePolicyBase
            : PolicyBase<SinglePolicyBase<Derived, PolicyClass>, PolicyClass> {
            using policy = Derived;
            using policy_class = PolicyClass;
            using policy_list = typepack::list<policy>;
#if 0
            using composite = SinglePolicyComposite<policy_class, policy>;
            /// @brief Implicit conversion to the
            operator composite() const { return composite(); }
#endif
        };
        namespace detail {
            template <typename T> struct GetPolicyListImpl;
            template <typename Derived, typename PolicyClass>
            struct GetPolicyListImpl<SinglePolicyBase<Derived, PolicyClass>> {
                using arg = SinglePolicyBase<Derived, PolicyClass>;
                using type = typename arg::policy_list;
            };

            template <typename PolicyClass, typename Policies>
            struct GetPolicyListImpl<
                PolicyCompositeBase<PolicyClass, Policies>> {
                using arg = PolicyCompositeBase<PolicyClass, Policies>;
                using type = typename arg::policy_list;
            };
        } // namespace detail
        template <typename T>
        using GetPolicyList = typepack::t_<detail::GetPolicyListImpl<T>>;
        template <typename PolicyClass, typename Derived, typename OtherDerived>
        using PolicyConcat =
            detail::PolicyCompositeBase<PolicyClass,
                                typepack::concat<GetPolicyList<Derived>,
                                                 GetPolicyList<OtherDerived>>>;

        template <typename PolicyClass, typename Derived, typename OtherDerived>
        inline PolicyConcat<PolicyClass, Derived, OtherDerived>
        operator+(PolicyBase<Derived, PolicyClass> const &,
                  PolicyBase<OtherDerived, PolicyClass> const &) {
            return PolicyConcat<PolicyClass, Derived, OtherDerived>{};
        }
    }
} // namespace util
} // namespace osvr
#endif // INCLUDED_Policies_h_GUID_84C007F4_C919_479B_5766_E7AEB55FFB8F
