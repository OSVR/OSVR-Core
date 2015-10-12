/** @file
    @brief "One-Euro" tracking filter analysis plugin

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

// Internal Includes
#include <osvr/AnalysisPluginKit/AnalysisPluginKitC.h>
#include <osvr/PluginKit/PluginKit.h>
#include <osvr/PluginKit/TrackerInterfaceC.h>

// Generated JSON header file
#include "org_osvr_filter_oneeuro_json.h"

// Library/third-party includes
#include <json/reader.h>
#include <json/value.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

// Standard includes
#include <iostream>
#include <cmath>

// Anonymous namespace to avoid symbol collision
namespace {

	static const auto DRIVER_NAME = "OneEuroFilter";

namespace one_euro {

    template <typename Derived>
    inline Derived computeLowPassStep(Eigen::MatrixBase<Derived> const &hatx,
                                      Eigen::MatrixBase<Derived> const &x,
                                      typename Derived::Scalar alpha) {
        return alpha * x + (1 - alpha) * hatx;
    }

    template <typename Derived>
    inline Derived
    computeLowPassStep(Eigen::QuaternionBase<Derived> const &hatx,
                       Eigen::QuaternionBase<Derived> const &x,
                       typename Derived::Scalar alpha) {
        return hatx.slerp(alpha, x).normalized();
    }

    /// Low pass filter (designed for use within the One Euro filter) that works
    /// with Eigen types.
    ///
    /// Requires that a `computeLowPassStep()` overload exist that can take the
    /// data type.
    template <typename T> class LowPassFilter {
      public:
        using value_type = T;
        using scalar = typename T::Scalar;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

        /// Feeds in new data with the specified alpha.
        /// @returns the new hatx
        value_type const &filter(Eigen::Ref<const value_type> const &x,
                                 scalar alpha) {
            if (m_firstTime) {
                m_firstTime = false;
                m_hatx = x;
                return m_hatx;
            }

            m_hatx = computeLowPassStep(hatx(), x, alpha);
            return m_hatx;
        }

        value_type const &hatx() const { return m_hatx; }

      private:
        value_type m_hatx;
        bool m_firstTime = true;
    };

    namespace detail {
        /// Computing the alpha value for a step in the one euro filter for any
        /// scalar type.
        template <typename T> T computeAlpha(T dt, T cutoff) {
            auto tau = T(1) / (T(2) * M_PI * cutoff);
            return T(1) / (T(1) + tau / dt);
        }

        /// @name One-Euro Filter required functions for Eigen::Vector3d
        /// @{
        void setDerivativeIdentity(Eigen::Vector3d &dx) {
            dx = Eigen::Vector3d::Zero();
        }
        Eigen::Vector3d computeDerivative(Eigen::Vector3d const &prev,
                                          Eigen::Vector3d const &curr,
                                          double dt) {
            return (curr - prev) / dt;
        }
        double computeDerivativeMagnitude(Eigen::Vector3d const &dx) {
            return dx.norm();
        }
        /// @}

        /// @name One-Euro Filter required functions for Eigen::Quaterniond
        /// @{
        void setDerivativeIdentity(Eigen::Quaterniond &dx) {
            dx = Eigen::Quaterniond::Identity();
        }
        Eigen::Quaterniond computeDerivative(Eigen::Quaterniond const &prev,
                                             Eigen::Quaterniond const &curr,
                                             double dt) {
            // slerp, based on dt, between the identity and our difference
            // rotation.
            return Eigen::Quaterniond::Identity()
                .slerp(dt, curr * prev.inverse())
                .normalized();
        }
        double computeDerivativeMagnitude(Eigen::Quaterniond const &quat) {
            return 2.0 * std::acos(quat.w());
        }
        /// @}
    } // namespace detail

    /// Parameters needed for the one-euro filter
    struct Params {
        double minCutoff;
        double beta;
        double derivativeCutoff;
    };
    static const Params POSITION_DEFAULTS = {1.15, 0.5, 1.2};
    static const Params ORIENTATION_DEFAULTS = {1.5, 0.5, 1.2};

    /// A simple filter designed for human input sources: high accuracy at
    /// low velocity, low latency at high velocity.
    ///
    /// @tparam T the Eigen datatype you're using
    template <typename T> class OneEuroFilter {
      public:
        using value_type = T;
        using scalar = typename T::Scalar;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
        explicit OneEuroFilter(Params const &p) : m_params(p) {}

        value_type const &filter(scalar dt,
                                 Eigen::Ref<const value_type> const &x) {
            auto dx = value_type{};
            if (m_firstTime) {
                m_firstTime = false;
                detail::setDerivativeIdentity(dx);
            } else {
                dx = detail::computeDerivative(m_xFilter.hatx(), x, dt);
            }
            // Low-pass-filter the derivative.
            m_dxFilter.filter(dx, detail::computeAlpha<scalar>(
                                      dt, m_params.derivativeCutoff));
            // Get the magnitude of the (filtered) derivative
            auto dxMag = detail::computeDerivativeMagnitude(m_dxFilter.hatx());
            // Compute the cutoff to use for the x filter
            auto cutoff = m_params.minCutoff + m_params.beta * dxMag;

            // Filter the x and return the results.
            return m_xFilter.filter(x,
                                    detail::computeAlpha<scalar>(dt, cutoff));
        }

      private:
        bool m_firstTime = true;
        const Params m_params;
        LowPassFilter<T> m_xFilter;
        LowPassFilter<T> m_dxFilter;
    };
} // namespace one_euro

class OneEuroFilterDevice {
  public:
    OneEuroFilterDevice(OSVR_PluginRegContext ctx, std::string const &name,
                        std::string const &input,
                        one_euro::Params const &posParams,
                        one_euro::Params const &oriParams)
        : m_posParams(posParams), m_oriParams(oriParams) {
        /// Create the initialization options
        OSVR_DeviceInitOptions opts = osvrDeviceCreateInitOptions(ctx);

        osvrDeviceTrackerConfigure(opts, &m_trackerOut);
        /// Create the device token with the options
        m_dev.initSync(ctx, name, opts);
        OSVR_DeviceToken dev;
        if (OSVR_RETURN_FAILURE ==
            osvrAnalysisSyncInit(ctx, name.c_str(), opts, &dev, &m_clientCtx)) {
            throw std::runtime_error("Could not initialize analysis plugin!");
        }
        m_dev = osvr::pluginkit::DeviceToken(dev);

        /// Send JSON descriptor
        m_dev.sendJsonDescriptor(org_osvr_filter_oneeuro_json);

        /// Register update callback
        m_dev.registerUpdateCallback(this);
    }

    OSVR_ReturnCode update() {
        // Nothing to do here - everything happens in a callback.
        return OSVR_RETURN_SUCCESS;
    }

  private:
    osvr::pluginkit::DeviceToken m_dev;
    OSVR_ClientContext m_clientCtx;
    OSVR_TrackerDeviceInterface m_trackerOut;
    const one_euro::Params m_posParams;
    const one_euro::Params m_oriParams;
};

class AnalysisPluginInstantiation {
  public:
    AnalysisPluginInstantiation() {}
    OSVR_ReturnCode operator()(OSVR_PluginRegContext ctx, const char *params) {
        Json::Value root;
        {
            Json::Reader reader;
            if (!reader.parse(params, root)) {
                std::cerr << "Couldn't parse JSON for one euro filter!"
                          << std::endl;
                return OSVR_RETURN_FAILURE;
            }
        }

        // Get filter parameters for position
        auto posParams = one_euro::Params(one_euro::POSITION_DEFAULTS);
        if (root.isMember("position")) {
            parseOneEuroParams(posParams, root["position"]);
        }

        // Get filter parameters for orientation
        auto oriParams = one_euro::Params(one_euro::ORIENTATION_DEFAULTS);
        if (root.isMember("orientation")) {
            parseOneEuroParams(oriParams, root["orientation"]);
        }

        auto input = root["input"].asString();
        auto deviceName = root.get("name", DRIVER_NAME).asString();

        return OSVR_RETURN_SUCCESS;
    }

    static void parseOneEuroParams(one_euro::Params &p,
                                   Json::Value const &json) {
        // In all cases, using the existing value as default value.
        p.minCutoff = json.get("minCutoff", p.minCutoff).asDouble();
        p.beta = json.get("beta", p.beta).asDouble();
        p.derivativeCutoff =
            json.get("derivativeCutoff", p.derivativeCutoff).asDouble();
    }
};
} // namespace

OSVR_PLUGIN(org_osvr_filter_oneeuro) {
    osvr::pluginkit::PluginContext context(ctx);

    /// Register a detection callback function object.
    context.registerDriverInstantiationCallback(DRIVER_NAME,
        new AnalysisPluginInstantiation());

    return OSVR_RETURN_SUCCESS;
}
