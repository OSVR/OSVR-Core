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
            value_type &operator[](Side s) {
                return m_data[static_cast<storage_type::Index>(s)];
            }
            value_type operator[](Side s) const {
                return m_data[static_cast<storage_type::Index>(s)];
            }

            /// @brief Chained function call operator for setting sides.
            type &operator()(Side s, value_type v) {
                (*this)[s] = v;
                return *this;
            }

            storage_type &data() { return m_data; }
            storage_type const &data() const { return m_data; }

          private:
            storage_type m_data;
        };
        /// @brief Takes in extents at the near clipping plane, as well as
        /// the near and far clipping planes. Result matrix maps [-l, r] and
        /// [-b, t] to [-1, 1], and [n, f] to [-1, 1] (should be configurable)
        inline Eigen::Matrix4d createProjectionMatrix(Rect const &bounds,
                                                      double near, double far) {
            // Convert from "left, right, bottom top, near, far" to the 4x4
            // transform.
            // See https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
            // NOTE: There is actually a bug in the documentation.  If you
            // call glFrustum() and print out the results and compare them,
            // the value D from that page holds -1 and the value where there
            // is a -1 is what holds D.  This error is also copied to the
            // Microsoft page describing this function.  These are elements
            // [2][3] and [3][2], which are swapped.
            auto right = bounds[Rect::RIGHT];
            auto left = bounds[Rect::LEFT];
            auto top = bounds[Rect::TOP];
            auto bottom = bounds[Rect::BOTTOM];

            Eigen::Matrix4d mat;
            // clang-format off
            mat << (2 * near / (right - left)), 0, ((right + left) / (right - left)), 0,
                   0, (2 * near / (top - bottom)), ((top + bottom) / (top - bottom)), 0,
                   0, 0,                           (-(far + near)/(far - near)), -1,
                   0, 0,                           (-2 * far * near/(far - near)), 0;
            // clang-format on
        }
    } // namespace
    class ComputationalDisplayModel::Impl : boost::noncopyable {
        DisplayConfiguration displayConfig;
    };

    DisplayModelList
    generateComputationalDisplayModel(std::string const &displayDescriptor) {
        DisplayModelList ret;
        return ret;
    }
    ComputationalDisplayModel::~ComputationalDisplayModel() {}

} // namespace client
} // namespace osvr
