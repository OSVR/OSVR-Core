/** @file
    @brief Helpers for using Eigen with Catch2

    @date 2019

    @author Ryan Pavlik <ryan.pavlik@collabora.com>

    Based in part on catch_approx.h from the Catch2 distribution
*/

// Copyright 2019, Collabora, Ltd.
// Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
//
// SPDX-License-Identifier: BSL-1.0
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <catch2/catch.hpp>

#include <iomanip>
#include <sstream>

template <typename VecType> class ApproxVecHolder {
    using Scalar = typename VecType::Scalar;
    const VecType expected;
    Scalar precision_ = Eigen::NumTraits<Scalar>::dummy_precision();
    template <typename T> bool equalityComparisonImpl(T const &other) const {
        if (expected == VecType::Zero()) {
            // isApprox is no good for comparing to zero vector.
            return other.isApproxToConstant(0, precision_);
        }
        return expected.isApprox(other, precision_);
    }

  public:
    VecType const &value() const { return expected; }
    explicit ApproxVecHolder(VecType const &v) : expected(v) {}

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<VecType, T>::value>::type>
    friend bool operator==(const T &lhs, ApproxVecHolder<VecType> const &rhs) {
        auto lhs_v = static_cast<VecType>(lhs);
        return rhs.equalityComparisonImpl(lhs_v);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<VecType, T>::value>::type>
    friend bool operator==(ApproxVecHolder<VecType> const &lhs, const T &rhs) {
        return operator==(rhs, lhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<VecType, T>::value>::type>
    friend bool operator!=(T const &lhs, ApproxVecHolder<VecType> const &rhs) {
        return !operator==(lhs, rhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<VecType, T>::value>::type>
    friend bool operator!=(ApproxVecHolder<VecType> const &lhs, T const &rhs) {
        return !operator==(rhs, lhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<Scalar, T>::value>::type>
    ApproxVecHolder &precision(T const &newPrecision) {
        precision_ = static_cast<Scalar>(newPrecision);
        return *this;
    }
};

template <typename Derived>
static inline ApproxVecHolder<Derived>
ApproxVec(Eigen::MatrixBase<Derived> const &vec) {
    EIGEN_STATIC_ASSERT_FIXED_SIZE(Derived);
    EIGEN_STATIC_ASSERT_VECTOR_ONLY(Derived);
    return ApproxVecHolder<Derived>{vec.derived()};
}

template <typename QuatType> class ApproxQuatHolder {
    using Scalar = typename QuatType::Scalar;
    using VecType = Eigen::Matrix<Scalar, 1, 4>;
    const QuatType expected;
    ApproxVecHolder<VecType> inner;
    template <typename T> bool equalityComparisonImpl(T const &other) const {
        return inner == other.coeffs();
    }

  public:
    QuatType const &value() const { return expected; }
    explicit ApproxQuatHolder(QuatType const &q)
        : expected(q), inner(q.coeffs()) {}

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<QuatType, T>::value>::type>
    friend bool operator==(const T &lhs,
                           ApproxQuatHolder<QuatType> const &rhs) {
        auto lhs_v = static_cast<QuatType>(lhs);
        return rhs.equalityComparisonImpl(lhs_v);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<QuatType, T>::value>::type>
    friend bool operator==(ApproxQuatHolder<QuatType> const &lhs,
                           const T &rhs) {
        return operator==(rhs, lhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<QuatType, T>::value>::type>
    friend bool operator!=(T const &lhs,
                           ApproxQuatHolder<QuatType> const &rhs) {
        return !operator==(lhs, rhs);
    }

    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<QuatType, T>::value>::type>
    friend bool operator!=(ApproxQuatHolder<QuatType> const &lhs,
                           T const &rhs) {
        return !operator==(rhs, lhs);
    }
    template <typename T, typename = typename std::enable_if<
                              std::is_constructible<Scalar, T>::value>::type>
    ApproxQuatHolder &precision(T const &newPrecision) {
        inner.precision(newPrecision);
        return *this;
    }
};

template <typename Derived>
static inline ApproxQuatHolder<Derived>
ApproxQuat(Eigen::QuaternionBase<Derived> const &q) {
    return ApproxQuatHolder<Derived>{q.derived()};
}
template <typename Derived>
static inline std::string
outputVecForMessage(Eigen::MatrixBase<Derived> const &value) {
    using Scalar = typename Derived::Scalar;
    std::ostringstream os;
    os << std::setprecision(std::numeric_limits<Scalar>::digits10 + 2);
    if (1 == value.rows()) {
        os << value;
    } else if (1 == value.cols()) {
        os << value.transpose();
    } else {
        os << "\n" << value;
    }
    return os.str();
}
namespace Catch {

template <> struct StringMaker<Eigen::Quaterniond> {
    static std::string convert(Eigen::Quaterniond const &value) {
        std::ostringstream os;
        os << std::setprecision(std::numeric_limits<double>::digits10 + 2);
        os << "[(" << value.vec().transpose() << "), " << value.w() << "]";
        return os.str();
    }
};
template <> struct StringMaker<Eigen::Vector3d> {
    static std::string convert(Eigen::Vector3d const &value) {
        return outputVecForMessage(value);
    }
};
template <> struct StringMaker<Eigen::Vector4d> {
    static std::string convert(Eigen::Vector4d const &value) {
        return outputVecForMessage(value);
    }
};
template <typename T> struct StringMaker<ApproxVecHolder<T>> {
    static std::string convert(ApproxVecHolder<T> const &value) {
        std::ostringstream os;
        os << "ApproxVec(" << StringMaker<T>::convert(value.value()) << ")";
        return os.str();
    }
};
template <typename T> struct StringMaker<ApproxQuatHolder<T>> {
    static std::string convert(ApproxQuatHolder<T> const &value) {
        std::ostringstream os;
        os << "ApproxQuat(" << StringMaker<T>::convert(value.value()) << ")";
        return os.str();
    }
};
} // namespace Catch
