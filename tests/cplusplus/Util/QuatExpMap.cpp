/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Util/EigenQuatExponentialMap.h>

// Library/third-party includes
#include "gtest/gtest.h"
#include "../EigenTestHelpers.h"
#include "quat.h"

// Standard includes
#include <array>

using osvr::util::quat_exp_map;

static const double SMALL = 0.1;
static const double SMALLER = 1.0e-5;

namespace quat_array {

using array_type = std::array<double, 4>;
inline void set(array_type &arr, Eigen::Quaterniond const &quat) {
    Eigen::Vector4d::Map(arr.data()) = quat.coeffs();
}

inline array_type set(Eigen::Quaterniond const &quat) {
    array_type ret;
    set(ret, quat);
    return ret;
}

inline void get(Eigen::Quaterniond &quat, array_type const &arr) {
    quat.coeffs() = Eigen::Vector4d::Map(arr.data());
}

inline Eigen::Quaterniond get(array_type const &arr) {
    Eigen::Quaterniond ret;
    get(ret, arr);
    return ret;
}

} // namespace quat_array

using Eigen::Quaterniond;
using Eigen::Vector3d;
using Eigen::AngleAxisd;

/// @name Quatlib interaction utilities
/// @{
/// Container for q_type that's C++-safe to pass around and such. To pass to
/// quatlib functions, use the `.data()` member function.
using QuatlibQuatArray = std::array<double, 4>;

/// Convert Eigen vector to a quatlib (pure: w = 0) quaternion, wrapped in an
/// std::array.
inline QuatlibQuatArray toQuatlib(Vector3d const &vec) {
    QuatlibQuatArray ret;
    ret[Q_W] = 0;
    ret[Q_X] = vec.x();
    ret[Q_Y] = vec.y();
    ret[Q_Z] = vec.z();
    return ret;
}
/// Convert Eigen quat to a quatlib quaternion, wrapped in an std::array.
inline QuatlibQuatArray toQuatlib(Quaterniond const &q) {
    QuatlibQuatArray ret;
    ret[Q_W] = q.w();
    ret[Q_X] = q.x();
    ret[Q_Y] = q.y();
    ret[Q_Z] = q.z();
    return ret;
}

/// Takes a quatlib quaternion wrapped in an array and converts it to an
/// Eigen::Quaterniond, no questions asked.
inline Quaterniond quatFromQuatlib(QuatlibQuatArray const &arr) {
    return Quaterniond(arr[Q_W], arr[Q_X], arr[Q_Y], arr[Q_Z]);
}
/// Takes a quatlib quaternion wrapped in an array and converts it to an
/// Eigen::Vector3d, no questions asked - assumes it's a pure quaternion (w=0)
/// or that you just want the vector part.
inline Vector3d vecFromQuatlib(QuatlibQuatArray const &arr) {
    return Vector3d(arr[Q_X], arr[Q_Y], arr[Q_Z]);
}
/// @}

inline std::string formatAngleAxis(double angle, Eigen::Vector3d const &axis) {
    std::ostringstream os;
    os << "Angle " << angle << ", Axis " << axis.transpose();
    return os.str();
}

class QuatCreator {
  public:
    explicit QuatCreator(quat_array::array_type &&arr, std::string &&input)
        : m_coeffs(std::move(arr)), m_input(std::move(input)) {}

    static QuatCreator Identity() {
        return QuatCreator(quat_array::set(Eigen::Quaterniond::Identity()),
                           "Identity");
    }
    static QuatCreator AngleAxis(double angle, Eigen::Vector3d const &axis) {
        return QuatCreator(
            quat_array::set(Eigen::Quaterniond(Eigen::AngleAxisd(angle, axis))),
            formatAngleAxis(angle, axis));
    }

    Eigen::Quaterniond get() const { return quat_array::get(m_coeffs); }

    std::string const &getDescription() const { return m_input; }

  private:
    std::array<double, 4> m_coeffs;
    std::string m_input;
};

