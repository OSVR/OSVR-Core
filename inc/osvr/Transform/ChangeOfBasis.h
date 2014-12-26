/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_ChangeOfBasis_h_GUID_3821B5B4_4DC3_4C94_71A5_3D05E043CC62
#define INCLUDED_ChangeOfBasis_h_GUID_3821B5B4_4DC3_4C94_71A5_3D05E043CC62

// Internal Includes
#include <osvr/Util/StdInt.h>
#include <osvr/Transform/Transform.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>

// Standard includes
// - none

namespace osvr {
namespace transform {
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
} // namespace transform
} // namespace osvr

#endif // INCLUDED_ChangeOfBasis_h_GUID_3821B5B4_4DC3_4C94_71A5_3D05E043CC62
