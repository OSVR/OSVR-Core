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

#ifndef INCLUDED_DecomposeOriginalSource_h_GUID_B7D02DC3_ACFA_4BAF_85C5_8B70854700A7
#define INCLUDED_DecomposeOriginalSource_h_GUID_B7D02DC3_ACFA_4BAF_85C5_8B70854700A7

// Internal Includes
#include <osvr/Common/PathNode.h>
#include <osvr/Common/ApplyPathNodeVisitor.h>
#include <osvr/Common/RoutingExceptions.h>

// Library/third-party includes
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/get.hpp>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace common {

    class DecomposeOriginalSource : public boost::static_visitor<> {
      public:
        DecomposeOriginalSource(PathNode &node)
            : m_device(nullptr), m_interface(nullptr), m_sensor(nullptr) {
            m_applyVisitor(node);
        }

        bool gotDeviceAndInterface() const {
            return nullptr != m_device && nullptr != m_interface;
        }

        template <typename T> void operator()(PathNode &, T &) {
            throw std::runtime_error("Element type encountered that is not "
                                     "recognized in decomposition!");
        }

        void operator()(PathNode &node, elements::SensorElement &) {
            m_sensor = &node;
            if (nullptr == node.getParent()) {
                throw exceptions::invariants::SensorMissingParent(
                    getFullPath(node));
            }
            m_applyVisitor(*node.getParent());
            if (nullptr == m_interface) {
                // Hmm, finished traversing upward and didn't get an interface.
                throw exceptions::invariants::SensorMissingInterfaceParent(
                    getFullPath(node));
            }
        }

        void operator()(PathNode &node, elements::InterfaceElement &) {
            m_interface = &node;
            if (nullptr == node.getParent()) {
                throw exceptions::invariants::InterfaceMissingParent(
                    getFullPath(node));
            }
            m_applyVisitor(*node.getParent());
        }

        void operator()(PathNode &node, elements::DeviceElement &) {
            m_device = &node;
            /// stop recursing through parents - the device is as far as we
            /// need.
        }

        PathNode *getDevice() const { return m_device; }

        elements::DeviceElement const &getDeviceElement() const {
            return boost::get<elements::DeviceElement>(m_device->value());
        }
        PathNode *getInterface() const { return m_interface; }

        std::string getInterfaceName() const {
            std::string ret;
            if (nullptr == m_interface) {
                return ret;
            }
            ret = m_interface->getName();
            return ret;
        }

        PathNode *getSensor() const { return m_sensor; }

        boost::optional<int> getSensorNumber() const {
            boost::optional<int> ret;
            if (nullptr == m_sensor) {
                return ret;
            }
            try {
                ret = boost::lexical_cast<int>(m_sensor->getName());
            } catch (boost::bad_lexical_cast &) {
                // Just means we can't convert to int, so returning an empty
                // optional is fine.
            }
            return ret;
        }

      private:
        void m_applyVisitor(PathNode &node) {
            applyPathNodeVisitor(*this, node);
        }
        PathNode *m_device;
        PathNode *m_interface;
        PathNode *m_sensor;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_DecomposeOriginalSource_h_GUID_B7D02DC3_ACFA_4BAF_85C5_8B70854700A7
