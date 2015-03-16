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
#include <osvr/Common/PathNode.h>
#include <osvr/Common/ApplyPathNodeVisitor.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <json/value.h>
#include <boost/variant.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace {

        class PathElementToJson : public boost::static_visitor<Json::Value> {
          public:
            Json::Value setup(PathNode const &node) {
                Json::Value val{Json::objectValue};
                val["path"] = getFullPath(node);
                val["type"] = getTypeName(node);
                return val;
            }

            template <typename T>
            Json::Value operator()(PathNode const &node, T const &) {
                return setup(node);
            }

            Json::Value operator()(PathNode const &node,
                                   elements::DeviceElement const &elt) {
                auto ret = setup(node);
                ret["device_name"] = elt.getDeviceName();
                ret["server"] = elt.getServer();
                return ret;
            }
            Json::Value operator()(PathNode const &node,
                                   elements::AliasElement const &elt) {
                auto ret = setup(node);
                ret["source"] = elt.getSource();
                return ret;
            }
        };

        class PathTreeToJsonVisitor {
          public:
            PathTreeToJsonVisitor(bool keepNulls)
                : m_ret(Json::arrayValue), m_keepNulls(keepNulls) {}

            Json::Value convert(PathTree &tree) {
                tree.visitConstTree(*this);
                return m_ret;
            }

            void operator()(PathNode const &node) {
                OSVR_DEV_VERBOSE("Visiting " << getFullPath(node));
                if (m_keepNulls ||
                    nullptr ==
                        boost::get<elements::NullElement>(&node.value())) {
                    // If we're keeping nulls or this isn't a null...
                    PathElementToJson visitor;
                    auto newEntry = applyPathNodeVisitor(visitor, node);
                    m_ret.append(newEntry);
                }
                node.visitConstChildren(*this);
            }

          private:
            Json::Value m_ret;
            bool m_keepNulls;
        };
    } // namespace
    Json::Value pathTreeToJson(PathTree &tree, bool keepNulls) {
        auto visitor = PathTreeToJsonVisitor{keepNulls};
        return visitor.convert(tree);
    }
} // namespace common
} // namespace osvr