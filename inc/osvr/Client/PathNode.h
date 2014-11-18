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

#ifndef INCLUDED_PathNode_h_GUID_EEC6C5AF_332A_4780_55C2_D794B3BF5106
#define INCLUDED_PathNode_h_GUID_EEC6C5AF_332A_4780_55C2_D794B3BF5106

// Internal Includes
#include <osvr/Client/PathNodePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <vector>

namespace osvr {
namespace client {
    class PathNode : public enable_shared_from_this<PathNode>,
                     boost::noncopyable {
      public:
        /// @brief Create a path node
        static PathNode &create(PathNode &parent, std::string const &name);

        PathNode &getOrCreateChildByName(std::string const &name);

        std::string const &getName() const;

        bool isRoot() const;

      private:
        /// @brief Private constructor for a non-root node
        PathNode(PathNode &parent, std::string const &name);

        /// @brief Private constructor for a root node
        PathNode();

        /// @brief Internal helper to get child by name, or a null pointer if no
        /// such child.
        PathNodePtr m_getChildByName(std::string const &name);
        /// @brief Internal helper to add a named child. Assumes no such child
        /// already exists!
        void m_addChild(PathNodePtr const &child);
        /// @brief Name
        std::string const m_name;
        /// @brief Weak pointer to parent.
        PathNodeWeakPtr m_parent;

        typedef std::vector<PathNodePtr> ChildList;
        /// @brief Ownership of children
        ChildList m_children;

        friend class PathTree;
    };
} // namespace client
} // namespace osvr

#endif // INCLUDED_PathNode_h_GUID_EEC6C5AF_332A_4780_55C2_D794B3BF5106
