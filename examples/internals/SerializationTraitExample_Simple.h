/** @file
    @brief Header containing an examples of how to implement serialization for
   simple (struct-like) new types.

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

#ifndef INCLUDED_SerializationTraitExample_Simple_h_GUID_1CD49F21_AA18_4DBE_2019_1F40B6695E00
#define INCLUDED_SerializationTraitExample_Simple_h_GUID_1CD49F21_AA18_4DBE_2019_1F40B6695E00

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
    struct YourSimpleType {
        double A;
        uint32_t B;
        int16_t C;
    };

    namespace serialization {
        /// @brief Example of serialization for a simple struct: no special
        /// treatment for any of the variables, just recursively
        /// serialize/deserialize/compute size for each of the members.
        ///
        /// If you specialize SimpleStructSerialization (and inherit from
        /// SimpleStructSerializationBase), the more complex version of the
        /// traits for the "default tag" will automatically be generated for
        /// you.
        ///
        /// If you need more control over the serialization process, you'll need
        /// to start with something like the
        /// SerializationTraitExample_Complicated.h which has a
        /// SerializationTraits explicit specialization.
        template <>
        struct SimpleStructSerialization<YourSimpleType>
            : SimpleStructSerializationBase {
            template <typename F, typename T> static void apply(F &f, T &val) {
                f(val.A);
                f(val.B);
                f(val.C);
            }
        };

    } // namespace serialization

} // namespace common
} // namespace osvr
#endif // INCLUDED_SerializationTraitExample_Simple_h_GUID_1CD49F21_AA18_4DBE_2019_1F40B6695E00
