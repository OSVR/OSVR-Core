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

class DOTNode : public NodeInterface {
  public:
    DOTNode() = default;
    virtual void addOutEdge(NodeInterface &sink, std::string const &type,
                            std::string const &data = std::string());

  protected:
};
class DOTGraphOutput : public GraphOutputInterface {
  public:
    DOTGraphOutput(std::ostream &os);
    virtual ~DOTGraphOutput();

    virtual NodeInterface &addNode(std::string const &name,
                                   std::string const &type,
                                   std::string const &parent = std::string());
    virtual NodeInterface &getNode(std::string const &name);

  protected:
    std::ostream &m_os;
    std::unordered_map<std::string, osvr::unique_ptr<DOTNode> > m_nodes;
    friend class DOTNode;
};

#endif // INCLUDED_DOTGraphOutput_h_GUID_3A3C0484_26CE_4E9F_BE40_93CE7454D34B
