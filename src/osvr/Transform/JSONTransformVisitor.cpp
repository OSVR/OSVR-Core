/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Transform/JSONTransformVisitor.h>
#include <osvr/Transform/Transform.h>
#include <osvr/Transform/ChangeOfBasis.h>
#include <osvr/Transform/Util.h>
#include <osvr/Util/StdInt.h>

// Library/third-party includes
#include <json/value.h>
#include <json/reader.h>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/count_if.hpp>

// Standard includes
#include <string>

namespace osvr {
namespace transform {

    static const char AXIS_NAMES[] = "XYZ";
    static const char MINUS[] = "-";
    template <typename T = Eigen::Vector3d>
    inline T vectorFromJson(Json::Value const &v) {
        T ret = T::Zero();
        if (v.isString()) {
            std::string inVal = v.asString();
            if (inVal.empty()) {
                throw std::runtime_error(
                    "Empty string can't be turned into a vector!");
            }
            using boost::is_any_of;
            std::string val(boost::to_upper_copy(inVal));
            if ((!boost::algorithm::all(
                     val, is_any_of(AXIS_NAMES) || is_any_of(MINUS) ||
                              boost::algorithm::is_space())) ||
                boost::count_if(val, is_any_of(AXIS_NAMES)) != 1 ||
                boost::count_if(val, is_any_of(MINUS)) > 1) {
                throw std::runtime_error(
                    "Cannot turn the specified string into a vector: " + inVal);
            }
            double factor =
                (val.find(MINUS[0]) == std::string::npos) ? 1.0 : -1.0;
            const std::string axisnames(AXIS_NAMES);
            for (const char c : val) {
                auto location = axisnames.find(c);
                if (location != std::string::npos) {
                    ret[location] = factor;
                    return ret;
                }
            }
            BOOST_ASSERT_MSG(false, "Should never reach here!");
        }
        if (v.isArray()) {
            if (v.size() != T::RowsAtCompileTime) {
                throw std::runtime_error(
                    "Vector size wrong when converting from JSON!");
            }
            for (Json::ArrayIndex i = 0, e = v.size(); i < e; ++i) {
                ret[i] = v[i].asFloat();
            }
            return ret;
        }
        throw std::runtime_error("Could not convert JSON to vector: " +
                                 v.toStyledString());
    }

    static const char DEGREES_KEY[] = "degrees";
    static const char RADIANS_KEY[] = "radians";
    static inline double angleAsRadians(Json::Value const &rotation) {
        double ret = 0;
        if (rotation[DEGREES_KEY].isNumeric()) {
            ret = degreesToRadians(rotation[DEGREES_KEY].asFloat());
        } else if (rotation[RADIANS_KEY].isNumeric()) {
            ret = rotation[RADIANS_KEY].asFloat();
        } else {
            throw std::runtime_error(
                "Cannot have a rotation with either degrees or radians!");
        }
        return ret;
    }

    static const char TRANSLATE_KEY[] = "posttranslate";
    static const char PRETRANSLATE_KEY[] = "translate";
    static const char ROTATE_KEY[] = "postrotate";
    static const char PREROTATE_KEY[] = "rotate";
    static const char AXIS_KEY[] = "axis";
    static const char CHANGE_BASIS_KEY[] = "changeBasis";
    static const char X_KEY[] = "x";
    static const char Y_KEY[] = "y";
    static const char Z_KEY[] = "z";
    static inline void handleLevel(Transform &t, Json::Value const &v) {

        if (v.isMember(CHANGE_BASIS_KEY)) {
            Json::Value changeBasis = v[CHANGE_BASIS_KEY];
            ChangeOfBasis cb;
            cb.setNewX(vectorFromJson<>(changeBasis[X_KEY]));
            cb.setNewY(vectorFromJson<>(changeBasis[Y_KEY]));
            cb.setNewZ(vectorFromJson<>(changeBasis[Z_KEY]));
            t.transform(cb.get());
            return;
        }
        Eigen::Vector3d position = Eigen::Vector3d::Zero();
        Eigen::AngleAxisd orientation = Eigen::AngleAxisd::Identity();
        Eigen::Vector3d preposition = Eigen::Vector3d::Zero();
        Eigen::AngleAxisd preorientation = Eigen::AngleAxisd::Identity();
        bool haveTransform = false;
        bool havePreTransform = false;
        if (v.isMember(TRANSLATE_KEY)) {
            position = vectorFromJson<>(v[TRANSLATE_KEY]);
            haveTransform = true;
        }
        if (v.isMember(PRETRANSLATE_KEY)) {
            preposition = vectorFromJson<>(v[PRETRANSLATE_KEY]);
            havePreTransform = true;
        }
        if (v.isMember(ROTATE_KEY)) {
            Json::Value rotate = v[ROTATE_KEY];
            orientation = Eigen::AngleAxisd(angleAsRadians(rotate),
                                            vectorFromJson<>(rotate[AXIS_KEY]));
            haveTransform = true;
        }
        if (v.isMember(PREROTATE_KEY)) {
            Json::Value rotate = v[PREROTATE_KEY];
            preorientation = Eigen::AngleAxisd(
                angleAsRadians(rotate), vectorFromJson<>(rotate[AXIS_KEY]));
            havePreTransform = true;
        }
        if (haveTransform) {
            Eigen::Affine3d xform;
            xform.fromPositionOrientationScale(position, orientation,
                                               Eigen::Vector3d::Constant(1));
            t.concatPost(xform.matrix());
        }
        if (havePreTransform) {
            Eigen::Affine3d xform;
            xform.fromPositionOrientationScale(preposition, preorientation,
                                               Eigen::Vector3d::Constant(1));
            t.concatPre(xform.matrix());
        }
    }

    static const char CHILD_KEY[] = "child";
    JSONTransformVisitor::JSONTransformVisitor(Json::Value const &root) {
        std::vector<Json::Value> levels;
        Json::Value current = root;
        levels.push_back(current);
        while (current[CHILD_KEY].isObject()) {
            current = current[CHILD_KEY];
            levels.push_back(current);
        }
        m_leaf = current;
        while (!levels.empty()) {
            handleLevel(m_transform, levels.back());
            levels.pop_back();
        }
    }

    JSONTransformVisitor::~JSONTransformVisitor() {}

    Transform const &JSONTransformVisitor::getTransform() const {
        return m_transform;
    }

    Json::Value const &JSONTransformVisitor::getLeaf() const { return m_leaf; }

} // namespace transform
} // namespace osvr