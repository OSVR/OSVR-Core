/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Routing/PathElementTools.h>
#include <osvr/Routing/PathElementTypes.h>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace routing {
    namespace elements {
        namespace detail {
            /// Class template, specialized to implement class name retrieval.
            template <typename ElementType> struct ElementTypeName {
                static const char *get();
            };

/// @brief Macro defining a specialization of ElementTypeName to return the type
/// name as a string literal.
#define OSVR_ROUTING_TYPENAME_HANDLER(CLASS)                                   \
    template <> struct ElementTypeName<CLASS> {                                \
        OSVR_ROUTING_EXPORT static const char *get() { return #CLASS; }        \
    };

            /// All types included in the bounded typelist of PathElement must
            /// be in this list.
            OSVR_ROUTING_TYPENAME_HANDLER(NullElement)
            OSVR_ROUTING_TYPENAME_HANDLER(PluginElement)
            OSVR_ROUTING_TYPENAME_HANDLER(DeviceElement)
            OSVR_ROUTING_TYPENAME_HANDLER(InterfaceElement)
            OSVR_ROUTING_TYPENAME_HANDLER(SensorElement)
            OSVR_ROUTING_TYPENAME_HANDLER(PhysicalAssociationElement)
            OSVR_ROUTING_TYPENAME_HANDLER(LogicalElement)
#undef OSVR_ROUTING_TYPENAME_HANDLER
        } // namespace detail

        namespace {
            /// @brief Visitor class used to help getTypeName()
            class TypeNameVisitor : public boost::static_visitor<const char *> {
              public:
                template <typename ElementType>
                const char *operator()(ElementType const &) const {
                    return detail::ElementTypeName<ElementType>::get();
                }
            };
        } // namespace

        const char *getTypeName(PathElement const &elt) {
            return boost::apply_visitor(TypeNameVisitor(), elt);
        }

        void ifNullReplaceWith(PathElement &dest, PathElement const &src) {
            if (boost::get<NullElement>(&dest)) {
                dest = src;
            }
        }
    } // namespace elements
} // namespace routing
} // namespace osvr
