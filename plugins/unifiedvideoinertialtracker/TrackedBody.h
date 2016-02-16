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

#ifndef INCLUDED_TrackedBody_h_GUID_1FC60169_196A_4F89_551C_E1B41531BBC8
#define INCLUDED_TrackedBody_h_GUID_1FC60169_196A_4F89_551C_E1B41531BBC8

// Internal Includes
#include "ConfigParams.h"
#include "BodyIdTypes.h"
#include "ModelTypes.h"
#include "CannedIMUMeasurement.h"

// Library/third-party includes
#include <osvr/Util/EigenCoreGeometry.h>
#include <osvr/Util/TimeValue.h>
#include <boost/assert.hpp>

// Standard includes
#include <memory>

namespace osvr {
namespace vbtracker {
    class TrackingSystem;
    class TrackedBodyIMU;
    class TrackedBodyTarget;
    struct TargetSetupData;

    /// This is the class representing a tracked rigid body in the system. It
    /// may be tracked by one (or eventually more) video-based "target"
    /// (constellation of beacons in a known pattern with other known traits),
    /// and optionally by an IMU/AHRS - an orientation/angular-velocity-only
    /// high speed sensor.
    ///
    /// This class has overall state
    class TrackedBody {
      public:
        /// Constructor
        TrackedBody(TrackingSystem &system, BodyId id);
        /// Destructor - explicit so we can use unique_ptr for our pimpls.
        ~TrackedBody();
        /// Noncopyable
        TrackedBody(TrackedBody const &) = delete;
        /// Non-copy-assignable
        TrackedBody &operator=(TrackedBody const &) = delete;
        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        /// @name Child factories
        /// @{
        /// Creates a fully-integrated IMU data source (that is, one that
        /// reports a reliable quaternion, and potentially angular velocity) to
        /// add to this body.
        ///
        /// @todo eventually fix: Right now assumes that there is only one IMU
        /// per body
        /// @todo assumes the IMU is at the origin of the body (most important
        /// for velocity)
        ///
        /// You do not own the pointer you get back - the tracked body does.
        ///
        /// @return nullptr if an error occurred (such as an IMU already being
        /// added to this body)
        TrackedBodyIMU *
        createIntegratedIMU(double orientationVariance,
                            double angularVelocityVariance = 1.0);

        /// Creates a video-based tracking target (constellation of beacons) to
        /// add to this body.
        ///
        /// @todo eventually fix: Right now assumes that there is only one
        /// target per body
        ///
        /// You do not own the pointer you get back - the tracked body does.
        ///
        /// @return nullptr if an error occurred (such as a target already being
        /// added to this body)
        TrackedBodyTarget *createTarget(Eigen::Vector3d const &targetToBody,
                                        TargetSetupData const &setupData);
        /// @}

        /// Gets the body ID within the tracking system.
        BodyId getId() const;

        /// @todo refactor
        ConfigParams const &getParams() const;

        BodyState const &getState() const { return m_state; }

        /// Does this tracked body have an IMU?
        bool hasIMU() const { return static_cast<bool>(m_imu); }

        /// Get the IMU - only valid if hasIMU is true.
        TrackedBodyIMU &getIMU() {
            BOOST_ASSERT_MSG(m_imu, "getIMU() called when hasIMU() is false!");
            return *m_imu;
        }

        /// Get the IMU - only valid if hasIMU is true.
        TrackedBodyIMU const &getIMU() const {
            BOOST_ASSERT_MSG(m_imu, "getIMU() called when hasIMU() is false!");
            return *m_imu;
        }

        /// How many (if any) video-based tracking targets does this tracked
        /// body have?
        std::size_t getNumTargets() const { return m_target ? 1 : 0; }

        TrackedBodyTarget *getTarget(TargetId id) {
            if (TargetId(0) == id) {
                return m_target.get();
            }
            return nullptr;
        }

        TrackedBodyTarget const *getTarget(TargetId id) const {
            if (TargetId(0) == id) {
                return m_target.get();
            }
            return nullptr;
        }

