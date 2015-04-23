/** @file
    @brief Header forward-declaring the types in PathElementTypes.h and
   including the PathElement typedef.

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_PathElementTypes_fwd_h_GUID_8EAAAC63_0E7E_4C8C_27E2_C5B5A58A6D0A
#define INCLUDED_PathElementTypes_fwd_h_GUID_8EAAAC63_0E7E_4C8C_27E2_C5B5A58A6D0A

// Internal Includes
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <boost/variant/variant_fwd.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    typedef uint8_t AliasPriority;
    static const AliasPriority ALIASPRIORITY_MINIMUM = 0;
    static const AliasPriority ALIASPRIORITY_AUTOMATIC = 127;
    static const AliasPriority ALIASPRIORITY_SEMANTICROUTE = 128;
    static const AliasPriority ALIASPRIORITY_MANUAL = 255;
    namespace elements {
        // list is kept sorted here for convenience
        class AliasElement;
        class DeviceElement;
        class InterfaceElement;
        class NullElement;
        class PluginElement;
        class SensorElement;
        class StringElement;

/// @brief The variant type containing a particular kind of path element.
#ifndef OSVR_DOXYGEN_EXTERNAL
///
/// NOTE: if you add an element type here, you must make sure it's:
///
/// - forward-declared above
/// - declared in PathElementTypes.h
/// - included in the name list in PathElementTools.cpp
///
/// Note that while most lists of these types are not order sensitive, and so
/// have been sorted, order does matter for the types in this typedef. Above
/// all, NullElement must remain first.
#endif
        typedef boost::variant<NullElement, AliasElement, SensorElement,
                               InterfaceElement, DeviceElement, PluginElement,
                               StringElement> PathElement;
    } // namespace elements

    using elements::PathElement;
} // namespace common
} // namespace osvr

#endif // INCLUDED_PathElementTypes_fwd_h_GUID_8EAAAC63_0E7E_4C8C_27E2_C5B5A58A6D0A
