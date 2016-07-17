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
#include "PathElementSerialization.h"
#include <osvr/Common/ApplyPathNodeVisitor.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathTreeSerialization.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <json/value.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace {
        /// @brief A PathNodeVisitor that returns a JSON object corresponding to
        /// a single PathNode (including its contained PathElement value)
        class PathNodeToJsonVisitor
            : public boost::static_visitor<Json::Value> {
          public:
            PathNodeToJsonVisitor() : boost::static_visitor<Json::Value>() {}

            /// @brief Adds data from the PathNode to an object containing
            /// PathElement data.
            void addNodeData(PathNode const &node, Json::Value &val) {
                val["path"] = getFullPath(node);
                val["type"] = getTypeName(node);
            }

            template <typename T>
            Json::Value operator()(PathNode const &node, T const &elt) {
                auto ret = pathElementToJson(elt);
                addNodeData(node, ret);
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
    } // namespace

    Json::Value pathTreeToJson(PathTree const &tree, bool keepNulls) {
        auto visitor = PathTreeToJsonVisitor{keepNulls};
        tree.visitConstTree(visitor);
        return visitor.getResult();
    }

    void jsonToPathTree(PathTree &tree, Json::Value nodes) {
        for (auto const &node : nodes) {
            elements::PathElement elt = jsonToPathElement(node);
            tree.getNodeByPath(node["path"].asString()).value() = elt;
        }
    }
} // namespace common
} // namespace osvr