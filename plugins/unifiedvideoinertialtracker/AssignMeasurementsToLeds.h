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

#ifndef INCLUDED_AssignMeasurementsToLeds_h_GUID_F7146BCA_13BF_4C91_1EE6_27E9FF039AD7
#define INCLUDED_AssignMeasurementsToLeds_h_GUID_F7146BCA_13BF_4C91_1EE6_27E9FF039AD7

// Internal Includes
#include "BeaconIdTypes.h"
#include "LED.h"
#include <LedMeasurement.h>

// Library/third-party includes
#include <boost/assert.hpp>
#include <opencv2/core/core.hpp>

// Standard includes
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

namespace osvr {
namespace vbtracker {

    /// In theory this shouldn't happen, but there are checks
    /// scattered all over the code. Now we can say that it doesn't
    /// happen because we won't let any bad values escape this
    /// routine.
    inline bool handleOutOfRangeIds(Led &led, const std::size_t numBeacons) {
        if (led.identified() &&
            makeZeroBased(led.getID()).value() >
                static_cast<UnderlyingBeaconIdType>(numBeacons)) {
            std::cerr << "Got a beacon claiming to be "
                      << led.getOneBasedID().value() << " when we only have "
                      << numBeacons << " beacons" << std::endl;
            /// @todo a kinder way of doing this? Right now this blows away
            /// the measurement history
            led.markMisidentified();
            return true;
        }
        return false;
    }

    /// Get the squared distance between two OpenCV points
    inline float sqDist(cv::Point2f const &lhs, cv::Point2f const &rhs) {
        auto diff = lhs - rhs;
        return diff.dot(diff);
    }

    class AssignMeasurementsToLeds {
        static const char *getPrefix() { return "[AssignMeasurements] "; }

      public:
        AssignMeasurementsToLeds(LedGroup &leds,
                                 LedMeasurementVec const &measurements,
                                 const std::size_t numBeacons,
                                 float blobMoveThresh, bool verbose = false)
            : leds_(leds), measurements_(measurements), ledsEnd_(end(leds_)),
              numBeacons_(numBeacons_), blobMoveThreshFactor_(blobMoveThresh),
              maxMatches_(std::min(leds_.size(), measurements_.size())),
              verbose_(verbose) {}

        using LedAndMeasurement = std::pair<Led &, LedMeasurement const &>;

        using LedMeasDistance = std::tuple<std::size_t, std::size_t, float>;
        using HeapValueType = LedMeasDistance;
        using HeapType = std::vector<HeapValueType>;
        using size_type = HeapType::size_type;

        /// Must call first, and only once.
        void populateStructures() {
            BOOST_ASSERT_MSG(!populated_,
                             "Can only call populateStructures() once.");
            populated_ = true;
            {
                /// Clean up LEDs and populate their ref vector.
                auto led = begin(leds_);
                while (led != end(leds_)) {
                    led->resetUsed();
                    ledRefs_.push_back(led);
                    ++led;
                }
            }

            for (auto &meas : measurements_) {
                /// Populate the measurement ref vector.
                measRefs_.push_back(&meas);
            }

            /// Do the O(n * m) distance computation to populate the vector that
            /// will become our min-heap.
            auto nMeas = measRefs_.size();
            auto nLed = ledRefs_.size();
            for (size_type measIdx = 0; measIdx < nMeas; ++measIdx) {
                auto distThreshSquared =
                    getDistanceThresholdSquared(*measRefs_[measIdx]);
                for (size_type ledIdx = 0; ledIdx < nLed; ++ledIdx) {
                    /// WARNING: watch the order of arguments to this function,
                    /// since the type of the indices is identical...
                    possiblyPushLedMeasurement(ledIdx, measIdx,
                                               distThreshSquared);
                }
            }
            /// Turn that vector into our min-heap.

            /// More efficient to do this one-time 3N=O(n) operation, than
            /// repeatedly push_heap (2log(N)=O(log n)) as we go, because:
            ///   - we don't need the heap property until we've completed
            ///   processing all the input (so push_heap gives us no benefit
            ///   there)
            ///   - the max number of comparisons, by the requirements of the
            ///   standard, are lower (and linear) and stay lower for the
            ///   make_heap approach for a heap size of > 5.
            makeHeap();
        }

