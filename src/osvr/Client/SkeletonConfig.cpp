/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include <osvr/Client/SkeletonConfig.h>
#include <osvr/Common/ApplyPathNodeVisitor.h>
#include <osvr/Common/PathElementTools.h>
#include <osvr/Common/PathElementTypes.h>
#include <osvr/Common/PathNode.h>
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/ProcessArticulationSpec.h>
#include <osvr/Util/SharedPtr.h>
#include <osvr/Util/TreeNode.h>
#include <osvr/Util/TreeTraversalVisitor.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/variant/get.hpp>

// Standard includes
#include <string>
#include <utility>

namespace osvr {
namespace client {

    class ArticulationTreeTraverser : public boost::static_visitor<> {
      public:
        /// @brief Constructor
        ArticulationTreeTraverser(osvr::common::RegisteredStringMap *jointMap,
                                  osvr::common::RegisteredStringMap *boneMap,
                                  InterfaceMap *jointInterfaces,
                                  OSVR_ClientContext &ctx)
            : boost::static_visitor<>(), m_jointMap(jointMap),
              m_boneMap(boneMap), m_jointInterfaces(jointInterfaces),
              m_ctx(ctx) {}

        /// @brief ignore null element
        void operator()(osvr::common::PathNode const &,
                        osvr::common::elements::NullElement const &) {}

        void
        operator()(osvr::common::PathNode const &node,
                   osvr::common::elements::ArticulationElement const &elt) {

            /// register joints
            if (elt.getArticulationType() == "joint") {

                /// register joint ID
                util::StringID jointID =
                    m_jointMap->registerStringID(node.getName());
                /// get an interface for tracker path

                /// store the id, interface association
                m_jointInterfaces->push_back(std::make_pair(
                    jointID, InternalInterfaceOwner(
                                 m_ctx, elt.getTrackerPath().c_str())));

                /// Specifying bone name for each joint is optional
                if (!elt.getBoneName().empty()) {
                    /// register boneId
                    m_boneMap->registerStringID(elt.getBoneName());
                }
            }
        }

        /// @brief Catch-all for other element types.
        template <typename T>
        void operator()(osvr::common::PathNode const &node, T const &elt) {}

      private:
        osvr::common::RegisteredStringMap *m_jointMap;
        osvr::common::RegisteredStringMap *m_boneMap;
        InterfaceMap *m_jointInterfaces;
        OSVR_ClientContext m_ctx;
    };

    class NodeToTrackerPathVisitor : public boost::static_visitor<std::string> {
      public:
        /// @brief Constructor
        NodeToTrackerPathVisitor() : boost::static_visitor<std::string>() {}

        std::string
        operator()(osvr::common::PathNode const &node,
                   osvr::common::elements::ArticulationElement const &elt) {
            return elt.getTrackerPath();
        }

        /// @brief Catch-all for other element types.
        template <typename T>
        std::string operator()(osvr::common::PathNode const &node,
                               T const &elt) {
            return std::string();
        }
    };

    SkeletonConfigPtr SkeletonConfigFactory::create(
        OSVR_ClientContext ctx,
        osvr::common::PathTree const &articulationTree) {

        if (ctx == nullptr) {
            OSVR_DEV_VERBOSE(
                "Couldn't create a skeleton config! Client context is null");
            throw std::runtime_error(
                "Couldn't create a skeleton config! Client context is null");
        }

        if (!ctx->getStatus()) {
            OSVR_DEV_VERBOSE("Couldn't create a skeleton config! "
                             "Client context is not yet initialized!");
            throw NoCtxYet();
        }
        /// Client context is ready
        SkeletonConfigPtr cfg(new SkeletonConfig(ctx));
        /// get the path tree
        osvr::common::clonePathTree(articulationTree, cfg->m_articulationTree);

        ArticulationTreeTraverser traverser(&cfg->m_jointMap, &cfg->m_boneMap,
                                            &cfg->m_jointInterfaces,
                                            cfg->m_ctx);
        osvr::util::traverseWith(
            articulationTree.getRoot(),
            [&traverser](osvr::common::PathNode const &node) {
                osvr::common::applyPathNodeVisitor(traverser, node);
            });

        return cfg;
    }

    SkeletonConfig::SkeletonConfig(OSVR_ClientContext ctx) : m_ctx(ctx) {}

    void SkeletonConfig::updateArticulationTree(
        osvr::common::PathTree const &articulationTree) {

        /// release previous interfaces
        m_jointInterfaces.clear();
        m_boneInterfaces.clear();

        /// get the path tree
        osvr::common::clonePathTree(articulationTree, m_articulationTree);
        ArticulationTreeTraverser traverser(&m_jointMap, &m_boneMap,
                                            &m_jointInterfaces, m_ctx);
        osvr::util::traverseWith(
            m_articulationTree.getRoot(),
            [&traverser](osvr::common::PathNode const &node) {
                osvr::common::applyPathNodeVisitor(traverser, node);
            });
    }

} // namespace client
} // namespace osvr
