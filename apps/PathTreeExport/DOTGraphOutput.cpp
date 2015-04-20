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

// Library/third-party includes
// - none

// Standard includes
// - none

void DOTNode::addOutEdge(NodeInterface &sink, std::string const &type,
    std::string const &data) {

}
DOTGraphOutput::DOTGraphOutput(std::ostream &os) : m_os(os) {
    m_os << "digraph {\n";
}
DOTGraphOutput::~DOTGraphOutput() {
    m_os << "}\n";
}
NodeInterface &DOTGraphOutput::addNode(std::string const &name,
                                       std::string const &type,
                                       std::string const &parent) {
    auto it = m_nodes.find(name);
    if (m_nodes.end() == it) {
        m_nodes.emplace(
            std::make_pair(name, osvr::unique_ptr<DOTNode>(new DOTNode)));
        return getNode(name);
    }
    return *(it->second);
}
NodeInterface &DOTGraphOutput::getNode(std::string const &name) {
    return *m_nodes.at(name);
}