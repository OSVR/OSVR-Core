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
#include <osvr/Routing/PathNode_fwd.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace routing {

    /// @brief A tree representation, with path/url syntax, of the known OSVR
    /// system.
    class PathTree : boost::noncopyable {
      public:
        /// @brief Constructor
        OSVR_ROUTING_EXPORT PathTree();

        /// @brief Visitation
        /// @{
        /// @brief Visit the tree, starting at the root, with the given functor.
        template <typename F> void visitTree(F &functor) { functor(*m_root); }

        /// @brief Visit the tree, with const nodes, starting at the root, with
        /// the given functor.
        template <typename F> void visitConstTree(F &functor) const {
            functor(const_cast<PathNode const &>(*m_root));
        }
        /// @}

        /// @brief Returns the node indicated by the path, which must be
        /// absolute (begin with a /). Any non-existent nodes will be created
        /// with values of NullElement
        OSVR_ROUTING_EXPORT PathNode &getNodeByPath(std::string const &path);

        /// @brief Get the root of the tree.
        OSVR_ROUTING_EXPORT PathNode &getRoot();

        /// @brief Get the root of the tree (const).
        OSVR_ROUTING_EXPORT PathNode const &getRoot() const;

        /// @brief Adds/updates nodes for the basic path to a device.
        /// @param deviceName A namespaced device name coming from a plugin,
        /// like "/org_opengoggles_plugin/SampleDevice". If a leading slash is
        /// missing, it will be assumed and added.
        /// @returns The device node
        /// @throws std::runtime_error if an invalid device name (less than two
        /// components) was passed.
        OSVR_ROUTING_EXPORT PathNode &addDevice(std::string const &deviceName);

      private:
        /// @brief Root node of the tree.
        PathNodePtr m_root;
    };
} // namespace routing
} // namespace osvr

#endif // INCLUDED_PathTree_h_GUID_8C6C691A_AAB1_4586_64DD_BD3F870C9071
