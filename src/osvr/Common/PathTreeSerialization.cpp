/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/PathTreeSerialization.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/ApplyPathNodeVisitor.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <json/value.h>
#include <boost/variant.hpp>
#include <boost/mpl/for_each.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace {

        /// @brief Class template (for specialization) allowing serialization
        /// and deserialization code to be generated from the same operations
        /// (ensuring keys stay in sync, etc.)
        template <typename T> class PathElementSerializationHandler {
          public:
            template <typename Functor, typename ValType>
            static void handle(Functor &, ValType &) {}
        };

        /// @brief Specialization for DeviceElement
        template <>
        class PathElementSerializationHandler<elements::DeviceElement> {
          public:
            template <typename Functor, typename ValType>
            static void handle(Functor &f, ValType &value) {
                f("device_name", value.getDeviceName());
                f("server", value.getServer());
            }
        };

        /// @brief Specialization for AliasElement
        template <>
        class PathElementSerializationHandler<elements::AliasElement> {
          public:
            template <typename Functor, typename ValType>
            static void handle(Functor &f, ValType &value) {
                f("source", value.getSource());
                f("priority", value.priority(),
                  ALIASPRIORITY_MINIMUM /* default value */);
            }
        };

        /// @brief Functor for use with PathElementSerializationHandler, for the
        /// direction PathElement->JSON
        class PathElementToJSONFunctor : boost::noncopyable {
          public:
            PathElementToJSONFunctor(Json::Value &val) : m_val(val) {}

            template <typename T>
            void operator()(const char name[], T const &data, ...) {
                m_val[name] = data;
            }

          private:
            Json::Value &m_val;
        };

        /// @brief A PathNodeVisitor that returns a JSON object corresponding to
        /// a single PathNode.
        class PathNodeToJsonVisitor
            : public boost::static_visitor<Json::Value> {
          public:
            PathNodeToJsonVisitor() : boost::static_visitor<Json::Value>() {}

            Json::Value setup(PathNode const &node) {
                Json::Value val{Json::objectValue};
                val["path"] = getFullPath(node);
                val["type"] = getTypeName(node);
                return val;
            }

            template <typename T>
            Json::Value operator()(PathNode const &node, T const &elt) {
                auto ret = setup(node);
                PathElementToJSONFunctor f(ret);
                PathElementSerializationHandler<T>::handle(f, elt);
                return ret;
            }
        };
        Json::Value pathNodeToJson(PathNode const &node) {
            PathNodeToJsonVisitor visitor;
            return applyPathNodeVisitor(visitor, node);
        }
        /// @brief A PathNode (tree) visitor to recursively convert nodes in a
        /// PathTree to JSON
        class PathTreeToJsonVisitor {
          public:
            PathTreeToJsonVisitor(bool keepNulls)
                : m_ret(Json::arrayValue), m_keepNulls(keepNulls) {}

            Json::Value getResult() { return m_ret; }

            void operator()(PathNode const &node) {
                OSVR_DEV_VERBOSE("Visiting " << getFullPath(node));
                if (m_keepNulls || !elements::isNull(node.value())) {
                    // If we're keeping nulls or this isn't a null...
                    m_ret.append(pathNodeToJson(node));
                }
                // Recurse on children
                node.visitConstChildren(*this);
            }

          private:
            Json::Value m_ret;
            bool m_keepNulls;
        };

        /// @brief Functor for use with PathElementSerializationHandler, for the
        /// direction JSON->PathElement
        class PathElementFromJsonFunctor : boost::noncopyable {
          public:
            PathElementFromJsonFunctor(Json::Value const &val) : m_val(val) {}
            void operator()(const char name[], std::string &dataRef) {
                m_requireName(name);
                dataRef = m_val[name].asString();
            }

            void operator()(const char name[], bool &dataRef) {
                m_requireName(name);
                dataRef = m_val[name].asBool();
            }

            void operator()(const char name[], bool &dataRef, bool defaultVal) {
                if (m_hasName(name)) {
                    dataRef = m_val[name].asBool();
                } else {
                    dataRef = defaultVal;
                }
            }

            void operator()(const char name[], uint8_t &dataRef,
                            uint8_t defaultVal) {
                if (m_hasName(name)) {
                    dataRef = static_cast<uint8_t>(m_val[name].asInt());
                } else {
                    dataRef = defaultVal;
                }
            }
            /// @todo add more methods here if other data types are stored
          private:
            void m_requireName(const char name[]) {
                if (!m_hasName(name)) {
                    throw std::runtime_error(
                        "Missing JSON object member named " +
                        std::string(name));
                }
            }
            bool m_hasName(const char name[]) { return m_val.isMember(name); }
            Json::Value const &m_val;
        };

        /// @brief Functor for use with the PathElement's type list and
        /// mpl::for_each, to convert from type name string to actual type and
        /// load the data.
        class DeserializeElementFunctor {
          public:
            DeserializeElementFunctor(Json::Value const &val,
                                      elements::PathElement &elt)
                : m_val(val), m_typename(val["type"].asString()), m_elt(elt) {}

            /// @brief Don't try to generate an assignment operator.
            DeserializeElementFunctor &
            operator=(const DeserializeElementFunctor &) = delete;

            template <typename T> void operator()(T const &) {
                if (elements::getTypeName<T>() == m_typename) {
                    T value;
                    PathElementFromJsonFunctor functor(m_val);
                    PathElementSerializationHandler<T>::handle(functor, value);
                    m_elt = value;
                }
            }

          private:
            Json::Value const &m_val;
            std::string const m_typename;
            elements::PathElement &m_elt;
        };
    } // namespace

    Json::Value pathTreeToJson(PathTree &tree, bool keepNulls) {
        auto visitor = PathTreeToJsonVisitor{keepNulls};
        tree.visitConstTree(visitor);
        return visitor.getResult();
    }

    void jsonToPathTree(PathTree &tree, Json::Value nodes) {
        for (auto const &node : nodes) {
            elements::PathElement elt;
            DeserializeElementFunctor functor{node, elt};
            boost::mpl::for_each<elements::PathElement::types>(functor);
            tree.getNodeByPath(node["path"].asString()).value() = elt;
        }
    }
} // namespace common
} // namespace osvr