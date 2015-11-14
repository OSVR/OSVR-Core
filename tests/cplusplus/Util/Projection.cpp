/** @file
    @brief Test implementation

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

// Internal Includes
#include <osvr/Util/ProjectionMatrix.h>
#include <osvr/Util/ProjectionMatrixFromFOV.h>
#include <osvr/Util/Angles.h>
#include <osvr/Util/EigenExtras.h>

// Library/third-party includes
#include "gtest/gtest.h"

// Standard includes
// - none

using osvr::util::degrees;
using osvr::util::computeSymmetricFOVRect;

typedef std::vector<double> BoundsList;
namespace opts = osvr::util::projection_options;

template <typename OptionType>
class ParameterizedProjectionTest : public ::testing::Test {
  public:
    static const opts::OptionType Options = OptionType::value;
    using Vec3 = Eigen::Vector3d;
    using Vec4 = Eigen::Vector4d;
    static double getMinZ() {
        return opts::IsZOutputUnsigned<Options>::value ? 0 : -1;
    }
    ParameterizedProjectionTest()
        : xBounds({-1, 1}), yBounds({-1, 1}), zBounds({getMinZ(), 1}) {
        std::cout
            << "\n Left handed input: " << std::boolalpha
            << osvr::util::projection_options::IsLeftHandedInput<Options>::value
            << "\n";
        std::cout
            << "Unsigned Z output: " << std::boolalpha
            << osvr::util::projection_options::IsZOutputUnsigned<Options>::value
            << "\n";
    }

    void setParams(double zNear, double zFar) {
        near = zNear;
        far = zFar;
        std::cout << "Near: " << near << "\tFar: " << far << "\n";
    }
    osvr::util::Rectd computeSymmetricRect() const {
        return computeSymmetricFOVRect(50. * degrees, 40. * degrees, near);
    }

    inline void tryProjection(osvr::util::Rectd const &rect) {

        std::cout << rect << std::endl;
        double handednessCorrection =
            osvr::util::projection_options::IsLeftHandedInput<Options>::value
                ? 1.
                : -1.;
        auto projection =
            osvr::util::parameterizedCreateProjectionMatrix<Options>(rect, near,
                                                                     far);
        std::cout << "Projection matrix:\n";
        std::cout << projection << std::endl;

        std::cout << "Frustum corners:\n";
        Eigen::Matrix4d inv = projection.inverse();
        for (auto z : zBounds) {
            for (auto y : yBounds) {
                for (auto x : xBounds) {
                    Vec4 bound(x, y, z, 1);
                    auto result =
                        osvr::util::extractPoint((inv * bound).eval());
                    std::cout << bound.transpose() << "\t<-\t" << result
                              << "\n";
                    if (z == getMinZ()) {
                        // near plane
                        ASSERT_NEAR(near * handednessCorrection, result.z(),
                                    0.0000001);
                    } else {
                        // far plane
                        ASSERT_NEAR(far * handednessCorrection, result.z(),
                                    0.0000001);
                    }
                    // F(osvr::util::extractPoint(bound), result);
                }
            }
        }
    }

    double near = 0.1;
    double far = 1000.;

  private:
    BoundsList xBounds;
    BoundsList yBounds;
    BoundsList zBounds;
};

template <opts::OptionType options = 0>
using Options = std::integral_constant<opts::OptionType, options>;

typedef ::testing::Types<
    Options<>, Options<opts::ZOutputUnsigned>, Options<opts::LeftHandedInput>,
    Options<opts::LeftHandedInput | opts::ZOutputUnsigned>> OptionCombinations;
TYPED_TEST_CASE(ParameterizedProjectionTest, OptionCombinations);
TYPED_TEST(ParameterizedProjectionTest, BasicSmoketest) {
    this->setParams(0.1, 1000);
    auto rect = this->computeSymmetricRect();
    this->tryProjection(rect);
}

TEST(ParameterizedProjectionTest, MatchesUnparameterized) {
    using namespace osvr::util;
    namespace opts = osvr::util::projection_options;
    double near = 0.1;
    double far = 100;
    auto rect = computeSymmetricFOVRect(50. * degrees, 40. * degrees, near);
    auto paramMat = parameterizedCreateProjectionMatrix<opts::RightHandedInput |
                                                        opts::ZOutputSigned>(
        rect, near, far);
    auto unparamMat = createProjectionMatrix(rect, near, far);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            ASSERT_EQ(paramMat(i, j), unparamMat(i, j));
        }
    }
}