        /// Discards invalid entries (those where either the LED or the
        /// measurement, or both, have already been assigned) from the heap, and
        /// returns the count of entries so discarded.
        size_type discardInvalidEntries(bool verbose = false) {
            checkAndThrowNotPopulated("discardInvalidEntries()");
            size_type discarded = 0;
            if (empty()) {
                return discarded;
            }

            while (!empty()) {
                if (verbose || verbose_) {
                    auto top = distanceHeap_.front();
                    std::cout << getPrefix() << "top: led index "
                              << ledIndex(top) << "\tmeas index "
                              << measIndex(top) << "\tsq dist "
                              << squaredDistance(top);
                }
                if (isTopValid()) {
                    if (verbose || verbose_) {
                        std::cout << " isTopValid() says keep!\n";
                    }
                    /// Great, we found one!
                    return discarded;
                }
                if (verbose || verbose_) {
                    std::cout << " isTopValid() says discard!\n";
                }
                popHeap();
                discarded++;
            }
            return discarded;
        }

        /// In case a measurement update goes bad, we can try to "un-mark" a
        /// measurement as consumed.
        bool resumbitMeasurement(LedMeasurement const &meas) {
            if (numMatches_ == 0) {
                // can't have been consumed in the first place!
                /// @todo do we throw a logic error here?
                return false;
            }

            auto it = std::find(begin(measurements_), end(measurements_), meas);
            if (it == end(measurements_)) {
                // sorry, can't help...
                return false;
            }

            auto idx = std::distance(begin(measurements_), it);
            if (isMeasValid(idx)) {
                std::cerr << "Trying to resubmit, but the measurement wasn't "
                             "marked as consumed!"
                          << std::endl;
                return false;
            }
            /// Un-count the match.
            numMatches_--;
            /// Restore the entry in the refs table.
            measRefs_[idx] = &(*it);
            return true;
        }

        /// Searches the heap, discarding now-invalid entries, until it
        /// finds an entry where both the LED and the measurement are unclaimed,
        /// or it runs out of entries.
        bool hasMoreMatches(bool verbose = false) {
            checkAndThrowNotPopulated("hasMoreMatches()");
            if (haveMadeMaxMatches()) {
                /// Early out:
                /// We've already matched up all of one type or another.
                if (verbose || verbose_) {
                    std::cout << getPrefix() << "hasMoreMatches: Already "
                                                "reached our limit for "
                                                "matches!\n";
                }
                return false;
            }
            auto discarded = discardInvalidEntries();
            if (verbose || verbose_) {
                std::cout << getPrefix() << "hasMoreMatches: Discarded "
                          << discarded << " invalid entries.\n";
            }
            if (empty()) {
                return false;
            }
            return isTopValid();
        }

        /// Requires that hasMoreMatches() has been run and returns true.
        LedAndMeasurement getMatch(bool verbose = false) {
            checkAndThrowNotPopulated("getMatch()");

            auto hasMatch = hasMoreMatches();

            if (!hasMatch) {
                throw std::logic_error("Can't call getMatch() without first "
                                       "getting success from hasMoreMatches()");
            }

            if (verbose || verbose_) {
                std::cout << getPrefix() << "Led Index "
                          << ledIndex(distanceHeap_.front()) << "\tMeas Index "
                          << measIndex(distanceHeap_.front()) << std::endl;
            }
            BOOST_ASSERT_MSG(isTopValid(), "Shouldn't be able to get here "
                                           "without a valid top element on the "
                                           "heap.");

            auto &topLed = *getTopLed();
            auto &topMeas = *getTopMeasurement();

            /// Mark that we've used this LED and measurement.
            markTopConsumed();

            /// Postcondition check - shouldn't be valid anymore, we just
            /// consumed it.
            BOOST_ASSERT(!isTopValid());

            /// Now, remove this entry from the heap.
            popHeap();

            /// Count it
            numMatches_++;

            /// and return the reward.
            return LedAndMeasurement(topLed, topMeas);
        }

