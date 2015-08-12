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

#ifndef INCLUDED_EigenExtras_h_GUID_7AE6CABA_333B_408A_C898_A2CBBE5BCE5D
#define INCLUDED_EigenExtras_h_GUID_7AE6CABA_333B_408A_C898_A2CBBE5BCE5D

// Internal Includes
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

namespace osvr {
namespace util {
    typedef Eigen::Matrix4d ColMatrix44d;
    typedef Eigen::Vector3d ColVector3d;
    typedef Eigen::Vector4d ColVector4d;
    namespace detail {
        /// @brief Type to use to wrap the Eigen::RowMajor constant.
        struct RowMajor : std::integral_constant<int, Eigen::RowMajor> {};
        /// @brief Type to use to wrap the Eigen::ColMajor constant.
        struct ColMajor : std::integral_constant<int, Eigen::ColMajor> {};
        /// @brief Creates a vector given the Eigen::ColMajor or Eigen::RowMajor
        /// integer arguments.
        template <int Size, int _Options = Eigen::ColMajor,
                  typename Scalar = double>
        using VectorImpl =
            Eigen::Matrix<Scalar, (_Options & Eigen::RowMajorBit) ? Size : 1,
                          (!(_Options & Eigen::RowMajorBit)) ? Size : 1>;
        /// @brief Creates a vector given the RowMajor or ColMajor type
        /// arguments.
        template <int Size, typename Ordering, typename Scalar = double>
        using Vector = VectorImpl<Size, Ordering::value, Scalar>;
    } // namespace detail

    /// @name Ordering keywords
    /// @brief Dummy names to use as a keyword argument to
    /// makeHomogeneousPoint() and makeHomogeneousVector()
    /// @{
    extern detail::RowMajor const *row_major;
    extern detail::ColMajor const *col_major;
    /// @}

    template <typename Derived, typename Ordering>
    inline detail::Vector<4, Ordering>
    makeHomogeneousPoint(Eigen::MatrixBase<Derived> const &vec,
                         Ordering * = col_major) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
        return (Vector<4, Ordering>() << vec, 1).finished();
    }
    template <typename Derived, typename Ordering>
    inline detail::Vector<4, Ordering>
    makeHomogeneousVector(Eigen::MatrixBase<Derived> const &vec,
                          Ordering const * = col_major) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
        return (Vector<4, Ordering>() << vec, 0).finished();
    }

    /// @brief Pulls the 3-dimensional point or vector from a 4-d vec,
    /// performing division by w if nonzero.
    inline Eigen::Vector3d extractPoint(Eigen::Vector4d const &homogenous) {
        return homogenous[3] == 0
                   ? homogenous.head<3>().eval()
                   : (homogenous.head<3>() / homogenous[3]).eval();
    }

    typedef Eigen::Matrix<double, 4, 4, Eigen::RowMajor> RowMatrix44d;
    using Eigen::RowVector3d;
    using Eigen::RowVector4d;

} // namespace util
} // namespace osvr

#endif // INCLUDED_EigenExtras_h_GUID_7AE6CABA_333B_408A_C898_A2CBBE5BCE5D
