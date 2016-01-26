/** @file
    @brief Implementation

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

// Internal Includes
#include "BeaconSetupData.h"

// Library/third-party includes
#include <osvr/Util/Verbosity.h>

// Standard includes
// - none

namespace osvr {
namespace vbtracker {
    namespace {
        using vector_size_type = Vec3Vector::size_type;
        struct VectorSizes {
            vector_size_type minSize = 0;
            vector_size_type maxSize = 0;
        };
        inline VectorSizes getMinMaxSetupDataSizes(TargetSetupData const &d) {
            VectorSizes ret;
            /// an initializer_list of all the sizes of all member vectors.
            auto sizes = {d.patterns.size(),
                          d.locations.size(),
                          d.emissionDirections.size(),
                          d.baseMeasurementVariances.size(),
                          d.initialAutocalibrationErrors.size(),
                          d.isFixed.size()};
            /// Let's get the minimum and maximum size simultaneously!
            std::tie(ret.minSize, ret.maxSize) = std::minmax(sizes);
            return ret;
        }
        class TargetDataChecker {
          public:
            using size_type = Vec3Vector::size_type;
            TargetDataChecker(TargetSetupData &data, TargetDataSummary &summary)
                : d(data), m_summary(summary),
                  m_sizes(getMinMaxSetupDataSizes(data)) {}
            /// Main function - contains two loops (one for
            /// all-vectors-populated, another for uneven tails)
            void process() {
                /// Only iterate through the min at first
                for (size_type i = 0; i < m_sizes.minSize; ++i) {
                    if (disabledBeacon(i)) {
                        continue;
                    }
                    if (0 == m_patternLength) {
                        /// Initialize pattern length if it hasn't been
                        /// initialized already.
                        m_patternLength = d.patterns[i].size();
                        OSVR_DEV_VERBOSE("Determined that patterns are "
                                         << m_patternLength << " bits long.");
                    }
                    bool gotError = false;
                    checkPatternLength(i, gotError);
                    checkLocationValidity(i, gotError);
                    /// @todo more here

                    if (!gotError) {
                        m_summary.validBeacons.push_back(wrapIndex(i));
                    }
                }

                /// Now look at unevent tails.
                /// here this variable is kind of a dummy - we won't get any
                /// valid beacons out of this section, we just need a reference
                /// to pass.
                bool gotError = false;
                for (size_type i = m_sizes.minSize; i < m_sizes.maxSize; ++i) {

                    recordError(
                        i, "Mismatched vectors: Beacon ID exists "
                           "in at least one vector, but not in all vectors.");
                    if (i < d.patterns.size()) {
                        if (disabledBeacon(i)) {
                            /// @todo how to handle "uneven tails" with a
                            /// "disabled beacon" pattern?
                            continue;
                        }
                        checkPatternLength(i, gotError);
                    } else {
                        recordError(i, "Mismatched vectors: Beacon ID exists "
                                       "in at least one vector, but not enough "
                                       "entries in the pattern vector!");
                    }

                    if (i < d.locations.size()) {
                        checkLocationValidity(i, gotError);
                    } else {
                        recordError(i, "Mismatched vectors: Beacon ID exists "
                                       "in at least one vector, but not enough "
                                       "entries in the locations vector!");
                    }
                    /// @todo more here.
                }
            }

            /// @name Loop methods
            /// @{
            /// Check pattern for a disabled beacon - this one is special
            /// because you need to check its return value and potentially
            /// skip the rest of a beacon if it returns true.
            bool disabledBeacon(size_type i) {
                if (d.patterns[i].empty()) {
                    m_summary.disabledByEmptyPattern.push_back(wrapIndex(i));
                    return true;
                }
                if (d.patterns[i].find_first_not_of(".*") !=
                    std::string::npos) {
                    /// This pattern was disabled by adding an invalid
                    /// character to it.
                    /// Not an error (though we will clear the pattern for
                    /// the ease of downstream code.)
                    m_summary.disabledByPattern.push_back(wrapIndex(i));
                    return true;
                }
                return false;
            }
            void checkPatternLength(size_type i, bool &gotError) {
                /// Check pattern length.
                if (d.patterns[i].size() != m_patternLength) {
                    gotError = true;
                    recordError(i, "Pattern was non-empty and all legal "
                                   "characters, but length did not match.");
                }
            }

            void checkLocationValidity(size_type i, bool &gotError) {
                /// Check that location was initialized.
                if (d.locations[i] == TargetSetupData::getBogusLocation()) {
                    gotError = true;
                    recordError(i, "Beacon location is still the bogus "
                                   "'uninitialized' sentinel value.");
                }
            }

            void checkEmissionDirection(size_type i, bool &gotError) {
                /// @todo
            }
            /// @}

            /// @name Helpers for the loop methods
            /// @{
            template <typename T> void recordError(size_type i, T &&arg) {
                m_summary.errors.emplace_back(wrapIndex(i),
                                              std::forward<T>(arg));
            }
            static OneBasedBeaconId wrapIndex(size_type i) {
                return makeOneBased(ZeroBasedBeaconId(i));
            }
            /// @}

          private:
            TargetSetupData &d;
            TargetDataSummary &m_summary;
            std::size_t m_patternLength = 0;
            VectorSizes const m_sizes;
            size_type m_minSize = 0;
            size_type m_maxSize = 0;
        };
    } // namespace
    TargetDataSummary TargetSetupData::cleanAndValidate() {
        TargetDataSummary ret;
        TargetDataChecker checker(*this, ret);
        checker.process();
        return ret;
    }
} // namespace vbtracker
} // namespace osvr
