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

#ifndef INCLUDED_NetworkClassOfService_h_GUID_5B1253E0_C690_4891_5C84_3F713099B8BE
#define INCLUDED_NetworkClassOfService_h_GUID_5B1253E0_C690_4891_5C84_3F713099B8BE

// Internal Includes
#include <osvr/Util/StdInt.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace common {
    /// @brief Contains tag types for requesting different classes of service
    /// from the networking-based underlying transport (VRPN)
    namespace class_of_service {
        /// @brief Reliable: mandatory in-order, acknowledged delivery, with a
        /// larger message size limit.
        ///
        /// When using networking, this maps to TCP as the underlying transport.
        /// Note that the TCP connection is optimized to minimize overhead
        /// despite being TCP.
        ///
        /// Corresponds to vrpn_CONNECTION_FIXED_LATENCY
        struct Reliable;

        /// @brief Fixed latency service: this class/flag is likely ignored.
        ///
        /// Corresponds to vrpn_CONNECTION_FIXED_LATENCY
        struct FixedLatency;

        /// @brief Low latency, with the caveats of smaller capacity per message
        /// and the possibility of dropped or out of order messages.
        ///
        /// When a UDP connection is available, this maps to UDP as the
        /// underlying transport. Note that it doesn't necessarily guarantee UDP
        /// transport and lower latency, depending on the environment.
        ///
        /// Corresponds to vrpn_CONNECTION_LOW_LATENCY
        struct LowLatency;

        /// @brief Fixed throughput service: this class/flag is likely ignored.
        ///
        /// Corresponds to vrpn_CONNECTION_FIXED_THROUGHPUT
        struct FixedThroughput;

        /// @brief High throughput service: this class/flag is likely ignored.
        ///
        /// Corresponds to vrpn_CONNECTION_HIGH_THROUGHPUT
        struct HighThroughput;

        /// @brief CRTP base for classes of service, useful for accepting
        /// classes of service for an argument without letting just any old type
        /// through.
        template <typename Derived> struct ClassOfService;

        /// @brief Type trait like std::is_base_of indicating whether the given
        /// type is a recognized class of service.
        template <typename T> struct IsClassOfService;

        /// @brief Given a class of service, returns a m
        template <typename Derived>
        size_t getMessageSizeLimit(ClassOfService<Derived> const &);

        /// @brief Association of class of service types with integral
        /// constants.
        ///
        /// Values from vrpn_Connection.h
        template <typename T> struct VRPNConnectionValue;

        template <>
        struct VRPNConnectionValue<Reliable>
            : std::integral_constant<uint32_t, (1 << 0)> {};
        template <>
        struct VRPNConnectionValue<FixedLatency>
            : std::integral_constant<uint32_t, (1 << 1)> {};
        template <>
        struct VRPNConnectionValue<LowLatency>
            : std::integral_constant<uint32_t, (1 << 2)> {};
        template <>
        struct VRPNConnectionValue<FixedThroughput>
            : std::integral_constant<uint32_t, (1 << 3)> {};
        template <>
        struct VRPNConnectionValue<HighThroughput>
            : std::integral_constant<uint32_t, (1 << 4)> {};

        /* -----implementation follows----- */

        /// @brief Implementation details
        namespace detail {
            /// @brief Base class, do not use directly
            struct ClassOfServiceRoot {
              protected:
                ClassOfServiceRoot() = default;
            };

            template <typename SpecificClass> struct GetMessageSizeLimit {
                static size_t get();
            };
        } // namespace detail

        template <typename Derived>
        struct ClassOfService : detail::ClassOfServiceRoot {
            typedef Derived type;
            typedef ClassOfService<Derived> base_type;

          protected:
            ClassOfService();
        };
        struct Reliable : ClassOfService<Reliable> {};

        struct FixedLatency : ClassOfService<FixedLatency> {};

        struct LowLatency : ClassOfService<LowLatency> {};

        struct FixedThroughput : ClassOfService<FixedThroughput> {};

        struct HighThroughput : ClassOfService<HighThroughput> {};

        template <typename T>
        struct IsClassOfService
            : std::is_base_of<detail::ClassOfServiceRoot, T> {};

        template <typename Derived>
        inline size_t getMessageSizeLimit(ClassOfService<Derived> const &) {
            return detail::GetMessageSizeLimit<Derived>::get();
        }

        /// This inline implementation MUST remain at the bottom of this
        /// file. It consists entirely of compile time checks, so it is
        /// effectively removed from the code once the conditions are
        /// verified.
        template <typename Derived>
        inline ClassOfService<Derived>::ClassOfService() {
            /// Partially enforce the Curiously-Recurring Template Pattern.
            /// The assertion here is that for some `ClassOfService<X>`,
            /// there exists a `class X : public ClassOfService<X> {};`
            /// Doesn't prevent inheriting from the wrong base (`class X :
            /// public ClassOfService<Y> {};` where there is already a
            /// `class Y : public ClassOfService<Y> {};`)
            static_assert(
                std::is_base_of<base_type, type>::value,
                "ClassOfService<T> must be the base of a class of service "
                "type T (the CRTP)!");
        }

    } // namespace class_of_service
} // namespace common
} // namespace osvr
#endif // INCLUDED_NetworkClassOfService_h_GUID_5B1253E0_C690_4891_5C84_3F713099B8BE
