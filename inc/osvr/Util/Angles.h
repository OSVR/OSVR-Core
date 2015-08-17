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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_Angles_h_GUID_DDFCCAF8_9ED0_4E85_62F3_173C073B8BB7
#define INCLUDED_Angles_h_GUID_DDFCCAF8_9ED0_4E85_62F3_173C073B8BB7

// Internal Includes
// - none

// Library/third-party includes
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/plane_angle.hpp>
#include <boost/units/systems/angle/degrees.hpp>

// Standard includes
// - none
namespace osvr {
namespace util {
    using boost::units::si::radians;
    using boost::units::degree::degrees;

    /// @brief Convenience template alias for a plane_angle quantity in an
    /// arbitrary system with arbitrary scalar
    template <typename System, typename Y = double>
    using AngleGeneric = boost::units::quantity<
        boost::units::unit<boost::units::plane_angle_dimension, System>, Y>;

    /// @brief Alias for an angle in radians with arbitrary scalar type
    template <typename Y>
    using AngleRadians =
        boost::units::quantity<boost::units::si::plane_angle, Y>;

    /// @brief Alias for an angle in degrees with arbitrary scalar type
    template <typename Y>
    using AngleDegrees =
        boost::units::quantity<boost::units::degree::plane_angle, Y>;

    typedef AngleRadians<double> AngleRadiansd;
    typedef AngleDegrees<double> AngleDegreesd;

    /// @brief Default angle type
    typedef AngleRadiansd Angle;

    /// @brief Get the raw scalar value of your angle in radians
    template <typename System, typename Y>
    inline Y getRadians(AngleGeneric<System, Y> const angle) {
        const AngleRadians<Y> ret(angle);
        return ret.value();
    }

    /// @brief Get the raw scalar value of your angle in degrees
    template <typename System, typename Y>
    inline Y getDegrees(AngleGeneric<System, Y> const angle) {
        const AngleDegrees<Y> ret(angle);
        return ret.value();
    }

} // namespace util
} // namespace osvr
#endif // INCLUDED_Angles_h_GUID_DDFCCAF8_9ED0_4E85_62F3_173C073B8BB7
