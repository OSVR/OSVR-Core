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

#ifndef INCLUDED_GraphFactoryRegistration_h_GUID_12D5835A_12E1_4A94_758D_853F0A98C2A5
#define INCLUDED_GraphFactoryRegistration_h_GUID_12D5835A_12E1_4A94_758D_853F0A98C2A5

// Internal Includes
#include <osvr/Util/UniquePtr.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <string>
#include <iosfwd>
#include <functional>
#include <unordered_map>

class GraphOutputFactory : boost::noncopyable {
  public:
    GraphOutputFactory() = default;

    /// @brief Factory method
    osvr::unique_ptr<GraphOutputInterface> create(std::ostream &os,
                                                  std::string const &type) {
        return m_factories.at(type)(os);
    }
    typedef std::function<osvr::unique_ptr<GraphOutputInterface>(
        std::ostream &os)> GraphFactory;
    void registerGraphFactory(std::string const &type, GraphFactory factory) {
        m_factories[type] = factory;
    }

  private:
    std::unordered_map<std::string, GraphFactory> m_factories;
};

template <typename T> class GraphFactoryRegistration {
  public:
    GraphFactoryRegistration(GraphOutputFactory &genericFactory,
                             std::string const &type) {
        genericFactory.registerGraphFactory(type, [](std::ostream &os) {
            return osvr::unique_ptr<GraphOutputInterface>(new T(os));
        });
    }
};

#endif // INCLUDED_GraphFactoryRegistration_h_GUID_12D5835A_12E1_4A94_758D_853F0A98C2A5
