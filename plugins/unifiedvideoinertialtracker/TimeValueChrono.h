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

#ifndef INCLUDED_TimeValueChrono_h_GUID_62662C09_CE8E_4394_87A9_26D2FC0D2920
#define INCLUDED_TimeValueChrono_h_GUID_62662C09_CE8E_4394_87A9_26D2FC0D2920

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/TimeValue.h>

// Standard includes
#include <chrono>
#include <cstdint>

namespace osvr {
namespace util {
    namespace time {
#ifdef OSVR_DUMMY_DEFINE //_WIN32
        /// The real resolution of the clock: 100 nanoseconds, assumes the
        /// default
        /// behavior on MSVC 2013, #define VRPN_WINDOWS_CLOCK_V2
        using time_value_period =
            std::ratio_multiply<std::ratio<100>, std::nano>::type;
#else
        /// Assumes the max representable resolution otherwise
        using time_value_period = std::micro;
#endif

        using time_value_scalar_rep = std::uint64_t;
        using time_value_scalar_duration =
            std::chrono::duration<time_value_scalar_rep, time_value_period>;
    } // namespace time
} // namespace util
} // namespace osvr

namespace std {
namespace chrono {
    /// @todo if needed to inject things into std::chrono
} // namespace chrono
} // namespace std

namespace osvr {
namespace util {
    namespace time {
        class TimeValueClock {
          public:
            /// Custom rep, which will undoubtedly cause issues that we'll
            /// manually resolve later.
            using rep = osvr::util::time::TimeValue;
            using period = time_value_period;
            using duration = std::chrono::duration<rep, period>;
            using clock = TimeValueClock;
            using time_point = std::chrono::time_point<clock>;

            /// @todo is this true?
            static const bool is_steady = true;

            static time_point now() {
                return time_point(duration{osvr::util::time::getNow()});
            }

            /// When we want a short-ish duration (time difference), we can go
            /// to a
            /// scalar instead of the structure - that's what this is for.
            using scalar_duration = time_value_scalar_duration;
        };

        /// Get the TimeValue inside of a TimeValueClock time_point.
        inline osvr::util::time::TimeValue
        getTimeValue(TimeValueClock::time_point const &tp) {
            return tp.time_since_epoch().count();
        }

        /// Duration representation of the first element in the structure.
        using time_value_seconds_duration =
            std::chrono::duration<OSVR_TimeValue_Seconds>;
        /// Duration representation of the second element of the structure.
        using time_value_microseconds_duration =
            std::chrono::duration<OSVR_TimeValue_Microseconds, std::micro>;
        namespace detail {
            /// CRTP base class for things that wrap a OSVR_TimeValue for the
            /// purposes of some interop with std::chrono.
            template <typename Derived> class TimeValueChronoWrapperBase {
              public:
                using derived_type = Derived;

                OSVR_TimeValue_Seconds getSecondsPart() const {
                    return get().seconds;
                }
                time_value_seconds_duration getSecondsPartAsDuration() const {
                    return time_value_seconds_duration{getSecondsPart()};
                }
                OSVR_TimeValue_Microseconds getMicrosecondsPart() const {
                    return get().microseconds;
                }

                time_value_microseconds_duration
                getMicrosecondsPartAsDuration() const {
                    return time_value_microseconds_duration{
                        getMicrosecondsPart()};
                }

                time_value_scalar_duration toScalarDuration() const {
                    using std::chrono::duration_cast;
                    return duration_cast<time_value_scalar_duration>(
                               getSecondsPartAsDuration()) +
                           duration_cast<time_value_scalar_duration>(
                               getMicrosecondsPartAsDuration());
                }

                derived_type const &derived() const {
                    return *static_cast<derived_type const *>(this);
                }
                util::time::TimeValue const &get() const {
                    return derived().get();
                }
            };
        } // namespace detail

        /// Copy and store a TimeValue in a wrapper for std::chrono interop.
        class TimeValueChronoValueWrapper
            : public detail::TimeValueChronoWrapperBase<
                  TimeValueChronoValueWrapper> {
          public:
            explicit TimeValueChronoValueWrapper(
                util::time::TimeValue const &tv)
                : m_timeval(tv){};
            util::time::TimeValue const &get() const { return m_timeval; }

          private:
            util::time::TimeValue m_timeval;
        };

