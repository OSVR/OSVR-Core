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
#include <osvr/Common/DecomposeOriginalSource.h>
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

    OriginalSource::OriginalSource()
        : m_device(nullptr), m_interface(nullptr), m_sensor(nullptr) {}

    bool OriginalSource::isResolved() const {
        return m_device != nullptr && m_interface != nullptr;
    }

    void OriginalSource::setDevice(common::PathNode &device) {
        BOOST_ASSERT_MSG(m_device == nullptr,
                         "Device should only be set once.");
        m_device = &device;
    }

    void OriginalSource::setInterface(common::PathNode &iface) {
        BOOST_ASSERT_MSG(m_interface == nullptr,
                         "Interface should only be set once.");
        m_interface = &iface;
    }

    void OriginalSource::setSensor(common::PathNode &sensor) {
        BOOST_ASSERT_MSG(m_sensor == nullptr,
                         "Sensor should only be set once.");
        m_sensor = &sensor;
    }

    void OriginalSource::setTransform(std::string const &transform) {
        BOOST_ASSERT_MSG(m_transform.empty(),
                         "Transform should only be set once.");
        m_transform = transform;
    }

    common::PathNode *OriginalSource::getDevice() const { return m_device; }

    common::elements::DeviceElement const &
    OriginalSource::getDeviceElement() const {
        BOOST_ASSERT_MSG(isResolved(),
                         "Only makes sense when called on a resolved source.");
        return boost::get<common::elements::DeviceElement>(m_device->value());
    }

    common::PathNode *OriginalSource::getInterface() const {
        return m_interface;
    }

    std::string OriginalSource::getInterfaceName() const {
        BOOST_ASSERT_MSG(isResolved(),
                         "Only makes sense when called on a resolved source.");
        std::string ret;
        if (nullptr == m_interface) {
            return ret;
        }
        ret = m_interface->getName();
        return ret;
    }

    common::PathNode *OriginalSource::getSensor() const { return m_sensor; }

    boost::optional<int> OriginalSource::getSensorNumber() const {
        BOOST_ASSERT_MSG(isResolved(),
                         "Only makes sense when called on a resolved source.");
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

    std::string OriginalSource::getTransform() const { return m_transform; }

    namespace {
        class DecomposeOriginalSource : public boost::static_visitor<>,
                                        boost::noncopyable {
          public:
            DecomposeOriginalSource(PathNode &node, OriginalSource &source)
                : boost::static_visitor<>(), m_source(source) {
                m_applyVisitor(node);
            }

            template <typename T> void operator()(PathNode &, T &) {
                throw std::runtime_error("Element type encountered that is not "
                                         "recognized in decomposition!");
            }

            void operator()(PathNode &node, elements::SensorElement &) {
                m_source.setSensor(node);
                if (nullptr == node.getParent()) {
                    throw exceptions::invariants::SensorMissingParent(
                        getFullPath(node));
                }
                m_applyVisitor(*node.getParent());
                if (nullptr == m_source.getInterface()) {
                    // Hmm, finished traversing upward and didn't get an
                    // interface.
                    throw exceptions::invariants::SensorMissingInterfaceParent(
                        getFullPath(node));
                }
            }

            void operator()(PathNode &node, elements::InterfaceElement &) {
                m_source.setInterface(node);
                if (nullptr == node.getParent()) {
                    throw exceptions::invariants::InterfaceMissingParent(
                        getFullPath(node));
                }
                m_applyVisitor(*node.getParent());
            }

            void operator()(PathNode &node, elements::DeviceElement &) {
                m_source.setDevice(node);
                /// stop recursing through parents - the device is as far as we
                /// need.
            }

          private:
            void m_applyVisitor(PathNode &node) {
                applyPathNodeVisitor(*this, node);
            }
            OriginalSource &m_source;
        };
    } // namespace
    void OriginalSource::decompose(PathNode &node) {
        DecomposeOriginalSource decomp{node, *this};
    }
} // namespace common
} // namespace osvr
