/** @file
    @brief Implementation

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
// - none

// Standard includes
#include <iostream>
#include <vector>
#include <cmath>

using osvr::util::degrees;

int main(int argc, char *argv[]) {
    typedef std::vector<double> BoundsList;
    BoundsList xBounds;
    xBounds.push_back(-1);
    xBounds.push_back(1);

    BoundsList yBounds(xBounds);
    BoundsList zBounds(xBounds);

    double near = 0.1;
    double far = 1000;
    auto rect = osvr::util::detail::computeRect(50. * degrees, 40. * degrees, near);
    std::cout << "Near: " << near << "\tFar: " << far << "\n";
    std::cout << rect << std::endl;
    auto projection = osvr::util::createProjectionMatrix(rect, near, far);
    std::cout << "Projection matrix:\n";
    std::cout << projection << std::endl;

    std::cout << "Frustum corners:\n";
    Eigen::Matrix4d inv = projection.inverse();
    for (auto z : zBounds) {
        for (auto y : yBounds) {
            for (auto x : xBounds) {
                Eigen::Vector4d bound(x, y, z, 1);
                std::cout << bound.transpose() << "\t<-\t"
                          << osvr::util::extractPoint(inv * bound).transpose() << "\n";
            }
        }
    }
    return 0;
}