        /// Store a reference to const to a TimeValue for std::chrono interop.
        class TimeValueChronoConstRefWrapper
            : public detail::TimeValueChronoWrapperBase<
                  TimeValueChronoConstRefWrapper> {
          public:
            explicit TimeValueChronoConstRefWrapper(
                util::time::TimeValue const &tv)
                : m_timeval(tv) {}
            /// movable
            TimeValueChronoConstRefWrapper(
                TimeValueChronoConstRefWrapper &&other)
                : m_timeval(other.m_timeval) {}
            /// noncopyable
            TimeValueChronoConstRefWrapper(
                TimeValueChronoConstRefWrapper const &) = delete;
            /// nonassignable
            TimeValueChronoConstRefWrapper &
            operator=(TimeValueChronoConstRefWrapper const &) = delete;
            /// Required accessor
            util::time::TimeValue const &get() const { return m_timeval; }

          private:
            util::time::TimeValue const &m_timeval;
        };

        namespace detail {
            inline TimeValueChronoConstRefWrapper
            wrap_time_value(util::time::TimeValue const &tv) {
                return TimeValueChronoConstRefWrapper{tv};
            }
            inline TimeValueChronoConstRefWrapper
            wrap_time_value(TimeValueClock::time_point const &tp) {
                return TimeValueChronoConstRefWrapper{
                    tp.time_since_epoch().count()};
            }
        } // namespace detail

        /// Subtract one wrapped TimeValue from another and get a std::duration
        /// back!
        ///
        /// This is the main implementation: all overloads forward to this,
        /// generally via the wrap_time_value() overloaded functions that use
        /// theTimeValueChronoConstRefWrapper.
        template <typename DerivedA, typename DerivedB>
        inline time_value_scalar_duration
        operator-(detail::TimeValueChronoWrapperBase<DerivedA> const &a,
                  detail::TimeValueChronoWrapperBase<DerivedB> const &b) {
            using namespace std::chrono;
            auto diffSeconds = time_value_seconds_duration{a.getSecondsPart() -
                                                           b.getSecondsPart()};
            auto diffUsec = time_value_microseconds_duration{
                a.getMicrosecondsPart() - b.getMicrosecondsPart()};
            return duration_cast<time_value_scalar_duration>(diffSeconds) +
                   duration_cast<time_value_scalar_duration>(diffUsec);
        }

        /// subtraction: wrapped value and timevalue
        template <typename Derived>
        inline time_value_scalar_duration
        operator-(detail::TimeValueChronoWrapperBase<Derived> const &a,
                  util::time::TimeValue const &b) {
            return a - detail::wrap_time_value(b);
        }
        /// subtraction: timevalue and wrapped value
        template <typename Derived>
        inline time_value_scalar_duration
        operator-(util::time::TimeValue const &a,
                  detail::TimeValueChronoWrapperBase<Derived> const &b) {
            return detail::wrap_time_value(a) - b;
        }

        /// Overloaded subtraction operator that returns a simple scalar
        /// duration.
        /// time-point and time-point
        inline time_value_scalar_duration
        operator-(TimeValueClock::time_point const &a,
                  TimeValueClock::time_point const &b) {
            return detail::wrap_time_value(a) - detail::wrap_time_value(b);
        }

        /// subtraction: time-point and wrapped value
        template <typename Derived>
        inline time_value_scalar_duration
        operator-(TimeValueClock::time_point const &a,
                  detail::TimeValueChronoWrapperBase<Derived> const &b) {
            return detail::wrap_time_value(a) - b;
        }

        /// subtraction: wrapped value and time-point
        template <typename Derived>
        inline time_value_scalar_duration
        operator-(detail::TimeValueChronoWrapperBase<Derived> const &a,
                  TimeValueClock::time_point const &b) {
            return a - detail::wrap_time_value(b);
        }

        /// subtraction: time-point and timevalue
        inline time_value_scalar_duration
        operator-(TimeValueClock::time_point const &a,
                  util::time::TimeValue const &b) {
            return detail::wrap_time_value(a) - detail::wrap_time_value(b);
        }

        /// subtraction: timevalue and time-point
        template <typename Derived>
        inline time_value_scalar_duration
        operator-(util::time::TimeValue const &a,
                  TimeValueClock::time_point const &b) {
            return detail::wrap_time_value(a) - detail::wrap_time_value(b);
        }

        /// Treat/convert a TimeValue into a time point
        inline TimeValueClock::time_point
        as_time_point(time::TimeValue const &tv) {
            return TimeValueClock::time_point{TimeValueClock::duration{tv}};
        }

        /// Convert a TimeValue into a scalar duration
        inline TimeValueClock::scalar_duration
        to_duration(time::TimeValue const &tv) {
            return detail::wrap_time_value(tv).toScalarDuration();
        }
    } // namespace time
} // namespace util
} // namespace osvr

#endif // INCLUDED_TimeValueChrono_h_GUID_62662C09_CE8E_4394_87A9_26D2FC0D2920
