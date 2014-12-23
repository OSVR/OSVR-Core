/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

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
