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

#ifndef INCLUDED_EigenTestHelpers_h_GUID_9C8A67BA_D846_4F94_8020_885E858600E3
#define INCLUDED_EigenTestHelpers_h_GUID_9C8A67BA_D846_4F94_8020_885E858600E3

// Internal Includes
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

namespace Eigen {
void PrintTo(Eigen::Quaterniond const &quat, ::std::ostream *os) {
    (*os) << "[(" << quat.vec().transpose() << "), " << quat.w() << "]";
}
void PrintTo(Eigen::Vector3d const &vec, ::std::ostream *os) {
    (*os) << vec.transpose();
}

/// Helper to convert to string for messages.
inline std::string to_string(Eigen::Quaterniond const &quat) {
    std::stringstream ss;
    PrintTo(quat, &ss);
    return ss.str();
}

/// Helper to convert to string for messages.
inline std::string to_string(Eigen::Vector3d const &vec) {
    std::stringstream ss;
    PrintTo(vec, &ss);
    return ss.str();
}
}
namespace EigenGTestCompareHelpers {
template <typename Scalar, std::size_t SIZE, typename Derived1,
          typename Derived2>
inline bool compareVecsEq_impl(Eigen::MatrixBase<Derived1> const &expected,
                               Eigen::MatrixBase<Derived2> const &actual) {
    for (std::size_t i = 0; i < SIZE; ++i) {
        ::testing::internal::FloatingPoint<Scalar> lhs(expected[i]),
            rhs(actual[i]);
        if (!lhs.AlmostEquals(rhs)) {
            return false;
        }
    }
    return true;
}
template <typename Scalar, typename T>
inline void formatStreamableValOrRowForMessage(std::ostream &os, T &&val) {
    os << std::setprecision(std::numeric_limits<Scalar>::digits10 + 2)
       << std::forward<T>(val);
}

template <typename Derived>
inline std::string
formatVectorForMessage(Eigen::DenseBase<Derived> const &vec) {
    using Scalar = typename Derived::Scalar;
    ::std::stringstream ss;
    if (1 == vec.rows()) {
        formatStreamableValOrRowForMessage<Scalar>(ss, vec);
    } else {
        formatStreamableValOrRowForMessage<Scalar>(ss, vec.transpose());
    }
    return ss.str();
}

template <typename Derived>
inline std::string
formatQuatForMessage(Eigen::QuaternionBase<Derived> const &quat) {
    using Scalar = typename Derived::Scalar;
    ::std::stringstream ss;
    ss << "[(";
    formatStreamableValOrRowForMessage<Scalar>(ss, quat.vec().transpose());
    ss << "), ";
    formatStreamableValOrRowForMessage<Scalar>(ss, quat.w());
    ss << "]";
    return ss.str();
}

/// based on CmpHelperFloatingPointEQ
template <typename Scalar>
::testing::AssertionResult
compareQuatsEq(const char *expected_expression, const char *actual_expression,
               Eigen::Quaternion<Scalar> const &expected,
               Eigen::Quaternion<Scalar> const &actual) {
    if (compareVecsEq_impl<Scalar, 4>(expected.coeffs(), actual.coeffs())) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::internal::EqFailure(
        expected_expression, actual_expression, formatQuatForMessage(expected),
        formatQuatForMessage(actual), false);
}

/// based on CmpHelperFloatingPointEQ
template <typename Scalar>
::testing::AssertionResult
compareQuatsNe(const char *expected_expression, const char *actual_expression,
               Eigen::Quaternion<Scalar> const &expected,
               Eigen::Quaternion<Scalar> const &actual) {
    if (!compareVecsEq_impl<Scalar, 4>(expected.coeffs(), actual.coeffs())) {
        return ::testing::AssertionSuccess();
    }
    return ::testing::AssertionFailure()
           << " Expected " << expected_expression << " ( "
           << formatQuatForMessage(expected) << ") != " << actual_expression
           << "\n  Got " << actual_expression << " = "
           << formatQuatForMessage(actual) << " instead.";
}

template <typename Derived1, typename Derived2>
inline void
checkVecStaticAssertions(Eigen::MatrixBase<Derived1> const &expected,
                         Eigen::MatrixBase<Derived2> const &actual) {

    static_assert(
        Derived1::IsVectorAtCompileTime,
        "Expected must be a vector at compile time to use this assertion.");
    static_assert(
        Derived2::IsVectorAtCompileTime,
        "Actual must be a vector at compile time to use this assertion.");
    static_assert(Derived1::SizeAtCompileTime == Derived2::SizeAtCompileTime,
                  "Expected and Actual must be the same size vector at compile "
                  "time to use this assertion.");
}
/// based on CmpHelperFloatingPointEQ
template <typename Scalar, typename Derived1, typename Derived2>
::testing::AssertionResult
compareVecsEq(const char *expected_expression, const char *actual_expression,
              Eigen::MatrixBase<Derived1> const &expected,
              Eigen::MatrixBase<Derived2> const &actual) {
    checkVecStaticAssertions(expected, actual);
    if (compareVecsEq_impl<Scalar, Derived1::SizeAtCompileTime>(expected,
                                                                actual)) {
        return ::testing::AssertionSuccess();
    }

    return ::testing::internal::EqFailure(
        expected_expression, actual_expression,
        formatVectorForMessage(expected), formatVectorForMessage(actual),
        false);
}

template <typename Scalar, typename Derived1, typename Derived2>
::testing::AssertionResult
compareVecsNe(const char *expected_expression, const char *actual_expression,
              Eigen::MatrixBase<Derived1> const &expected,
              Eigen::MatrixBase<Derived2> const &actual) {
    checkVecStaticAssertions(expected, actual);
    if (!compareVecsEq_impl<Scalar, Derived1::SizeAtCompileTime>(expected,
                                                                 actual)) {
        return ::testing::AssertionSuccess();
    }

    return ::testing::AssertionFailure()
           << " Expected " << expected_expression << " ( "
           << formatVectorForMessage(expected) << ") != " << actual_expression
           << "\n  Got " << actual_expression << " = "
           << formatVectorForMessage(actual) << " instead.";
#if 0
    ::testing::internal::EqFailure(
        expected_expression, actual_expression,
        ::testing::internal::StringStreamToString(&expected_ss),
        ::testing::internal::StringStreamToString(&actual_ss), false);
#endif
}
} // namespace EigenGTestCompareHelpers

#define ASSERT_QUAT_DOUBLE_EQ(expected, actual)                                \
    ASSERT_PRED_FORMAT2(::EigenGTestCompareHelpers::compareQuatsEq<double>,    \
                        expected, actual)

#define ASSERT_QUAT_DOUBLE_NE(expected, actual)                                \
    ASSERT_PRED_FORMAT2(::EigenGTestCompareHelpers::compareQuatsNe<double>,    \
                        expected, actual)
#define ASSERT_VEC_DOUBLE_EQ(expected, actual)                                 \
    ASSERT_PRED_FORMAT2(::EigenGTestCompareHelpers::compareVecsEq<double>,     \
                        expected, actual)
#define ASSERT_VEC_DOUBLE_NE(expected, actual)                                 \
    ASSERT_PRED_FORMAT2(::EigenGTestCompareHelpers::compareVecsNe<double>,     \
                        expected, actual)

#endif // INCLUDED_EigenTestHelpers_h_GUID_9C8A67BA_D846_4F94_8020_885E858600E3