inline ::std::ostream &operator<<(::std::ostream &os, QuatCreator const &q) {
    os << q.getDescription();
    return os;
}

using QuatVecPair = std::pair<QuatCreator, Eigen::Vector3d>;

inline QuatVecPair makePairFromAngleAxis(double angle,
                                         Eigen::Vector3d const &axis) {
    return std::make_pair(QuatCreator::AngleAxis(angle, axis),
                          (angle * axis).eval());
}

inline ::std::ostream &operator<<(::std::ostream &os, QuatVecPair const &q) {
    os << q.first << " (quat-vec pair, vec " << q.second.transpose() << ")";
    return os;
}
class UnitQuatInput : public ::testing::TestWithParam<QuatCreator> {
  public:
    // You can implement all the usual fixture class members here.
    // To access the test parameter, call GetParam() from class
    // TestWithParam<T>.

    // Gets the parameter and converts it to a real Eigen quat.
    Eigen::Quaterniond getQuat() const { return GetParam().get(); }
};

TEST_P(UnitQuatInput, BasicRunLn) {
    ASSERT_NO_THROW(quat_exp_map(getQuat()).ln());
    if (getQuat().vec().norm() > 0) {
        // that is, if this isn't the identity
        ASSERT_VEC_DOUBLE_NE(Vector3d::Zero(), quat_exp_map(getQuat()).ln());
    }
}

TEST_P(UnitQuatInput, RoundTripLn) {
    ASSERT_QUAT_DOUBLE_EQ(getQuat(),
                          quat_exp_map(quat_exp_map(getQuat()).ln()).exp());
}

TEST_P(UnitQuatInput, QuatlibRoundTripLn) {
    QuatlibQuatArray quatlib_q = toQuatlib(getQuat());
    q_log(quatlib_q.data(), quatlib_q.data());
    q_exp(quatlib_q.data(), quatlib_q.data());
    ASSERT_QUAT_DOUBLE_EQ(getQuat(), quatFromQuatlib(quatlib_q))
        << "Actual is result of applying q_log then q_exp to the input "
           "quaternion.\nTests Quatlib as a source of truth - does not test "
           "OSVR-custom code.";
}

INSTANTIATE_TEST_CASE_P(
    BasicQuats, UnitQuatInput,
    ::testing::Values(QuatCreator::Identity(),
                      QuatCreator::AngleAxis(M_PI / 2, Vector3d::UnitX()),
                      QuatCreator::AngleAxis(M_PI / 2, Vector3d::UnitY()),
                      QuatCreator::AngleAxis(M_PI / 2, Vector3d::UnitZ()),
                      QuatCreator::AngleAxis(-M_PI / 2, Vector3d::UnitX()),
                      QuatCreator::AngleAxis(-M_PI / 2, Vector3d::UnitY()),
                      QuatCreator::AngleAxis(-M_PI / 2, Vector3d::UnitZ())));

INSTANTIATE_TEST_CASE_P(
    SmallQuats, UnitQuatInput,
    ::testing::Values(QuatCreator::AngleAxis(SMALL, Vector3d::UnitX()),
                      QuatCreator::AngleAxis(SMALL, Vector3d::UnitY()),
                      QuatCreator::AngleAxis(SMALL, Vector3d::UnitZ()),
                      QuatCreator::AngleAxis(SMALLER, Vector3d::UnitX()),
                      QuatCreator::AngleAxis(SMALLER, Vector3d::UnitY()),
                      QuatCreator::AngleAxis(SMALLER, Vector3d::UnitZ())));
INSTANTIATE_TEST_CASE_P(
    SmallNegativeQuats, UnitQuatInput,
    ::testing::Values(QuatCreator::AngleAxis(-SMALL, Vector3d::UnitX()),
                      QuatCreator::AngleAxis(-SMALL, Vector3d::UnitY()),
                      QuatCreator::AngleAxis(-SMALL, Vector3d::UnitZ()),
                      QuatCreator::AngleAxis(-SMALLER, Vector3d::UnitX()),
                      QuatCreator::AngleAxis(-SMALLER, Vector3d::UnitY()),
                      QuatCreator::AngleAxis(-SMALLER, Vector3d::UnitZ())));
