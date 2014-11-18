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
#include <osvr/Client/PathNode.h>

// Library/third-party includes
// - none

// Standard includes
#include <algorithm>
#include <stdexcept>
#include <boost/assert.hpp>

namespace osvr {
namespace client {

    PathNode &PathNode::getOrCreateChildByName(std::string const &name) {
        PathNodePtr child = m_getChildByName(name);
        if (child) {
            return *child;
        }
        return PathNode::create(*this, name);
    }

    std::string const &PathNode::getName() const { return m_name; }

    bool PathNode::isRoot() const {
        BOOST_ASSERT_MSG(m_parent.expired() == m_name.empty(),
                         "The root and only the root should have an empty name "
                         "and no parent!");
        return m_name.empty();
    }

    PathNodePtr PathNode::m_getChildByName(std::string const &name) {
        /// @todo Don't use a linear search here - use an unordered map or
        /// something.
        ChildList::const_iterator it = std::find_if(
            begin(m_children), end(m_children),
            [&](PathNodePtr const &n) { return n->getName() == name; });
        PathNodePtr ret;
        if (it != end(m_children)) {
            ret = *it;
        }
        return ret;
    }

    void PathNode::m_addChild(PathNodePtr const &child) {
        m_children.push_back(child);
    }

    PathNode &PathNode::create(PathNode &parent, std::string const &name) {
        if (parent.m_getChildByName(name)) {
            throw std::logic_error("Can't create a child with the same name as "
                                   "an existing child!");
        }
        PathNodePtr ret(new PathNode(parent, name));
        parent.m_addChild(ret);
        return *ret;
    }

    PathNode::PathNode(PathNode &parent, std::string const &name)
        : m_name(name), m_parent(parent.shared_from_this()), m_children() {
        if (m_parent.expired()) {
            throw std::logic_error(
                "Can't create a named path node with no parent!");
        }
        if (m_name.empty()) {
            throw std::logic_error(
                "Can't create a named path node with an empty name!");
        }
    }

    PathNode::PathNode() : m_name(), m_parent(), m_children() {
        /// Special root constructor
    }
} // namespace client
} // namespace osvr