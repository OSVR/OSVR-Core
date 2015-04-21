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

#ifndef INCLUDED_GraphOutputInterface_h_GUID_CA9DFEEF_13CD_41BB_B5AF_377C61EE81A6
#define INCLUDED_GraphOutputInterface_h_GUID_CA9DFEEF_13CD_41BB_B5AF_377C61EE81A6

// Internal Includes
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <iosfwd>

class NodeInterface : boost::noncopyable {
  public:
    virtual std::string const &getID() const = 0;

  protected:
    NodeInterface() = default;
};
class GraphOutputInterface : boost::noncopyable {
  public:
    /// @brief Factory method
    static osvr::unique_ptr<GraphOutputInterface>
    createGraphOutputInterface(std::ostream &os, std::string const &type);

    virtual ~GraphOutputInterface();

    virtual NodeInterface &addNode(std::string const &label,
                                   std::string const &fullPath,
                                   std::string const &type) = 0;
    virtual NodeInterface &getNode(std::string const &fullPath) = 0;
    virtual void addEdge(NodeInterface &tail, NodeInterface &head,
                         std::string const &type,
                         std::string const &data = std::string()) = 0;
    /// @brief Sets a flag indicating that we should try to organize nodes in the path tree structure.
    virtual void enableTreeOrganization() = 0;

  protected:
    GraphOutputInterface() = default;
};

#endif // INCLUDED_GraphOutputInterface_h_GUID_CA9DFEEF_13CD_41BB_B5AF_377C61EE81A6