#if 0
QuatCreator::AngleAxis(M_PI, Vector3d::UnitX()),
QuatCreator::AngleAxis(M_PI, Vector3d::UnitY()),
QuatCreator::AngleAxis(M_PI, Vector3d::UnitZ()),
QuatCreator::AngleAxis(3 * M_PI / 2, Vector3d::UnitX()),
QuatCreator::AngleAxis(3 * M_PI / 2, Vector3d::UnitY()),
QuatCreator::AngleAxis(3 * M_PI / 2, Vector3d::UnitZ()),
#endif

class ExpMapVecInput : public ::testing::TestWithParam<Eigen::Vector3d> {
  public:
    // You can implement all the usual fixture class members here.
    // To access the test parameter, call GetParam() from class
    // TestWithParam<T>.
};

TEST_P(ExpMapVecInput, BasicRunExp) {
    ASSERT_NO_THROW(quat_exp_map(GetParam()).exp());
    if (GetParam() != Vector3d::Zero()) {
        // that is, if this isn't the null rotation
        ASSERT_QUAT_DOUBLE_NE(Quaterniond::Identity(),
                              quat_exp_map(GetParam()).exp());
    }
}
TEST_P(ExpMapVecInput, RoundTripExp) {
    ASSERT_VEC_DOUBLE_EQ(GetParam(),
                         quat_exp_map(quat_exp_map(GetParam()).exp()).ln());
}

TEST_P(ExpMapVecInput, QuatlibRoundTripExp) {
    QuatlibQuatArray quatlib_q = toQuatlib(GetParam());
    q_exp(quatlib_q.data(), quatlib_q.data());
    q_log(quatlib_q.data(), quatlib_q.data());
    Vector3d vec = GetParam();
    ASSERT_VEC_DOUBLE_EQ(GetParam(), vecFromQuatlib(quatlib_q))
        << "Actual is result of applying q_exp then q_log to the input "
           "quaternion.\nTests Quatlib as a source of truth - does not test "
           "OSVR-custom code.";
}

INSTANTIATE_TEST_CASE_P(
    BasicVecs, ExpMapVecInput,
    ::testing::Values(Vector3d::Zero(), Vector3d(M_PI / 2, 0, 0),
                      Vector3d(0, M_PI / 2, 0), Vector3d(0, 0, M_PI / 2),
                      Vector3d(-M_PI / 2, 0, 0), Vector3d(0, -M_PI / 2, 0),
                      Vector3d(0, 0, -M_PI / 2)));
INSTANTIATE_TEST_CASE_P(
    SmallVecs, ExpMapVecInput,
    ::testing::Values(Vector3d(SMALL, 0, 0), Vector3d(0, SMALL, 0),
                      Vector3d(0, 0, SMALL), Vector3d(SMALLER, 0, 0),
                      Vector3d(0, SMALLER, 0), Vector3d(0, 0, SMALLER)));
INSTANTIATE_TEST_CASE_P(
    SmallNegativeVecs, ExpMapVecInput,
    ::testing::Values(Vector3d(-SMALL, 0, 0), Vector3d(0, -SMALL, 0),
                      Vector3d(0, 0, -SMALL), Vector3d(-SMALLER, 0, 0),
                      Vector3d(0, -SMALLER, 0), Vector3d(0, 0, -SMALLER)));

inline Quaterniond makeQuat(double angle, Vector3d const &axis) {
    return Quaterniond(AngleAxisd(angle, axis));
}

TEST(SimpleEquivalencies, Ln) {
    ASSERT_VEC_DOUBLE_EQ(Vector3d::Zero(),
                         quat_exp_map(Quaterniond::Identity()).ln());
}

