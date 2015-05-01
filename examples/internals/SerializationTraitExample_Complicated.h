/** @file
    @brief Header containing an examples of how to implement serialization for
   complicated new types.

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

#ifndef INCLUDED_SerializationTraitExample_Complicated_h_GUID_FDF4E42A_9B56_4A86_7FDF_3C6425E6F9FB
#define INCLUDED_SerializationTraitExample_Complicated_h_GUID_FDF4E42A_9B56_4A86_7FDF_3C6425E6F9FB

// Internal Includes
#include <osvr/Common/SerializationTraits.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {

    /// @brief Dummy example for your type.
    ///
    /// Of course, if your type actually was this simple, you'd use the simple
    /// example as your model instead, but...
    struct YourComplicatedType {
        double A;
        uint32_t B;
        int16_t C;
    };
    namespace serialization {
        /// @brief Default serialization traits for YourComplicatedType
        ///
        /// This is the "complex" way of doing things - in case you have some
        /// members you have to treat specially, etc. If your type is literally
        /// just a composition of other known types, like this one is, you're
        /// better off using the SimpleStructSerializer method - see
        /// SerializationTraitExample_Simple.h
        template <>
        struct SerializationTraits<DefaultSerializationTag<YourComplicatedType>,
                                   void>
            : BaseSerializationTraits<YourComplicatedType> {

            typedef BaseSerializationTraits<YourComplicatedType> Base;
            typedef DefaultSerializationTag<YourComplicatedType> tag_type;

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
                                        Base::param_type val,
                                        tag_type const &) {
                size_t bytes = existingBytes;
                bytes += getBufferSpaceRequiredRaw(bytes, val.A);
                bytes += getBufferSpaceRequiredRaw(bytes, val.B);
                bytes += getBufferSpaceRequiredRaw(bytes, val.C);
                return bytes - existingBytes;
            }
        };

    } // namespace serialization

} // namespace common
} // namespace osvr
#endif // INCLUDED_SerializationTraitExample_Complicated_h_GUID_FDF4E42A_9B56_4A86_7FDF_3C6425E6F9FB
