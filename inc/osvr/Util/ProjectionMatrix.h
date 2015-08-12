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

#ifndef INCLUDED_ProjectionMatrix_h_GUID_638B5832_DF1C_4BB7_9844_BF9810926107
#define INCLUDED_ProjectionMatrix_h_GUID_638B5832_DF1C_4BB7_9844_BF9810926107

// Internal Includes
#include <osvr/Util/Rect.h>
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace util {
    /// @brief Takes in points at the near clipping plane, as well as
    /// the near and far clipping planes. Result matrix maps [l, r] and
    /// [b, t] to [-1, 1], and [n, f] to [-1, 1] (should be configurable)
    ///
    /// Resulting matrix operates on Eigen::Vector4d, which are row-matrices -
    /// transpose if you want it to work on column matrices.
    ///
    /// @todo Look into using Eigen::Projective3d?
    inline Eigen::Matrix4d createProjectionMatrix(Rectd const &bounds,
                                                  double near, double far) {
        // Convert from "left, right, bottom top, near, far" to the 4x4
        // transform.
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
        // BEGIN APPARENTLY OUTDATED NOTE - works more accurately if you ignore
        // that.
        // NOTE: There is actually a bug in the documentation.  If you
        // call glFrustum() and print out the results and compare them,
        // the value D from that page holds -1 and the value where there
        // is a -1 is what holds D.  This error is also copied to the
        // Microsoft page describing this function.  These are elements
        // [2][3] and [3][2], which are swapped.
        // END APPARENTLY OUTDATED NOTE
        auto const right = bounds[Rectd::RIGHT];
        auto const left = bounds[Rectd::LEFT];
        auto const top = bounds[Rectd::TOP];
        auto const bottom = bounds[Rectd::BOTTOM];

        Eigen::Matrix4d mat;
        // clang-format off
        mat << (2 * near / (right - left)), 0, ((right + left) / (right - left)), 0,
               0, (2 * near / (top - bottom)), ((top + bottom) / (top - bottom)), 0,
               0, 0, (-(far + near) / (far - near)), (-2 * far * near / (far - near)),
               0, 0,                           -1,  0;
        // clang-format on
        return mat;
    }

    namespace projection_options {

        /// @brief The bit masks for testing conditions - do not depend on these
        /// being equal to a given enum value of OptBits!
        enum OptMasks {
            ZOutputUnsignedBit = 0x1,
            LeftHandedInputBit = 0x2,
            ColumnVectorBit = 0x4
        };

        /// @brief The flags (combine with bitwise-or `|`) for specifying
        /// configuration - do not depend on these being equal to a given enum
        /// value of OptMasks!
        enum OptBits {
            ZOutputSigned = 0,
            ZOutputUnsigned = ZOutputUnsignedBit,

            RightHandedInput = 0,
            LeftHandedInput = LeftHandedInputBit,

            RowVector = 0,
            ColumnVector = ColumnVectorBit
        };

        typedef unsigned char OptionType;
        template <OptionType Options, OptionType Bit>
        using CheckOptionBit =
            std::integral_constant<bool, static_cast<bool>(Options &Bit)>;
        template <OptionType Options, OptionType Bit>
        using ClearOptionBit =
            std::integral_constant<OptionType, Options &(~Bit)>;

        template <OptionType Options>
        using IsZOutputUnsigned = CheckOptionBit<Options, ZOutputUnsignedBit>;
        template <OptionType Options>
        using IsLeftHandedInput = CheckOptionBit<Options, LeftHandedInputBit>;
        template <OptionType Options>
        using IsColumnVector = CheckOptionBit<Options, ColumnVectorBit>;

    } // namespace projection_options

    namespace projection_detail {
        // 33 and 34 for signed Z output (range of [-1, 1])
        template <projection_options::OptionType options, bool = projection_options::IsZOutputUnsigned<options>::value>
        struct ThirdRow {
            static inline double get3(double near, double far) {
                return (-(far + near) / (far - near));
            }
            static inline double get4(double near, double far) {
                return (-2 * far * near / (far - near));
            }
        };
        // 33 and 34 for unsigned Z output (range of [0, 1])
        template <projection_options::OptionType options>
        struct ThirdRow<
            options, true> {
            static inline double get3(double near, double far) {
                return (-far / (far - near));
            }
            static inline double get4(double near, double far) {
                return (-far * near / (far - near));
            }
        };
        template <projection_options::OptionType options>
        inline double get33(double near, double far) {
            return ThirdRow<options>::get3(near, far);
        };
        template <projection_options::OptionType options>
        inline double get34(double near, double far) {
            return ThirdRow<options>::get4(near, far);
        }
    } // namespace projection_detail
    /// @brief Takes in points at the near clipping plane, as well as
    /// the near and far clipping planes. Result matrix maps [l, r] and
    /// [b, t] to [-1, 1], and [n, f] to [-1, 1] (should be configurable)
    template <projection_options::OptionType options =
                  projection_options::ZOutputSigned |
                  projection_options::RightHandedInput |
                  projection_options::RowVector>
    inline Eigen::Matrix4d
    parameterizedCreateProjectionMatrix(Rectd const &bounds, double near,
                                        double far) {
        // Convert from "left, right, bottom top, near, far" to the 4x4
        // transform.
        // See https://www.opengl.org/sdk/docs/man2/xhtml/glFrustum.xml
        // BEGIN APPARENTLY OUTDATED NOTE - works accurately if you ignore it
        // NOTE: There is actually a bug in the documentation.  If you
        // call glFrustum() and print out the results and compare them,
        // the value D from that page holds -1 and the value where there
        // is a -1 is what holds D.  This error is also copied to the
        // Microsoft page describing this function.  These are elements
        // [2][3] and [3][2], which are swapped.
        // END APPARENTLY OUTDATED NOTE
        auto right = bounds[Rectd::RIGHT];
        auto left = bounds[Rectd::LEFT];
        auto top = bounds[Rectd::TOP];
        auto bottom = bounds[Rectd::BOTTOM];

        using projection_detail::get33;
        using projection_detail::get34;
        Eigen::Matrix4d mat;
        // clang-format off
        mat << (2 * near / (right - left)), 0, ((right + left) / (right - left)), 0,
            0, (2 * near / (top - bottom)), ((top + bottom) / (top - bottom)), 0,
            0, 0, get33<options>(near, far), get34<options>(near, far),
            0, 0, -1, 0;
        // clang-format on

        if (projection_options::IsLeftHandedInput<options>::value) {
            mat.col(2) *= -1.;
        }
        if (projection_options::IsColumnVector<options>::value) {
            mat.transposeInPlace();
        }
        return mat;
    }
} // namespace util
} // namespace osvr
#endif // INCLUDED_ProjectionMatrix_h_GUID_638B5832_DF1C_4BB7_9844_BF9810926107
