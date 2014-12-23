/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_JSONTransformVisitor_h_GUID_35872365_9953_479F_C548_86F8BDAE030B
#define INCLUDED_JSONTransformVisitor_h_GUID_35872365_9953_479F_C548_86F8BDAE030B

// Internal Includes
#include <osvr/Transform/Export.h>
#include <osvr/Util/UniquePtr.h>
#include <osvr/Transform/Transform.h>

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <json/value.h>

// Standard includes
#include <string>

namespace osvr {
namespace transform {
    class JSONTransformData;
    class JSONTransformVisitor {
      public:
        /// @brief Parses json describing a transformation to compute the
        /// overall transformation.
        OSVR_TRANSFORM_EXPORT JSONTransformVisitor(Json::Value const &root);
        OSVR_TRANSFORM_EXPORT ~JSONTransformVisitor();

        OSVR_TRANSFORM_EXPORT Transform const &getTransform() const;

        OSVR_TRANSFORM_EXPORT Json::Value const &getLeaf() const;

      private:
        Transform m_transform;
        Json::Value m_leaf;
    };
} // namespace transform
} // namespace osvr

#endif // INCLUDED_JSONTransformVisitor_h_GUID_35872365_9953_479F_C548_86F8BDAE030B
