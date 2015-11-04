/** @file
    @brief Header

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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_JSONEigen_h_GUID_A8A98FBE_0A98_4C83_9A13_D022330FE13E
#define INCLUDED_JSONEigen_h_GUID_A8A98FBE_0A98_4C83_9A13_D022330FE13E

// Internal Includes
#include <osvr/Util/EigenCoreGeometry.h>

// Library/third-party includes
#include <json/value.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/range/algorithm/count_if.hpp>
#include <boost/assert.hpp>

// Standard includes
#include <stdexcept>
#include <cstring>
#include <string>

namespace osvr {
namespace common {
    /// @brief Converts quaternions to JSON objects.
    template <typename Derived>
    inline Json::Value toJson(Eigen::QuaternionBase<Derived> const &quat) {
        Json::Value ret(Json::objectValue);
        ret["w"] = quat.w();
        ret["x"] = quat.x();
        ret["y"] = quat.y();
        ret["z"] = quat.z();
        return ret;
    }

    /// @brief Converts vectors to JSON arrays.
    template <typename Derived>
    inline Json::Value toJson(Eigen::MatrixBase<Derived> const &vec) {
        static_assert(Derived::ColsAtCompileTime == 1,
                      "This function is only for converting vectors to JSON");
        Json::Value ret(Json::arrayValue);
        for (size_t i = 0; i < Derived::RowsAtCompileTime; ++i) {
            ret.append(vec[i]);
        }
        return ret;
    }

    namespace detail {
        static const char AXIS_NAMES[] = "XYZ";
        static const char MINUS[] = "-";
        template <typename VecType>
        inline VecType stringToVector(std::string const &s) {
            typedef typename VecType::Scalar Scalar;

            // Return value for NRVO
            VecType ret = VecType::Zero();
            if (s.empty()) {
                throw std::runtime_error(
                    "Can't parse a vector from an empty string!");
            }

            // Will work with uppercase string from here on out.
            std::string upperS(boost::to_upper_copy(s));
            {
                // Input validity checks.
                using boost::is_any_of;
                using boost::algorithm::is_space;
                using boost::count_if;
                if ((!boost::algorithm::all(upperS, is_any_of(AXIS_NAMES) ||
                                                        is_any_of(MINUS) ||
                                                        is_space())) ||
                    count_if(upperS, is_any_of(AXIS_NAMES)) != 1 ||
                    count_if(upperS, is_any_of(MINUS)) > 1) {
                    throw std::runtime_error(
                        "Cannot turn the specified string into a vector: " + s);
                }
            }
            Scalar factor =
                (upperS.find(MINUS[0]) == std::string::npos) ? 1.0 : -1.0;
            for (const char c : upperS) {
                auto axisPtr = std::strchr(AXIS_NAMES, c);
                if (nullptr != axisPtr) {
                    auto index = axisPtr - AXIS_NAMES;
                    if (index >= VecType::RowsAtCompileTime) {
                        throw std::runtime_error(
                            "A vector of this size cannot be initialized to " +
                            s);
                    }
                    ret[index] = factor;
                    return ret;
                }
            }

            BOOST_ASSERT_MSG(false, "Should never reach here!");
        }
    } // namespace detail

#define OSVR_EXTRACT_JSON_MEMBER(_DATATYPENAME, _LOWER, _UPPER)                \
    if (json.isMember(#_LOWER)) {                                              \
        ret._LOWER() = json[#_LOWER].asDouble();                               \
    } else if (json.isMember(#_UPPER)) {                                       \
        ret._LOWER() = json[#_UPPER].asDouble();                               \
    } else {                                                                   \
        throw std::runtime_error("In parsing " _DATATYPENAME                   \
                                 " from JSON, no " #_LOWER " element found!"); \
    }

    Eigen::Quaterniond quatFromJson(Json::Value const &json) {
        Eigen::Quaterniond ret;
        if (json.isObject()) {
            OSVR_EXTRACT_JSON_MEMBER("quaternion", w, W);
            OSVR_EXTRACT_JSON_MEMBER("quaternion", x, X);
            OSVR_EXTRACT_JSON_MEMBER("quaternion", y, Y);
            OSVR_EXTRACT_JSON_MEMBER("quaternion", z, Z);
        } else if (json.isArray()) {
            // Array - ugh, must assume an order, guess we'll assume w, x, y, z
            ret = Eigen::Quaterniond(json[0].asDouble(), json[1].asDouble(),
                                     json[2].asDouble(), json[3].asDouble());
        } else {
            throw std::runtime_error("Could not parse quaternion from JSON - "
                                     "neither an object nor an array");
        }
        return ret;
    }

    inline Eigen::Vector3d vec3FromJson(Json::Value const &json) {
        typedef Eigen::Vector3d type;
#define OSVR_PARSE_NAME "Vector3d"
        type ret;
        if (json.isString()) {
            // Handle just straight axis names
            if ((json == "x") || (json == "X")) {
                ret = type::UnitX();
            } else if ((json == "y") || (json == "Y")) {
                ret = type::UnitY();
            } else if ((json == "z") || (json == "Z")) {
                ret = type::UnitZ();
            } else {
                throw std::runtime_error(
                    "Could not parse a " OSVR_PARSE_NAME
                    " from JSON string: looking for an axis name, got " +
                    json.asString());
            }
        } else if (json.isNumeric()) {
            if (json == 0) {
                ret = type::Zero();
            } else {
                throw std::runtime_error(
                    "Could not parse a " OSVR_PARSE_NAME
                    " from the scalar value " +
                    boost::lexical_cast<std::string>(json.asDouble()));
            }
        } else if (json.isArray()) {
            if (json.size() != type::RowsAtCompileTime) {
                throw std::runtime_error(
                    "Could not parse a " OSVR_PARSE_NAME
                    " from an array with " +
                    boost::lexical_cast<std::string>(json.size()) +
                    " elements");
            }
            ret = type(json[0].asDouble(), json[1].asDouble(),
                       json[2].asDouble());
        } else if (json.isObject()) {
            OSVR_EXTRACT_JSON_MEMBER(OSVR_PARSE_NAME, x, X);
            OSVR_EXTRACT_JSON_MEMBER(OSVR_PARSE_NAME, y, Y);
            OSVR_EXTRACT_JSON_MEMBER(OSVR_PARSE_NAME, z, Z);
        } else {
            throw std::runtime_error(
                "Could not parse " OSVR_PARSE_NAME " from JSON - "
                "not a component name, 0, an array, nor an object");
        }
        return ret;
#undef OSVR_PARSE_NAME
    }

#undef OSVR_EXTRACT_JSON_MEMBER
} // namespace common
} // namespace osvr

#endif // INCLUDED_JSONEigen_h_GUID_A8A98FBE_0A98_4C83_9A13_D022330FE13E
