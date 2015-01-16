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
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_PathElementTypes_fwd_h_GUID_8EAAAC63_0E7E_4C8C_27E2_C5B5A58A6D0A
#define INCLUDED_PathElementTypes_fwd_h_GUID_8EAAAC63_0E7E_4C8C_27E2_C5B5A58A6D0A

// Internal Includes
// - none

// Library/third-party includes
#include <boost/variant/variant_fwd.hpp>

// Standard includes
// - none

namespace osvr {
namespace routing {
    namespace elements {
        class NullElement;
        class PluginElement;
        class DeviceElement;
        class InterfaceElement;
        class SensorElement;
        class PhysicalAssociationElement;
        class LogicalElement;
        class AliasElement;

/// @brief The variant type containing a particular kind of path
/// element.
/// @ingroup Routing
#ifndef OSVR_DOXYGEN_EXTERNAL
///
/// NOTE: if you add an element type here, you must make sure it's:
///
/// - forward-declared above
/// - declared in PathElementTypes.h
/// - included in the name list in PathElementTools.cpp
#endif
        typedef boost::variant<NullElement, PluginElement, DeviceElement,
                               InterfaceElement, SensorElement,
                               PhysicalAssociationElement, LogicalElement,
                               AliasElement> PathElement;
    } // namespace elements

    using elements::PathElement;
} // namespace routing
} // namespace osvr

#endif // INCLUDED_PathElementTypes_fwd_h_GUID_8EAAAC63_0E7E_4C8C_27E2_C5B5A58A6D0A