TEST(SimpleEquivalencies, Exp) {
    ASSERT_QUAT_DOUBLE_EQ(Quaterniond::Identity(),
                          quat_exp_map(Vector3d::Zero().eval()).exp());
}

class EquivalentInput : public ::testing::TestWithParam<QuatVecPair> {
  public:
    // Gets the first part of theparameter and converts it to a real Eigen quat.
    Eigen::Quaterniond getQuat() const { return GetParam().first.get(); }
    // Gets the second part of the parameter: the vec
    Eigen::Vector3d getVec() const { return GetParam().second; }
};

/// @todo Test appears broken?
TEST_P(EquivalentInput, DISABLED_Ln) {
    ASSERT_VEC_DOUBLE_EQ(getVec(), quat_exp_map(getQuat()).ln());
}

TEST_P(EquivalentInput, LnCompareWithQuatlib) {
    QuatlibQuatArray quatlib_q = toQuatlib(getQuat());
    q_log(quatlib_q.data(), quatlib_q.data());

    ASSERT_VEC_DOUBLE_EQ(vecFromQuatlib(quatlib_q),
                         quat_exp_map(getQuat()).ln())
        << "\n'Expected' value is the output of quatlib's q_log function on "
           "the same input: " +
               to_string(getQuat()) + "\n";
}

/// @todo Test appears broken?
TEST_P(EquivalentInput, DISABLED_Exp) {
    ASSERT_QUAT_DOUBLE_EQ(getQuat(), quat_exp_map(getVec()).exp());
}

TEST_P(EquivalentInput, ExpCompareWithQuatlib) {
    QuatlibQuatArray quatlib_q = toQuatlib(getVec());
    q_exp(quatlib_q.data(), quatlib_q.data());
    q_normalize(quatlib_q.data(), quatlib_q.data());

    ASSERT_QUAT_DOUBLE_EQ(quatFromQuatlib(quatlib_q),
                          quat_exp_map(getVec()).exp())
        << "\n'Expected' value is the normalized output of quatlib's q_exp "
           "function on the same input: " +
               to_string(getVec()) + "\n";
}

INSTANTIATE_TEST_CASE_P(
    HalfPiMultiples, EquivalentInput,
    ::testing::Values(makePairFromAngleAxis(M_PI / 2, Vector3d::UnitX()),
                      makePairFromAngleAxis(M_PI / 2, Vector3d::UnitY()),
                      makePairFromAngleAxis(M_PI / 2, Vector3d::UnitZ()),
                      makePairFromAngleAxis(-M_PI / 2, Vector3d::UnitX()),
                      makePairFromAngleAxis(-M_PI / 2, Vector3d::UnitY()),
                      makePairFromAngleAxis(-M_PI / 2, Vector3d::UnitZ())

                          ));

INSTANTIATE_TEST_CASE_P(
    SmallValues, EquivalentInput,
    ::testing::Values(makePairFromAngleAxis(SMALL, Vector3d::UnitX()),
                      makePairFromAngleAxis(SMALL, Vector3d::UnitY()),
                      makePairFromAngleAxis(SMALL, Vector3d::UnitZ()),
                      makePairFromAngleAxis(SMALLER, Vector3d::UnitX()),
                      makePairFromAngleAxis(SMALLER, Vector3d::UnitY()),
                      makePairFromAngleAxis(SMALLER, Vector3d::UnitZ())));

INSTANTIATE_TEST_CASE_P(
    SmallNegativeValues, EquivalentInput,
    ::testing::Values(makePairFromAngleAxis(-SMALL, Vector3d::UnitX()),
                      makePairFromAngleAxis(-SMALL, Vector3d::UnitY()),
                      makePairFromAngleAxis(-SMALL, Vector3d::UnitZ()),
                      makePairFromAngleAxis(-SMALLER, Vector3d::UnitX()),
                      makePairFromAngleAxis(-SMALLER, Vector3d::UnitY()),
                      makePairFromAngleAxis(-SMALLER, Vector3d::UnitZ())));
