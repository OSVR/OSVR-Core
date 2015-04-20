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
#include "DOTGraphOutput.h"
#include <osvr/Common/PathElementTools.h>

// Library/third-party includes
#include <boost/range/iterator_range_core.hpp>

// Standard includes
#include <sstream>
#include <map>
#include <set>
#include <algorithm>

DOTNode::DOTNode(std::string const &id) : m_id(id) {}

DOTGraphOutput::DOTGraphOutput(std::ostream &stream)
    : m_count(0), m_os(stream), m_treeShape(false) {
    auto &root = m_addNode("/");
    std::ostringstream os;
    os << root.getID() << "[shape=circle, label=\"root\"];";
    m_outputLineWithId(root.getID(), os.str());
}
DOTGraphOutput::~DOTGraphOutput() {
    m_os << "digraph {\n";
    m_os << "rankdir = LR\n";
    auto endOfReferencedIds = end(m_referencedIds);

    /// Output only the referenced nodes
    for (auto const &idAndOutput : m_idAndOutput) {
        if (m_referencedIds.find(idAndOutput.first) != endOfReferencedIds) {
            m_os << idAndOutput.second << "\n";
        }
    }

    if (m_treeShape) {
        /// Specify the nodes that should be on the same "rank" for the tree
        /// structure.
        typedef uint8_t Rank;
        std::set<Rank> ranks;
        std::multimap<Rank, std::string> rankAndIDs;
        for (auto const &nameNode : m_nodes) {
            if (m_referencedIds.find(nameNode.second->getID()) ==
                endOfReferencedIds) {
                /// This node wasn't referenced.
                continue;
            }
            Rank rank;
            if (nameNode.first == "/") {
                // This is the root - special
                rank = 0;
            } else {
                rank = std::count(nameNode.first.begin(), nameNode.first.end(),
                                  '/');
            }
            ranks.insert(rank);
            rankAndIDs.insert(std::make_pair(rank, nameNode.second->getID()));
        }
        for (auto const rank : ranks) {
            m_os << "{ rank=same; ";
            for (auto const &rankAndID :
                 boost::make_iterator_range(rankAndIDs.equal_range(rank))) {
                m_os << rankAndID.second << " ";
            }
            m_os << "}\n";
        }
    }
    m_os << "}\n";
}

NodeInterface &DOTGraphOutput::addNode(std::string const &label,
                                       std::string const &fullPath,
                                       std::string const &type) {
    auto it = m_nodes.find(fullPath);
    if (m_nodes.end() == it) {
        auto &node = m_addNode(fullPath);
        m_outputNode(label, node.getID(), type);
        return node;
    }
    return *(it->second);
}
NodeInterface &DOTGraphOutput::getNode(std::string const &name) {
    return m_getNode(name);
}
void DOTGraphOutput::addEdge(NodeInterface &tail, NodeInterface &head,
                             std::string const &type, std::string const &data) {

    std::string attributes;
    if (type == "alias") {
        if (!data.empty()) {
            attributes = "label=\"has transform\""; /// @todo not including
            /// transform because we'd have
            /// to escape characters.
        }
    }
    auto &dotHead = m_getNode(head);
    auto &dotTail = m_getNode(tail);
    addEdge(dotTail, dotHead, attributes);
}

void DOTGraphOutput::addEdge(DOTNode &tail, DOTNode &head,
                             std::string const &attributes) {
    m_referencedIds.insert(tail.getID());
    m_referencedIds.insert(head.getID());
    std::ostringstream os;
    os << tail.getID() << " -> " << head.getID();
    if (!attributes.empty()) {
        os << "[" << attributes << "]";
    }
    os << ";";
    m_outputLineWithId(tail.getID(), os.str());
}

void DOTGraphOutput::enableTreeOrganization() {}

DOTNode &DOTGraphOutput::m_getNode(NodeInterface const &node) {
    return m_getNode(m_nodePathsByID.at(node.getID()));
}

DOTNode &DOTGraphOutput::m_getNode(std::string const &fullPath) {
    return *m_nodes.at(fullPath);
}

void DOTGraphOutput::m_outputNode(std::string const &label,
                                  std::string const &id,
                                  std::string const &type) {
    using namespace osvr::common;
    std::ostringstream os;
    os << id << "[shape=box,label=\"" << label << "\"";
    if (type == elements::getTypeName<elements::SensorElement>()) {
        os << ",style=filled,color=green";
    }
    os << "];";
    m_outputLineWithId(id, os.str());
}
void DOTGraphOutput::m_outputLineWithId(std::string const &id,
                                        std::string const &line) {
    m_idAndOutput.push_back(std::make_pair(id, line));
}
DOTNode &DOTGraphOutput::m_addNode(std::string const &fullPath) {
    auto id = m_getNextID();
    m_nodes.emplace(
        std::make_pair(fullPath, osvr::unique_ptr<DOTNode>(new DOTNode(id))));
    m_nodePathsByID.insert(std::make_pair(id, fullPath));
    return m_getNode(fullPath);
}

std::string DOTGraphOutput::m_getNextID() {
    std::ostringstream id;
    id << "n" << m_count;
    m_count++;
    return id.str();
}