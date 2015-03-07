/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_RouterTransforms_h_GUID_FBF7CB86_1F82_40BC_1D08_1AB3C5178B2B
#define INCLUDED_RouterTransforms_h_GUID_FBF7CB86_1F82_40BC_1D08_1AB3C5178B2B

// Internal Includes
#include <osvr/Util/ClientCallbackTypesC.h>
#include <osvr/Util/EigenInterop.h>
#include <osvr/Transform/Transform.h>

// Library/third-party includes
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
// - none

namespace osvr {
namespace client {
    class NullTransform {
      public:
        template <typename T> void operator()(T &) {}
    };

    template <typename T1, typename T2> class CombinedTransforms {
      public:
        CombinedTransforms(T1 const &a, T2 const &b) : m_xforms(a, b) {}
        template <typename T> void operator()(T &report) {
            m_xforms.first(report);
            m_xforms.second(report);
        }

      private:
        std::pair<T1, T2> m_xforms;
    };

    template <typename T1, typename T2>
    inline CombinedTransforms<T1, T2> combineTransforms(T1 const &a,
                                                        T2 const &b) {
        return CombinedTransforms<T1, T2>(a, b);
    }

} // namespace client
} // namespace osvr
#endif // INCLUDED_RouterTransforms_h_GUID_FBF7CB86_1F82_40BC_1D08_1AB3C5178B2B
