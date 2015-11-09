/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>

// Library/third-party includes
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/mpl/for_each.hpp>

// Standard includes
#include <algorithm>
#include <string.h>

namespace osvr {
namespace common {
    namespace elements {
        namespace {
            /// Class template, specialized to implement class name retrieval.
            template <typename ElementType> struct ElementTypeName {
                static const char *get();
            };

/// @brief Macro defining a specialization of ElementTypeName to return the type
/// name as a string literal.
#define OSVR_ROUTING_TYPENAME_HANDLER(CLASS)                                   \
    template <> struct ElementTypeName<CLASS> {                                \
        static const char *get() { return #CLASS; }                            \
    };

            /// All types included in the bounded typelist of PathElement must
            /// be in this list. It's kept sorted for ease of maintenance.
            OSVR_ROUTING_TYPENAME_HANDLER(AliasElement)
            OSVR_ROUTING_TYPENAME_HANDLER(ArticulationElement)
            OSVR_ROUTING_TYPENAME_HANDLER(DeviceElement)
            OSVR_ROUTING_TYPENAME_HANDLER(InterfaceElement)
            OSVR_ROUTING_TYPENAME_HANDLER(NullElement)
            OSVR_ROUTING_TYPENAME_HANDLER(PluginElement)
            OSVR_ROUTING_TYPENAME_HANDLER(SensorElement)
            OSVR_ROUTING_TYPENAME_HANDLER(StringElement)
#undef OSVR_ROUTING_TYPENAME_HANDLER

            /// @brief Visitor class used to help getTypeName()
            class TypeNameVisitor : public boost::static_visitor<const char *> {
              public:
                template <typename ElementType>
                const char *operator()(ElementType const &) const {
                    return ElementTypeName<ElementType>::get();
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

        bool isNull(PathElement const &elt) {
            return (nullptr != boost::get<NullElement>(&elt));
        }

        namespace {
            class MaxTypeNameLength {
              public:
                MaxTypeNameLength(size_t &maxLen) : m_maxLen(maxLen) {}
                /// @brief no assignment operator
                MaxTypeNameLength &
                operator=(MaxTypeNameLength const &) = delete;
                template <typename T> void operator()(T const &) {
                    m_maxLen =
                        (std::max)(m_maxLen, strlen(ElementTypeName<T>::get()));
                }

              private:
                size_t &m_maxLen;
            };
        } // namespace

        size_t getMaxTypeNameLength() {
            size_t maxLen = 0;
            boost::mpl::for_each<elements::PathElement::types>(
                MaxTypeNameLength(maxLen));
            return maxLen;
        }
    } // namespace elements
} // namespace common
} // namespace osvr
