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

#ifndef INCLUDED_DOTGraphOutput_h_GUID_3A3C0484_26CE_4E9F_BE40_93CE7454D34B
#define INCLUDED_DOTGraphOutput_h_GUID_3A3C0484_26CE_4E9F_BE40_93CE7454D34B

// Internal Includes
#include "GraphOutputInterface.h"

// Library/third-party includes
// - none

// Standard includes
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdint.h>

class DOTGraphOutput;
class DOTNode : public NodeInterface {
  public:
    DOTNode(std::string const &id);

    virtual std::string const &getID() const { return m_id; }

  private:
    std::string const m_id;
};
class DOTGraphOutput : public GraphOutputInterface {
  public:
    DOTGraphOutput(std::ostream &stream);
    virtual ~DOTGraphOutput();

    virtual NodeInterface &addNode(std::string const &label,
                                   std::string const &fullPath,
                                   std::string const &type);
    virtual NodeInterface &getNode(std::string const &fullPath);
    virtual void addEdge(NodeInterface &tail, NodeInterface &head,
                         std::string const &type,
                         std::string const &data = std::string());
    void addEdge(DOTNode &tail, DOTNode &head,
                 std::string const &attributes = std::string());
    virtual void enableTreeOrganization();
  private:
    void m_outputNode(std::string const &label, std::string const &id,
                      std::string const &type);
    void m_outputLineWithId(std::string const &id, std::string const &line);
    DOTNode &m_addNode(std::string const &fullPath);
    DOTNode &m_getNode(std::string const &fullPath);
    DOTNode &m_getNode(NodeInterface const &node);
    std::string m_getNextID();
    uint32_t m_count;
    std::unordered_set<std::string> m_referencedIds;
    std::vector<std::pair<std::string, std::string> > m_idAndOutput;
    std::ostream &m_os;
    /// @brief map from full path to nodes
    std::unordered_map<std::string, osvr::unique_ptr<DOTNode> > m_nodes;
    /// @brief map from node ID to full path
    std::unordered_map<std::string, std::string> m_nodePathsByID;
    bool m_treeShape;
};

#endif // INCLUDED_DOTGraphOutput_h_GUID_3A3C0484_26CE_4E9F_BE40_93CE7454D34B