        /// Have we made as many matches as we possibly can? (that is, the
        /// minimum of the number of LED objects and the number of measurements)
        bool haveMadeMaxMatches() const {
            /// Not terribly harmful here, just illogical, so assert instead
            /// of unconditional check and throw.
            BOOST_ASSERT_MSG(populated_, "Should have called "
                                         "populateStructures() before calling "
                                         "haveMadeMaxMatches().");
            return numMatches_ == maxMatches_;
        }

        /// The maximum number of matches theoretically achievable with this
        /// input: the minimum of the number of LED objects and the number of
        /// measurements
        size_type maxMatches() const { return maxMatches_; }

        /// Is our heap of possibilities empty?
        bool empty() const {
            /// Not terribly harmful here, just illogical, so assert instead
            /// of unconditional check and throw.
            BOOST_ASSERT_MSG(populated_, "Should have called "
                                         "populateStructures() before calling "
                                         "empty().");
            return distanceHeap_.empty();
        }

        /// Entries in the heap of possibilities.
        size_type size() const {
            /// Not terribly harmful here, just illogical, so assert instead
            /// of nconditional check and throw.
            BOOST_ASSERT_MSG(populated_, "Should have called "
                                         "populateStructures() before "
                                         "calling size().");
            return distanceHeap_.size();
        }

        /// This is the size it could have potentially been, had all LEDs
        /// been within the distance threshold. (O(n m))
        size_type theoreticalMaxSize() const {
            return leds_.size() * measurements_.size();
        }

        /// The fraction of the theoretical max that the size is.
        double heapSizeFraction() const {
            /// Not terribly harmful here, just illogical, so assert instead
            /// of unconditional check and throw.
            BOOST_ASSERT_MSG(populated_, "Should have called "
                                         "populateStructures() before calling "
                                         "heapSizeFraction().");
            return static_cast<double>(size()) /
                   static_cast<double>(theoreticalMaxSize());
        }

        size_type numUnclaimedLedObjects() const {
            return std::count_if(
                begin(ledRefs_), end(ledRefs_),
                [&](LedIter const &it) { return it != ledsEnd_; });
        }

        void eraseUnclaimedLedObjects(bool verbose = false) {
            for (auto &ledIter : ledRefs_) {
                if (ledIter == ledsEnd_) {
                    /// already used
                    continue;
                }
                if (verbose) {
                    if (ledIter->identified()) {
                        std::cout << "Erasing identified LED "
                                  << ledIter->getOneBasedID().value()
                                  << " because of a lack of updated data.\n";
                    } else {
                        std::cout << "Erasing unidentified LED at "
                                  << ledIter->getLocation()
                                  << " because of a lack of updated data.\n";
                    }
                }
                leds_.erase(ledIter);
            }
        }

        size_type numUnclaimedMeasurements() const {
            return std::count_if(
                begin(measRefs_), end(measRefs_),
                [&](MeasPtr const &ptr) { return ptr != nullptr; });
        }

        template <typename F> void forEachUnclaimedMeasurement(F &&op) {
            for (auto &measRef : measRefs_) {
                if (!measRef) {
                    /// already used
                    continue;
                }
                /// Apply the operation.
                std::forward<F>(op)(*measRef);
            }
        }

        size_type numCompletedMatches() const { return numMatches_; }

      private:
        using LedIter = LedGroup::iterator;
        using LedPtr = Led *;
        using MeasPtr = LedMeasurement const *;
        void checkAndThrowNotPopulated(const char *functionName) const {
            if (!populated_) {
                throw std::logic_error("Must have called "
                                       "populateStructures() before "
                                       "calling " +
                                       std::string(functionName));
            }
        }

