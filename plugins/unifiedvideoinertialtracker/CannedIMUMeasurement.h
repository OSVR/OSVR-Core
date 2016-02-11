/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_CannedIMUMeasurement_h_GUID_98C6C454_C977_4081_2065_A89E6A62C8FA
#define INCLUDED_CannedIMUMeasurement_h_GUID_98C6C454_C977_4081_2065_A89E6A62C8FA

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <boost/assert.hpp>

// Standard includes
#include <array>

namespace osvr {
namespace vbtracker {

    /// A safe way to store and transport an orientation measurement or an
    /// angular velocity measurement without needing special alignment
    class CannedIMUMeasurement {
      public:
        void setOrientation(Eigen::Quaterniond const &quat,
                            Eigen::Vector3d const &variance) {
            Eigen::Vector4d::Map(m_quat.data()) = quat.coeffs();
            Eigen::Vector3d::Map(m_quatVar.data()) = variance;
            m_orientationValid = true;
        }
        bool orientationValid() const { return m_orientationValid; }

        void restoreQuat(Eigen::Quaterniond &quat) const {
            BOOST_ASSERT_MSG(
                orientationValid(),
                "restoring quat on an invalid orientation measurement!");
            quat.coeffs() = Eigen::Vector4d::Map(m_quat.data());
        }

        void restoreQuatVariance(Eigen::Vector3d &var) const {
            BOOST_ASSERT_MSG(orientationValid(), "restoring quat variance on "
                                                 "an invalid orientation "
                                                 "measurement!");
            var = Eigen::Vector3d::Map(m_quatVar.data());
        }

        void setAngVel(Eigen::Vector3d const &angVel,
                       Eigen::Vector3d const &variance) {
            Eigen::Vector3d::Map(m_angVel.data()) = angVel;
            Eigen::Vector3d::Map(m_angVelVar.data()) = variance;
            m_angVelValid = true;
        }

        bool angVelValid() const { return m_angVelValid; }
        void restoreAngVel(Eigen::Vector3d &angVel) const {
            BOOST_ASSERT_MSG(angVelValid(), "restoring ang vel on "
                                            "an invalid ang vel "
                                            "measurement!");
            angVel = Eigen::Vector3d::Map(m_angVel.data());
        }
        void restoreAngVelVariance(Eigen::Vector3d &var) const {
            BOOST_ASSERT_MSG(angVelValid(), "restoring ang vel variance on "
                                            "an invalid ang vel "
                                            "measurement!");
            var = Eigen::Vector3d::Map(m_angVelVar.data());
        }

      private:
        bool m_orientationValid = false;
        std::array<double, 4> m_quat;
        std::array<double, 3> m_quatVar;
        bool m_angVelValid = false;
        std::array<double, 3> m_angVel;
        std::array<double, 3> m_angVelVar;
    };
} // namespace vbtracker
} // namespace osvr
#endif // INCLUDED_CannedIMUMeasurement_h_GUID_98C6C454_C977_4081_2065_A89E6A62C8FA
