/** @file
    @brief Implementation

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
// Copyright 2019 Collabora, Ltd.
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
#include "CatchEigen.h"
#include <osvr/Util/EigenQuatExponentialMap.h>

// Library/third-party includes
#ifdef HAVE_QUATLIB
#include "quat.h"
#endif // HAVE_QUATLIB
#include <catch2/catch.hpp>

// Standard includes
#include <array>

#ifndef HAVE_QUATLIB
#define Q_X 0
#define Q_Y 1
#define Q_Z 2
#define Q_W 3
#endif // !HAVE_QUATLIB

using osvr::util::quat_exp_map;

static const double SMALL = 0.1;
static const double SMALLER = 1.0e-5;

// Make an equality comparison for quaternions, for the sake of Catch2.

namespace Eigen {
static inline bool operator==(Quaterniond const &lhs, Quaterniond const &rhs) {
    return lhs.coeffs() == rhs.coeffs();
}
} // namespace Eigen

using Eigen::AngleAxisd;
using Eigen::Quaterniond;
using Eigen::Vector3d;

/// @name Quatlib interaction utilities
/// @{
/// Container for q_type that's C++-safe to pass around and such. To pass to
/// quatlib functions, use the `.data()` member function.
using QuatArray = std::array<double, 4>;

/// Convert Eigen vector to a quatlib (pure: w = 0) quaternion, wrapped in an
/// std::array.
static inline QuatArray toQuatlib(Vector3d const &vec) {
    QuatArray ret;
    ret[Q_W] = 0;
    ret[Q_X] = vec.x();
    ret[Q_Y] = vec.y();
    ret[Q_Z] = vec.z();
    return ret;
}

/// Convert Eigen quat to a quatlib quaternion, wrapped in an std::array.
static inline QuatArray toQuatlib(Quaterniond const &q) {
    QuatArray ret;
    ret[Q_W] = q.w();
    ret[Q_X] = q.x();
    ret[Q_Y] = q.y();
    ret[Q_Z] = q.z();
    return ret;
}

/// Takes a quatlib quaternion wrapped in an array and converts it to an
/// Eigen::Quaterniond, no questions asked.
static inline Quaterniond quatFromQuatlib(QuatArray const &arr) {
    return Quaterniond(arr[Q_W], arr[Q_X], arr[Q_Y], arr[Q_Z]);
}
/// Takes a quatlib quaternion wrapped in an array and converts it to an
/// Eigen::Vector3d, no questions asked - assumes it's a pure quaternion (w=0)
/// or that you just want the vector part.
static inline Vector3d vecFromQuatlib(QuatArray const &arr) {
    return Vector3d(arr[Q_X], arr[Q_Y], arr[Q_Z]);
}
/// @}

/// Creates a quaternion from angle+axis or identity,
/// and stores it along with a human-readable description.
class QuatCreator {
  public:
    explicit QuatCreator(QuatArray &&arr, std::string &&input)
        : m_coeffs(std::move(arr)), m_input(std::move(input)) {}

    static QuatCreator Identity() {
        return QuatCreator(toQuatlib(Eigen::Quaterniond::Identity()),
                           "Identity");
    }
    static QuatCreator AngleAxis(double angle, Eigen::Vector3d const &axis) {
        return QuatCreator(
            toQuatlib(Eigen::Quaterniond(Eigen::AngleAxisd(angle, axis))),
            formatAngleAxis(angle, axis));
    }

    Eigen::Quaterniond get() const { return quatFromQuatlib(m_coeffs); }

    std::string const &getDescription() const { return m_input; }

  private:
    QuatArray m_coeffs;
    std::string m_input;
    static std::string formatAngleAxis(double angle,
                                       Eigen::Vector3d const &axis) {
        std::ostringstream os;
        os << "Angle " << angle << ", Axis " << axis.transpose();
        return os.str();
    }
};

static inline ::std::ostream &operator<<(::std::ostream &os,
                                         QuatCreator const &q) {
    os << q.getDescription();
    return os;
}

using QuatVecPair = std::pair<QuatCreator, Eigen::Vector3d>;

static inline QuatVecPair makePairFromAngleAxis(double angle,
                                                Eigen::Vector3d const &axis) {
    return std::make_pair(QuatCreator::AngleAxis(angle, axis),
                          (angle * axis * 0.5).eval());
}

static inline ::std::ostream &operator<<(::std::ostream &os,
                                         QuatVecPair const &q) {
    os << q.first << " (quat-vec pair, vec " << q.second.transpose() << ")";
    return os;
}

static const Vector3d Vec3dZero = Vector3d::Zero();

/* Tests that take a unit quat as input */
TEST_CASE("UnitQuatInput") {
    const auto doTests = [](QuatCreator const &qCreator) {
        CAPTURE(qCreator);
        Quaterniond q = qCreator.get();
        CAPTURE(q);
        SECTION("Basic run ln") {
            REQUIRE_NOTHROW(quat_exp_map(q).ln());
            const Vector3d ln_q = quat_exp_map(q).ln();
            const bool isIdentityQuat = q.vec().norm() == 0;
            CAPTURE(isIdentityQuat);
            if (isIdentityQuat) {
                REQUIRE(ln_q == Vec3dZero);
            } else {
                REQUIRE_FALSE(ln_q == Vec3dZero);
            }
            SECTION("Round trip") {

                const Quaterniond exp_ln_q = quat_exp_map(ln_q).exp();

                REQUIRE(q == ApproxQuat(exp_ln_q));
            }
        }
#ifdef HAVE_QUATLIB
        SECTION("Quatlib roundtrip (exp(ln(q))) as ground truth") {
            QuatArray quatlib_q = toQuatlib(q);
            q_log(quatlib_q.data(), quatlib_q.data());
            q_exp(quatlib_q.data(), quatlib_q.data());
            REQUIRE(ApproxQuat(q) == quatFromQuatlib(quatlib_q));
        }
#endif // HAVE_QUATLIB
    };
    SECTION("Basic quats") {
        auto quatCreator = GENERATE(
            values({QuatCreator::Identity(),
                    QuatCreator::AngleAxis(EIGEN_PI / 2, Vector3d::UnitX()),
                    QuatCreator::AngleAxis(EIGEN_PI / 2, Vector3d::UnitY()),
                    QuatCreator::AngleAxis(EIGEN_PI / 2, Vector3d::UnitZ()),
                    QuatCreator::AngleAxis(-EIGEN_PI / 2, Vector3d::UnitX()),
                    QuatCreator::AngleAxis(-EIGEN_PI / 2, Vector3d::UnitY()),
                    QuatCreator::AngleAxis(-EIGEN_PI / 2, Vector3d::UnitZ())}));
        doTests(quatCreator);
    }
    SECTION("Small quats") {
        auto quatCreator = GENERATE(
            values({QuatCreator::AngleAxis(SMALL, Vector3d::UnitX()),
                    QuatCreator::AngleAxis(SMALL, Vector3d::UnitY()),
                    QuatCreator::AngleAxis(SMALL, Vector3d::UnitZ()),
                    QuatCreator::AngleAxis(SMALLER, Vector3d::UnitX()),
                    QuatCreator::AngleAxis(SMALLER, Vector3d::UnitY()),
                    QuatCreator::AngleAxis(SMALLER, Vector3d::UnitZ())}));
        doTests(quatCreator);
    }
    SECTION("Small negative quats") {
        auto quatCreator = GENERATE(
            values({QuatCreator::AngleAxis(-SMALL, Vector3d::UnitX()),
                    QuatCreator::AngleAxis(-SMALL, Vector3d::UnitY()),
                    QuatCreator::AngleAxis(-SMALL, Vector3d::UnitZ()),
                    QuatCreator::AngleAxis(-SMALLER, Vector3d::UnitX()),
                    QuatCreator::AngleAxis(-SMALLER, Vector3d::UnitY()),
                    QuatCreator::AngleAxis(-SMALLER, Vector3d::UnitZ())}));
        doTests(quatCreator);
    }

#if 0
QuatCreator::AngleAxis(EIGEN_PI, Vector3d::UnitX()),
QuatCreator::AngleAxis(EIGEN_PI, Vector3d::UnitY()),
QuatCreator::AngleAxis(EIGEN_PI, Vector3d::UnitZ()),
QuatCreator::AngleAxis(3 * EIGEN_PI / 2, Vector3d::UnitX()),
QuatCreator::AngleAxis(3 * EIGEN_PI / 2, Vector3d::UnitY()),
QuatCreator::AngleAxis(3 * EIGEN_PI / 2, Vector3d::UnitZ()),
#endif
}

