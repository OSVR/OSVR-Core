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

#ifndef INCLUDED_PoseFilter_h_GUID_B1DDEE93_412D_4EA3_44B5_C1853F6F0FAA
#define INCLUDED_PoseFilter_h_GUID_B1DDEE93_412D_4EA3_44B5_C1853F6F0FAA

// Internal Includes
#include <osvr/Util/EigenFilters.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace vbtracker {

    class PoseFilter {
      public:
        PoseFilter(util::filters::one_euro::Params const &positionFilterParams =
                       util::filters::one_euro::Params{},
                   util::filters::one_euro::Params const &oriFilterParams =
                       util::filters::one_euro::Params{})
            : m_positionFilter(positionFilterParams),
              m_orientationFilter(oriFilterParams){};

        void filter(double dt, Eigen::Vector3d const &position,
                    Eigen::Quaterniond const &orientation) {
            if (dt <= 0) {
                /// Avoid div by 0
                dt = 1;
            }
            m_positionFilter.filter(dt, position);
            m_orientationFilter.filter(dt, orientation);
        }

        Eigen::Vector3d const &getPosition() const {
            return m_positionFilter.getState();
        }

        Eigen::Quaterniond const &getOrientation() const {
            return m_orientationFilter.getState();
        }

        Eigen::Isometry3d getIsometry() const {
            return Eigen::Translation3d(getPosition()) *
                   Eigen::Isometry3d(getOrientation());
        }

      private:
        util::filters::OneEuroFilter<Eigen::Vector3d> m_positionFilter;
        util::filters::OneEuroFilter<Eigen::Quaterniond> m_orientationFilter;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_PoseFilter_h_GUID_B1DDEE93_412D_4EA3_44B5_C1853F6F0FAA
