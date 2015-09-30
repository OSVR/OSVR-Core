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

#ifndef INCLUDED_TreeTraversalVisitor_h_GUID_A75E39C9_AB5B_4494_0BF6_E8C3BA1BCB34
#define INCLUDED_TreeTraversalVisitor_h_GUID_A75E39C9_AB5B_4494_0BF6_E8C3BA1BCB34

// Internal Includes
#include <osvr/Util/TreeNode_fwd.h>

// Library/third-party includes
// - none

// Standard includes
#include <utility>

namespace osvr {
namespace util {
    namespace tree {
        /// @brief A wrapper for pre-order traversal of a TreeNode-based tree
        /// with something like a lambda.
        template <typename F> class TreeTraversalWrapper {
          public:
            explicit TreeTraversalWrapper(F &&functor)
                : m_functor(std::move(functor)) {}

            TreeTraversalWrapper(TreeTraversalWrapper const &) = delete;
            TreeTraversalWrapper &
            operator=(TreeTraversalWrapper const &) = delete;

            template <typename T> void operator()(TreeNode<T> &node) {
                m_functor(node);
                node.visitChildren(*this);
            }
            template <typename T> void operator()(TreeNode<T> const &node) {
                m_functor(node);
                node.visitConstChildren(*this);
            }

          private:
            F m_functor;
        };

        /// @brief A method to handle visiting every node in a tree with a
        /// lambda or other by-value function object
        template <typename T, typename F>
        inline void traverseWith(T &node, F &&functor) {
            TreeTraversalWrapper<F> funcWrap{std::forward<F>(functor)};
            funcWrap(node);
        }
    } // namespace tree
    using tree::traverseWith;
} // namespace util
} // namespace osvr
#endif // INCLUDED_TreeTraversalVisitor_h_GUID_A75E39C9_AB5B_4494_0BF6_E8C3BA1BCB34