/* Tests that take a rotation vector as input */
TEST_CASE("ExpMapVecInput") {
    const auto doTests = [](Vector3d const &v) {
        CAPTURE(v);
        SECTION("BasicRunExp") {
            REQUIRE_NOTHROW(quat_exp_map(v).exp());
            const Quaterniond exp_v = quat_exp_map(v).exp();

            const bool isNullRotation = (v == Vector3d::Zero());
            CAPTURE(isNullRotation);

            if (isNullRotation) {
                REQUIRE(exp_v == Quaterniond::Identity());
            } else {
                REQUIRE_FALSE(exp_v == Quaterniond::Identity());
            }

            SECTION("Round-trip") {
                Vector3d ln_exp_v = quat_exp_map(exp_v).ln();
                REQUIRE(ln_exp_v == ApproxVec(v));
            }
        }

#ifdef HAVE_QUATLIB
        SECTION("Quatlib roundtrip (ln(exp(v))) as ground truth") {
            QuatArray quatlib_q = toQuatlib(v);
            q_exp(quatlib_q.data(), quatlib_q.data());
            q_log(quatlib_q.data(), quatlib_q.data());
            REQUIRE(ApproxVec(v) == vecFromQuatlib(quatlib_q));
        }
#endif // HAVE_QUATLIB
    };
    SECTION("BasicVecs") {
        Vector3d v = GENERATE(values(
            {Vector3d(Vector3d::Zero()), Vector3d(EIGEN_PI / 2, 0, 0),
             Vector3d(0, EIGEN_PI / 2, 0), Vector3d(0, 0, EIGEN_PI / 2),
             Vector3d(-EIGEN_PI / 2, 0, 0), Vector3d(0, -EIGEN_PI / 2, 0),
             Vector3d(0, 0, -EIGEN_PI / 2)}));
        doTests(v);
    }
    SECTION("SmallVecs") {
        Vector3d v = GENERATE(
            values({Vector3d(SMALL, 0, 0), Vector3d(0, SMALL, 0),
                    Vector3d(0, 0, SMALL), Vector3d(SMALLER, 0, 0),
                    Vector3d(0, SMALLER, 0), Vector3d(0, 0, SMALLER)}));
        doTests(v);
    }
    SECTION("SmallNegativeVecs") {
        Vector3d v = GENERATE(
            values({Vector3d(-SMALL, 0, 0), Vector3d(0, -SMALL, 0),
                    Vector3d(0, 0, -SMALL), Vector3d(-SMALLER, 0, 0),
                    Vector3d(0, -SMALLER, 0), Vector3d(0, 0, -SMALLER)}));
        doTests(v);
    }
}

