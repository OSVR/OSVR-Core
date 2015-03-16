/** @file
    @brief Header

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

#ifndef INCLUDED_ApplyPathNodeVisitor_h_GUID_5F05BD33_812E_4A98_354F_1481FC8AB5BE
#define INCLUDED_ApplyPathNodeVisitor_h_GUID_5F05BD33_812E_4A98_354F_1481FC8AB5BE

// Internal Includes
#include <osvr/Common/Export.h>
#include <osvr/Common/PathElementTypes_fwd.h>
#include <osvr/Util/TreeNode_fwd.h>
#include <osvr/Common/PathNode_fwd.h>

// Library/third-party includes
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace detail {
        template <typename Visitor, typename NodeType,
                  typename ResultType = typename Visitor::result_type>
        class PathNodeVisitorImpl : public boost::static_visitor<ResultType>,
                                    boost::noncopyable {
          public:
            PathNodeVisitorImpl(NodeType &node, Visitor &v)
                : m_node(node), m_v(v) {}

            template <typename T> ResultType operator()(T &val) {
                return m_v(m_node, val);
            }

            template <typename T> ResultType operator()(T const &val) {
                return m_v(m_node, val);
            }

          private:
            NodeType &m_node;
            Visitor &m_v;
        };

    } // namespace detail

    template <typename Visitor>
    inline typename Visitor::result_type applyPathNodeVisitor(Visitor &v,
                                                              PathNode &node) {
        auto visitor = detail::PathNodeVisitorImpl<Visitor, PathNode>(node, v);
        return boost::apply_visitor(visitor, node.value());
    }

    // const version
    template <typename Visitor>
    inline typename Visitor::result_type
    applyPathNodeVisitor(Visitor &v, PathNode const &node) {
        auto visitor =
            detail::PathNodeVisitorImpl<Visitor, PathNode const>(node, v);
        return boost::apply_visitor(visitor, node.value());
    }
    /// @}
} // namespace common
} // namespace osvr

#endif // INCLUDED_ApplyPathNodeVisitor_h_GUID_5F05BD33_812E_4A98_354F_1481FC8AB5BE
