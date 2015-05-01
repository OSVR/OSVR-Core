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

#ifndef INCLUDED_SerializationTraitExample_h_GUID_7F76F312_BA0F_4EAB_B659_B2B6C6DF67ED
#define INCLUDED_SerializationTraitExample_h_GUID_7F76F312_BA0F_4EAB_B659_B2B6C6DF67ED

// Internal Includes
#include <osvr/Common/SerializationTraits.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    // Dummy example for your type.
    struct YourType {
        double A;
        uint32_t B;
        int16_t C;
    };

    namespace serialization {
        /// @brief Default serialization traits for YourType - imagine it's got
        /// 3 members, A, B, and C.
        template <>
        struct SerializationTraits<DefaultSerializationTag<YourType>, void>
            : BaseSerializationTraits<YourType> {

            typedef BaseSerializationTraits<YourType> Base;
            typedef DefaultSerializationTag<YourType> tag_type;

            template <typename BufferType>
            static void serialize(BufferType &buf,
                                  typename Base::param_type val,
                                  tag_type const &) {
                serializeRaw(buf, val.A);
                serializeRaw(buf, val.B);
                serializeRaw(buf, val.C);
            }

            template <typename BufferReaderType>
            static void deserialize(BufferReaderType &buf,
                                    typename Base::reference_type val,
                                    tag_type const &) {
                deserializeRaw(buf, val.A);
                deserializeRaw(buf, val.B);
                deserializeRaw(buf, val.C);
            }

            static size_t spaceRequired(size_t existingBytes,
                                        typename Base::param_type val,
                                        tag_type const &) {
                auto requiredA =
                    getBufferSpaceRequiredRaw(existingBytes, val.A);
                auto requiredB =
                    getBufferSpaceRequiredRaw(existingBytes + requiredA, val.B);
                auto requiredC =
                    getBufferSpaceRequiredRaw(existingBytes + requiredB, val.B);
                return requiredC;
            }
        };

    } // namespace serialization

} // namespace common
} // namespace osvr

#endif // INCLUDED_SerializationTraitExample_h_GUID_7F76F312_BA0F_4EAB_B659_B2B6C6DF67ED
