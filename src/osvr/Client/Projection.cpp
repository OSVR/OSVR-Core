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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "Projection.h"
#include "DisplayConfiguration.h"
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace client {

    namespace {
        class Rect {
          public:
            EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

            typedef Rect type;
            typedef double value_type;
            typedef Eigen::Vector4d storage_type;
            enum Side { LEFT = 0, TOP = 1, RIGHT = 2, BOTTOM = 3 };

            /// @brief Access by side
            value_type &operator()(Side s) {
                return m_data[static_cast<storage_type::Index>(s)];
            }
            value_type operator()(Side s) const {
                return m_data[static_cast<storage_type::Index>(s)];
            }

            /// @brief Chained function call operator for setting sides.
            type &operator()(Side s, value_type v) {
                (*this)(s) = v;
                return *this;
            }

            storage_type &data() { return m_data; }
            storage_type const &data() const { return m_data; }

          private:
            storage_type m_data;
        };
    } // namespace
    static inline class ComputationalDisplayModel::Impl : boost::noncopyable {
        DisplayConfiguration displayConfig;
    };

    DisplayModelList
    generateComputationalDisplayModel(std::string const &displayDescriptor) {}
    ComputationalDisplayModel::~ComputationalDisplayModel() {}

} // namespace client
} // namespace osvr