TEST_CASE("SimpleEquivalencies-Ln") {
    REQUIRE(Vec3dZero == Vector3d(quat_exp_map(Quaterniond::Identity()).ln()));
}

TEST_CASE("SimpleEquivalencies-Exp") {
    REQUIRE(Quaterniond::Identity() ==
            Quaterniond(quat_exp_map(Vec3dZero).exp()));
}

/* Tests that take a pair of equivalent quaternion and vector as input */
TEST_CASE("EquivalentInput") {
    const auto doTests = [](QuatCreator const &qCreator, Vector3d const &v) {
        CAPTURE(v);
        CAPTURE(qCreator);
        Quaterniond q = qCreator.get();
        CAPTURE(q);
        Vector3d ln_q = quat_exp_map(q).ln();
        Quaterniond exp_v = quat_exp_map(v).exp();

        SECTION("Ln") { REQUIRE(ln_q == ApproxVec(v)); }

        SECTION("Exp") { REQUIRE(exp_v == ApproxQuat(q)); }

#ifdef HAVE_QUATLIB
        SECTION("Compare ln with quatlib") {
            QuatArray quatlib_q = toQuatlib(q);
            q_log(quatlib_q.data(), quatlib_q.data());
            REQUIRE(vecFromQuatlib(quatlib_q) == ln_q);
        }
        SECTION("Compare exp with quatlib") {
            QuatArray quatlib_q = toQuatlib(v);
            q_exp(quatlib_q.data(), quatlib_q.data());
            q_normalize(quatlib_q.data(), quatlib_q.data());

            REQUIRE(quatFromQuatlib(quatlib_q) == exp_v);
        }
#endif // HAVE_QUATLIB
    };
    SECTION("HalfPiMultiples") {
        QuatVecPair qvp = GENERATE(
            values({makePairFromAngleAxis(EIGEN_PI / 2, Vector3d::UnitX()),
                    makePairFromAngleAxis(EIGEN_PI / 2, Vector3d::UnitY()),
                    makePairFromAngleAxis(EIGEN_PI / 2, Vector3d::UnitZ()),
                    makePairFromAngleAxis(-EIGEN_PI / 2, Vector3d::UnitX()),
                    makePairFromAngleAxis(-EIGEN_PI / 2, Vector3d::UnitY()),
                    makePairFromAngleAxis(-EIGEN_PI / 2, Vector3d::UnitZ())}));
        doTests(qvp.first, qvp.second);
    }
    SECTION("SmallEquivalentValues") {
        QuatVecPair qvp = GENERATE(
            values({makePairFromAngleAxis(SMALL, Vector3d::UnitX()),
                    makePairFromAngleAxis(SMALL, Vector3d::UnitY()),
                    makePairFromAngleAxis(SMALL, Vector3d::UnitZ()),
                    makePairFromAngleAxis(SMALLER, Vector3d::UnitX()),
                    makePairFromAngleAxis(SMALLER, Vector3d::UnitY()),
                    makePairFromAngleAxis(SMALLER, Vector3d::UnitZ())}));
        doTests(qvp.first, qvp.second);
    }
    SECTION("SmallNegativeVecs") {
        QuatVecPair qvp = GENERATE(
            values({makePairFromAngleAxis(-SMALL, Vector3d::UnitX()),
                    makePairFromAngleAxis(-SMALL, Vector3d::UnitY()),
                    makePairFromAngleAxis(-SMALL, Vector3d::UnitZ()),
                    makePairFromAngleAxis(-SMALLER, Vector3d::UnitX()),
                    makePairFromAngleAxis(-SMALLER, Vector3d::UnitY()),
                    makePairFromAngleAxis(-SMALLER, Vector3d::UnitZ())}));
        doTests(qvp.first, qvp.second);
    }
}
