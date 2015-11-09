/** @file
    @brief Header containing the `serializationDescriptor` function templates
   that serve to describe (for code gen purposes) the contents of PathElement
   types.

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

#ifndef INCLUDED_PathElementSerializationDescriptions_h_GUID_88DACA90_818A_4B81_9655_1177F10E8142
#define INCLUDED_PathElementSerializationDescriptions_h_GUID_88DACA90_818A_4B81_9655_1177F10E8142

// Internal Includes
#include <osvr/Common/PathElementTypes.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace common {
    namespace {

        /// @brief "using" statement to combine/simplify the enable_if test for
        /// an element type's serialization.
        template <typename Input, typename Known>
        using enable_if_element_type = typename std::enable_if<std::is_same<
            typename std::remove_const<Input>::type, Known>::value>::type;

        /// @brief Description for DeviceElement
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::DeviceElement>
        serializationDescription(Functor &f, ValType &value) {
            f("device_name", value.getDeviceName());
            f("server", value.getServer());
            f("descriptor", value.getDescriptor());
        }

        /// @brief Description for AliasElement
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::AliasElement>
        serializationDescription(Functor &f, ValType &value) {
            f("source", value.getSource());
            f("priority", value.priority());
        }

        /// @brief Description for ArticulationElement
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::ArticulationElement>
        serializationDescription(Functor &f, ValType &value) {
            f("articulationName", value.getArticulationType());
            f("boneName", value.getBoneName());
        }

        /// @brief Description for StringElement
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::StringElement>
        serializationDescription(Functor &f, ValType &value) {
            f("string", value.getString());
        }

        // Descriptions for elements without extra data
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::NullElement>
        serializationDescription(Functor &, ValType &) {}
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::PluginElement>
        serializationDescription(Functor &, ValType &) {}
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::InterfaceElement>
        serializationDescription(Functor &, ValType &) {}
        template <typename Functor, typename ValType>
        inline enable_if_element_type<ValType, elements::SensorElement>
        serializationDescription(Functor &, ValType &) {}

    } // namespace
} // namespace common
} // namespace osvr
#endif // INCLUDED_PathElementSerializationDescriptions_h_GUID_88DACA90_818A_4B81_9655_1177F10E8142
