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
        template <int _Options>
        using IsEigenOptionsRowMajor =
            std::integral_constant<bool,
                                   ((_Options & Eigen::RowMajorBit) != 0)>;
        /// @brief Creates a vector given the Eigen::ColMajor or Eigen::RowMajor
        /// integer arguments.
        template <int Size, int _Options = Eigen::ColMajor,
                  typename Scalar = double>
        using VectorImpl =
            Eigen::Matrix<Scalar,
                          IsEigenOptionsRowMajor<_Options>::value ? Size : 1,
                          IsEigenOptionsRowMajor<_Options>::value ? 1 : Size>;
        /// @brief Creates a vector given the RowMajor or ColMajor type
        /// arguments.
        template <int Size, typename Ordering, typename Scalar = double>
        using Vector = VectorImpl<Size, Ordering::value, Scalar>;

        template <int Size, typename PrototypeVector>
        using SameLayoutVector =
            Eigen::Matrix<typename PrototypeVector::Scalar,
                          PrototypeVector::IsRowMajor ? Size : 1,
                          PrototypeVector::IsRowMajor ? 1 : Size>;
    } // namespace detail

    /// @name Ordering keywords
    /// @brief Dummy names to use as a keyword argument to
    /// makeHomogeneousPoint() and makeHomogeneousVector()
    /// @{
    extern detail::RowMajor const *row_major;
    extern detail::ColMajor const *col_major;
    /// @}

    /// @brief Makes a 3D vector into a 4D homogeneous point, with the same
    /// options (scalar, row vs col vector) as the input.
    template <typename Derived>
    inline detail::SameLayoutVector<4, Derived>
    makeHomogeneousPoint(Eigen::MatrixBase<Derived> const &vec) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
        return (detail::SameLayoutVector<4, Derived>() << vec, 1).finished();
    }

    template <typename Scalar, typename Ordering>
    inline detail::Vector<4, Ordering, Scalar>
    makeHomogeneousPoint(Scalar x, Scalar y, Scalar z, Ordering * = col_major) {
        return detail::Vector<4, Ordering>(x, y, z, 1);
    }

    /// @brief Makes a 3D vector into a 4D homogeneous vector, with the same
    /// options (scalar, row vs col vector) as the input.
    template <typename Derived>
    inline detail::SameLayoutVector<4, Derived>
    makeHomogeneousVector(Eigen::MatrixBase<Derived> const &vec) {
        EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(Derived, 3);
        return (detail::SameLayoutVector<4, Derived>() << vec, 0).finished();
    }

    template <typename Scalar, typename Ordering>
    inline detail::Vector<4, Ordering, Scalar>
    makeHomogeneousVector(Scalar x, Scalar y, Scalar z,
                          Ordering * = col_major) {
        return detail::Vector<4, Ordering>(x, y, z, 0);
    }

    /// @brief Pulls the 3-dimensional point or vector from a 4-d vec,
    /// performing division by w if nonzero.template <typename Derived>
    template <typename Derived>
    inline detail::SameLayoutVector<3, Derived>
    extractPoint(Eigen::MatrixBase<Derived> const &homogenous) {
        return homogenous[3] == 0
                   ? homogenous.template head<3>().eval()
                   : (homogenous.template head<3>() / homogenous[3]).eval();
    }
    typedef Eigen::Matrix<double, 4, 4, Eigen::RowMajor> RowMatrix44d;
    using Eigen::RowVector3d;
    using Eigen::RowVector4d;

} // namespace util
} // namespace osvr

#endif // INCLUDED_EigenExtras_h_GUID_7AE6CABA_333B_408A_C898_A2CBBE5BCE5D
