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

#ifndef INCLUDED_PathTree_h_GUID_8C6C691A_AAB1_4586_64DD_BD3F870C9071
#define INCLUDED_PathTree_h_GUID_8C6C691A_AAB1_4586_64DD_BD3F870C9071

// Internal Includes
#include <osvr/Routing/Export.h>
#include <osvr/Routing/PathElementTypes_fwd.h>
#include <osvr/Util/TreeNode_fwd.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
// - none

namespace osvr {
namespace routing {

    /// @brief A tree representation, with path/url syntax, of the known OSVR
    /// system.
    class PathTree : boost::noncopyable {
      public:
        /// @brief The specific tree node type that contains a path element.
        typedef ::osvr::util::TreeNode<PathElement> Node;

        /// @brief Constructor
        OSVR_ROUTING_EXPORT PathTree();

        /// @brief Visit the tree, starting at the root, with the given functor.
        template <typename F> void visitTree(F &functor) { functor(*m_root); }

        /// @brief Visit the tree, with const nodes, starting at the root, with
        /// the given functor.
        template <typename F> void visitConstTree(F &functor) const {
            functor(const_cast<Node const &>(*m_root));
        }

      private:
        /// @brief The ownership pointer of the specific tree node.
        typedef ::osvr::util::TreeNodePointer<PathElement>::type NodePtr;

        NodePtr m_root;
    };
} // namespace routing
} // namespace osvr

#endif // INCLUDED_PathTree_h_GUID_8C6C691A_AAB1_4586_64DD_BD3F870C9071
