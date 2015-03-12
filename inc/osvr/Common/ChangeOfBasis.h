/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ChangeOfBasis_h_GUID_3821B5B4_4DC3_4C94_71A5_3D05E043CC62
#define INCLUDED_ChangeOfBasis_h_GUID_3821B5B4_4DC3_4C94_71A5_3D05E043CC62

// Internal Includes
#include <osvr/Util/StdInt.h>
#include <osvr/Common/Transform.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

namespace osvr {
namespace common {
    class ChangeOfBasis {
      public:
        ChangeOfBasis() : m_xform(Eigen::Matrix4d::Zero()) {
            m_xform(3, 3) = 1;
        }
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        void setNewX(Eigen::Vector3d const &oldVec) { m_setNew(0, oldVec); }
        void setNewY(Eigen::Vector3d const &oldVec) { m_setNew(1, oldVec); }
        void setNewZ(Eigen::Vector3d const &oldVec) { m_setNew(2, oldVec); }

        Transform get() const {
            return Transform(m_xform.transpose(), m_xform);
        }

      private:
        void m_setNew(uint8_t newAxis, Eigen::Vector3d const &oldVec) {
            m_xform.block<1, 3>(newAxis, 0) = oldVec.transpose();
        }
        Eigen::Matrix4d m_xform;
    };
} // namespace common
} // namespace osvr

#endif // INCLUDED_ChangeOfBasis_h_GUID_3821B5B4_4DC3_4C94_71A5_3D05E043CC62