        template <typename F> void forEachTarget(F &&f) {
            if (m_target) {
                std::forward<F>(f)(*m_target);
            }
        }

        template <typename F> void forEachTarget(F &&f) const {
            if (m_target) {
                std::forward<F>(f)(*m_target);
            }
        }

        /// Do we have a pose estimate for this body in general?
        bool hasPoseEstimate() const;

        /// Requests a copy of (a possibly historical snapshot of) body state,
        /// as close to desiredTime without being later than it. If no such
        /// state is available (primarily because no valid state has been set
        /// yet), false will be returned and the out parameters will be
        /// untouched.
        ///
        /// The purpose is to provide state that is potentially in the past, to
        /// integrate delayed measurements (like from video tracking) then later
        /// incorporate that state and replay later low-latency measurements.
        ///
        /// @todo should this block others from trying to submit updates for
        /// times between outTime and desiredTime?
        ///
        /// @param[in] desiredTime The time you'd like state to be at or before
        /// - typically your measurement time.
        /// @param[out] outTime The timestamp for the state copy you receive -
        /// modified only if return value is true. You'll need to retain this to
        /// re-submit with your updated state.
        /// @param[out] outState The copy of the state will be placed here -
        /// modified only if return value is true.
        ///
        /// @return true if a state for the body has been recorded at or prior
        /// to desiredTime and has thus been returned in outTime and outState.
        bool getStateAtOrBefore(osvr::util::time::TimeValue const &desiredTime,
                                osvr::util::time::TimeValue &outTime,
                                BodyState &outState);

        /// This is the counterpart to getStateAtOrBefore() and should only be
        /// called subsequent to it. You provide the timestamp that you
        /// originally got with the state from getStateAtOrBefore(), then the
        /// updated timestamp and state you got from incorporating your
        /// measurement.
        ///
        /// In the history of this body, the old state will effectively be
        /// replaced (or immediately followed, implementation detail) by this
        /// one: classes of newer measurements will be replayed on the state as
        /// required to update the current body state to properly incorporate
        /// the presumably-dated information you just provided.
        ///
        /// @param origTime the timestamp originally received from
        /// getStateAtOrBefore() as `outTime`
        /// @param newTime the timestamp currently associated with the state
        /// @param newState the updated state.
        void replaceStateSnapshot(osvr::util::time::TimeValue const &origTime,
                                  osvr::util::time::TimeValue const &newTime,
                                  BodyState const &newState);

        /// Clean histories of no-longer-needed historical state and
        /// measurements.
        void pruneHistory();

        /// Get timestamp associated with current state.
        osvr::util::time::TimeValue getStateTime() const;

        BodyProcessModel &getProcessModel() { return m_processModel; }

        /// @todo Note that this is stored in camera space!
        BodyState &getState() { return m_state; }

        /// Incorporates a brand-new measurement from the IMU into the state.
        /// Called only from the TrackedBodyIMU itself, please!
        void incorporateNewMeasurementFromIMU(util::time::TimeValue const &tv,
                                              CannedIMUMeasurement const &meas);

        TrackingSystem &getSystem() { return m_system; }
        TrackingSystem const &getSystem() const { return m_system; }

      private:
        /// Method used both when incorporating new measurements and replaying
        /// historical measurements: pushes to state history but not to IMU
        /// history.
        void applyIMUMeasurement(util::time::TimeValue const &tv,
                                 CannedIMUMeasurement const &meas);
        /// Pushes current state on to history: assumes you've already updated
        /// m_state and the stateTime.
        void pushState();
        TrackingSystem &m_system;
        const BodyId m_id;

        util::time::TimeValue m_stateTime;
        BodyState m_state;
        BodyProcessModel m_processModel;
        /// private implementation data
        struct Impl;
        std::unique_ptr<Impl> m_impl;
        std::unique_ptr<TrackedBodyIMU> m_imu;
        std::unique_ptr<TrackedBodyTarget> m_target;
    };
} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_TrackedBody_h_GUID_1FC60169_196A_4F89_551C_E1B41531BBC8
