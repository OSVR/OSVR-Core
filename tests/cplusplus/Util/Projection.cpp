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
#include <osvr/Util/Angles.h>
#include <osvr/Util/EigenExtras.h>
#include <osvr/Util/ProjectionMatrix.h>
#include <osvr/Util/ProjectionMatrixFromFOV.h>

// Library/third-party includes
#include <catch2/catch.hpp>

// Standard includes
#include <iostream>

using osvr::util::computeSymmetricFOVRect;
using osvr::util::degrees;

typedef std::vector<double> BoundsList;
namespace opts = osvr::util::projection_options;

template <opts::OptionType options = 0>
using Options = std::integral_constant<opts::OptionType, options>;
template <opts::OptionType Options> inline double getMinZ() {
    return opts::IsZOutputUnsigned<Options>::value ? 0 : -1;
}

TEMPLATE_TEST_CASE("ParameterizedProjectionTest-BasicSmoketest", "", Options<>,
                   Options<opts::ZOutputUnsigned>,
                   Options<opts::LeftHandedInput>,
                   Options<opts::LeftHandedInput | opts::ZOutputUnsigned>) {

    static const opts::OptionType Options = TestType::value;

    using Vec3 = Eigen::Vector3d;
    using Vec4 = Eigen::Vector4d;
    const auto minZ = getMinZ<Options>();

    INFO("Left handed input: "
         << std::boolalpha
         << osvr::util::projection_options::IsLeftHandedInput<Options>::value);
    INFO("Unsigned Z output: "
         << std::boolalpha
         << osvr::util::projection_options::IsZOutputUnsigned<Options>::value);
    BoundsList xBounds{{-1, 1}};
    BoundsList yBounds{{-1, 1}};
    BoundsList zBounds{{minZ, 1}};
    double near = 0.1;
    double far = 1000.;
    CAPTURE(near);
    CAPTURE(far);
    auto rect = computeSymmetricFOVRect(50. * degrees, 40. * degrees, near);

    std::ostringstream os;
    os << rect;
    INFO(os.str());

    double handednessCorrection =
        osvr::util::projection_options::IsLeftHandedInput<Options>::value ? 1.
                                                                          : -1.;
    auto projection = osvr::util::parameterizedCreateProjectionMatrix<Options>(
        rect, near, far);

    CAPTURE(projection);

    INFO("Frustum corners:");
    Eigen::Matrix4d inv = projection.inverse();
    for (auto z : zBounds) {
        for (auto y : yBounds) {
            for (auto x : xBounds) {
                Vec4 bound(x, y, z, 1);
                auto result = osvr::util::extractPoint((inv * bound).eval());
                INFO(bound.transpose() << "\t<-\t" << result);
                if (z == minZ) {
                    // near plane
                    REQUIRE(near * handednessCorrection == Approx(result.z()));
                } else {
                    // far plane
                    REQUIRE(far * handednessCorrection == Approx(result.z()));
                }
                // F(osvr::util::extractPoint(bound), result);
            }
        }
    }
}

TEST_CASE("ParameterizedProjectionTest-MatchesUnparameterized") {
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
            REQUIRE(paramMat(i, j) == unparamMat(i, j));
        }
    }
}