        /// @name Accessors for tuple elements.
        /// @{
        static std::size_t ledIndex(LedMeasDistance const &val) {
            return std::get<0>(val);
        }
        static std::size_t &ledIndex(LedMeasDistance &val) {
            return std::get<0>(val);
        }
        static std::size_t measIndex(LedMeasDistance const &val) {
            return std::get<1>(val);
        }
        static std::size_t &measIndex(LedMeasDistance &val) {
            return std::get<1>(val);
        }
        static float squaredDistance(LedMeasDistance const &val) {
            return std::get<2>(val);
        }
        /// @}

        void possiblyPushLedMeasurement(std::size_t ledIdx, std::size_t measIdx,
                                        float distThreshSquared) {
            auto meas = measRefs_[measIdx];
            auto led = ledRefs_[ledIdx];
            auto squaredDist = sqDist(led->getLocation(), meas->loc);
            if (squaredDist < distThreshSquared) {
                // If we're within the threshold, let's push this candidate
                // on the vector that will be turned into a heap.
                distanceHeap_.emplace_back(ledIdx, measIdx, squaredDist);
            }
        }
        LedIter getTopLed() const {
            return ledRefs_[ledIndex(distanceHeap_.front())];
        }

        MeasPtr getTopMeasurement() const {
            return measRefs_[measIndex(distanceHeap_.front())];
        }

        bool isLedValid(size_type idx) const {
            return (ledRefs_[idx] != ledsEnd_);
        }

        bool isLedValid(LedMeasDistance const &elt) const {
            return (ledRefs_[ledIndex(elt)] != ledsEnd_);
        }

        bool isMeasValid(size_type idx) const {
            return (measRefs_[idx] != nullptr);
        }

        bool isMeasValid(LedMeasDistance const &elt) const {
            return (measRefs_[measIndex(elt)] != nullptr);
        }

        bool isTopValid() const {
            LedMeasDistance elt = distanceHeap_.front();
            return isLedValid(elt) && isMeasValid(elt);
        }

        void markTopConsumed() {
            LedMeasDistance elt = distanceHeap_.front();
            ledRefs_[ledIndex(elt)] = ledsEnd_;
            measRefs_[measIndex(elt)] = nullptr;
            /// Postcondition assertion.
            BOOST_ASSERT(!isLedValid(elt));
            BOOST_ASSERT(!isMeasValid(elt));
        }

        /// For a given measurement, compute the corresponding search distance
        /// threshold
        ///
        /// Squared, so we can just use squared norm instead of doing square
        /// roots
        float getDistanceThresholdSquared(LedMeasurement const &meas) const {
            auto thresh = blobMoveThreshFactor_ * meas.diameter;
            return thresh * thresh;
        }

        /// min heap comparator needs greater-than, want to compare on the
        /// "squared distance" (third) tuple element.
        class Comparator {
          public:
            bool operator()(HeapValueType const &lhs,
                            HeapValueType const &rhs) const {
                return squaredDistance(lhs) > squaredDistance(rhs);
            }
        };

        void makeHeap() {
            /// cost of 3 * len, which is O(n m)
            /// Paid once, at the end of populateStructures()
            std::make_heap(begin(distanceHeap_), end(distanceHeap_),
                           Comparator());
        }

        /// Resizes for you too.
        void popHeap() {
            BOOST_ASSERT_MSG(!distanceHeap_.empty(),
                             "Cannot pop from an empty heap");
            /// Puts the new thing in the front, and moves the old front to the
            /// back.
            std::pop_heap(begin(distanceHeap_), end(distanceHeap_),
                          Comparator());
            /// We get rid of the back.
            distanceHeap_.pop_back();
        }
		
        bool populated_ = false;
        std::vector<LedIter> ledRefs_;
        std::vector<MeasPtr> measRefs_;
        HeapType distanceHeap_;
        size_type numMatches_ = 0;
        LedGroup &leds_;
        LedMeasurementVec const &measurements_;
        const LedIter ledsEnd_;
        const std::size_t numBeacons_;
        const float blobMoveThreshFactor_;
        const size_type maxMatches_;
        const bool verbose_;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_AssignMeasurementsToLeds_h_GUID_F7146BCA_13BF_4C91_1EE6_27E9FF039AD7
