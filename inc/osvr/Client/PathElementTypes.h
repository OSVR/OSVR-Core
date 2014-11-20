/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_PathElementTypes_h_GUID_5CC817E5_C7CB_45AE_399D_0B0D39579374
#define INCLUDED_PathElementTypes_h_GUID_5CC817E5_C7CB_45AE_399D_0B0D39579374

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {
    namespace elements {
#define OSVR_DECLARE_NAME_STRING(CLASS)                                        \
    namespace names {                                                          \
        extern const char CLASS[];                                             \
    }
        /// @brief Base, using the CRTP, providing some basic functionality for
        /// path elements.
        template <typename Type, const char *ElementName> class ElementBase {
          public:
            const char *getType() const { return ElementName; }
        };

        OSVR_DECLARE_NAME_STRING(NullElement)
        /// @brief The element type created when requesting a path that isn't
        /// yet in the tree.
        class NullElement
            : public ElementBase<NullElement, names::NullElement> {};

        OSVR_DECLARE_NAME_STRING(PluginElement)
        /// @brief The element type corresponding to a plugin
        class PluginElement
            : public ElementBase<PluginElement, names::PluginElement> {};

        OSVR_DECLARE_NAME_STRING(DeviceElement)
        /// @brief The element type corresponding to a device, which implements
        /// 0 or more interfaces
        class DeviceElement
            : public ElementBase<DeviceElement, names::DeviceElement> {};

        /// @brief The element type corresponding to a device, which implements
        /// 0 or more interfaces
        OSVR_DECLARE_NAME_STRING(InterfaceElement)
        class InterfaceElement
            : public ElementBase<InterfaceElement, names::InterfaceElement> {};

#undef OSVR_DECLARE_NAME_STRING
    } // namespace elements
} // namespace client
} // namespace osvr

#endif // INCLUDED_PathElementTypes_h_GUID_5CC817E5_C7CB_45AE_399D_0B0D39579374
