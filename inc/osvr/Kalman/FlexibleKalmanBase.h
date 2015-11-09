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

#ifndef INCLUDED_FlexibleKalmanBase_h_GUID_E4CC84A0_1B4F_4389_95ED_9C37F5FEE95D
#define INCLUDED_FlexibleKalmanBase_h_GUID_E4CC84A0_1B4F_4389_95ED_9C37F5FEE95D

// Internal Includes
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
// - none

// Standard includes
#include <type_traits>

#ifndef OSVR_KALMAN_DEBUG_OUTPUT
#define OSVR_KALMAN_DEBUG_OUTPUT(Name, Value)
#endif

namespace osvr {
/// @brief Header-only framework for building Kalman-style filters, prediction,
/// and sensor fusion
namespace kalman {
    /// @brief Type aliases, including template type aliases.
    namespace types {
        /// Common scalar type
        using Scalar = double;

        /// Type for dimensions
        using DimensionType = std::size_t;

        /// Type constant for dimensions
        template <DimensionType n>
        using DimensionConstant = std::integral_constant<DimensionType, n>;

        struct HasDimensionBase {};
    } // namespace types

    /// Convenience base class for things (like states and measurements) that
    /// have a dimension.
    template <types::DimensionType DIM>
    struct HasDimension : types::HasDimensionBase {
        using Dimension = types::DimensionConstant<DIM>;
    };

    namespace types {
        namespace detail {
            template <typename T, typename = void> struct Dimension_impl {
                using type = DimensionConstant<T::DIMENSION>;
            };
            // explicit specialization
            template <DimensionType n>
            struct Dimension_impl<DimensionConstant<n>, void> {
                using type = DimensionConstant<n>;
            };
            // explicit specialization
            template <typename T>
            struct Dimension_impl<T, typename std::enable_if<std::is_base_of<
                                         HasDimensionBase, T>::value>::type> {
                using type = typename T::Dimension;
            };
        } // namespace detail
        /// Given a state or measurement, get the dimension as a
        /// std::integral_constant
        template <typename T>
        using Dimension = typename detail::Dimension_impl<T>::type;

        /// Given a filter type, get the state type.
        template <typename FilterType>
        using StateType = typename FilterType::State;

        /// Given a filter type, get the process model type.
        template <typename FilterType>
        using ProcessModelType = typename FilterType::ProcessModel;

        /// A vector of length n
        template <DimensionType n> using Vector = Eigen::Matrix<Scalar, n, 1>;

        /// A vector of length = dimension of T
        template <typename T> using DimVector = Vector<Dimension<T>::value>;

        /// A square matrix, n x n
        template <DimensionType n>
        using SquareMatrix = Eigen::Matrix<Scalar, n, n>;

        /// A square matrix, n x n, where n is the dimension of T
        template <typename T>
        using DimSquareMatrix = SquareMatrix<Dimension<T>::value>;

        /// A square diagonal matrix, n x n
        template <DimensionType n>
        using DiagonalMatrix = Eigen::DiagonalMatrix<Scalar, n>;

        /// A square diagonal matrix, n x n, where n is the dimension of T
        template <typename T>
        using DimDiagonalMatrix = DiagonalMatrix<Dimension<T>::value>;

        /// A matrix with rows = m,  cols = n
        template <DimensionType m, DimensionType n>
        using Matrix = Eigen::Matrix<Scalar, m, n>;

        /// A matrix with rows = dimension of T, cols = dimension of U
        template <typename T, typename U>
        using DimMatrix = Matrix<Dimension<T>::value, Dimension<U>::value>;

    } // namespace types

    /// Computes P-
    ///
    /// Usage is optional, most likely called from the process model
    /// `updateState()`` method.
    template <typename StateType, typename ProcessModelType>
    inline types::DimSquareMatrix<StateType>
    predictErrorCovariance(StateType const &state,
                           ProcessModelType &processModel, double dt) {
        types::DimSquareMatrix<StateType> A =
            processModel.getStateTransitionMatrix(state, dt);
        // OSVR_KALMAN_DEBUG_OUTPUT("State transition matrix", A);
        types::DimSquareMatrix<StateType> P = state.errorCovariance();
        /// @todo Determine if the fact that Q is (at least in one case)
        /// symmetrical implies anything else useful performance-wise here or
        /// later in the data flow.
        // auto Q = processModel.getSampledProcessNoiseCovariance(dt);
        OSVR_KALMAN_DEBUG_OUTPUT(
            "Process Noise Covariance Q",
            processModel.getSampledProcessNoiseCovariance(dt));
        return A * P * A.transpose() +
               processModel.getSampledProcessNoiseCovariance(dt);
    }

} // namespace kalman
} // namespace osvr

#endif // INCLUDED_FlexibleKalmanBase_h_GUID_E4CC84A0_1B4F_4389_95ED_9C37F5FEE95D
