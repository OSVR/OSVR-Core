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
            makeZeroBased(led.getID()).value() > numBeacons) {
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
      public:
        AssignMeasurementsToLeds(LedGroup &leds,
                                 LedMeasurementVec &measurements,
                                 const std::size_t numBeacons,
                                 float blobMoveThresh)
            : leds_(leds), measurements_(measurements), ledsEnd_(end(leds_)),
              numBeacons_(numBeacons_), blobMoveThreshFactor_(blobMoveThresh) {}

        void populateStructures() {
            {
                /// Clean up LEDs and populate their ref vector.
                auto led = begin(leds_);
                while (led != end(leds_)) {
                    led->resetUsed();
                    handleOutOfRangeIds(*led, numBeacons_);
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
            makeHeap();
        }

        using LedAndMeasurement = std::pair<Led &, LedMeasurement &>;

        /// Searches the heap, discarding now-invalid entries, until it finds an
        /// entry where both the LED and the measurement are unclaimed, or it
        /// runs out of entries.
        bool hasMoreMatches() {
            if (distanceHeap_.empty()) {
                return false;
            }

            /// This class deals with avoiding popping too much and also with
            /// erasing in bulk at end of scope.
            HeapUsage heap(distanceHeap_);
            while (!heap.empty()) {
                if (isTopValid()) {
                    /// Great, we found one!
                    return true;
                }
                heap.pop();
            }
            return false;
        }

        /// Requires that hasMoreMatches() has been run and returns true.
        LedAndMeasurement getMatch() {
            auto hasMatch = hasMoreMatches();
            if (!hasMatch) {
                throw std::logic_error("Can't call getMatch() without first "
                                       "getting success from hasMoreMatches()");
            }
            auto &topLed = *getTopLed();
            auto &topMeas = *getTopMeasurement();
            /// Mark that we've used this LED and measurement.
            markTopConsumed();
            /// Now, remove this entry from the heap.
            popHeap();
            /// and return the reward.
            return LedAndMeasurement(topLed, topMeas);
        }

      private:
        using LedIter = LedGroup::iterator;
        using LedPtr = Led *;
        using MeasPtr = LedMeasurement *;
        using LedMeasDistance = std::tuple<std::size_t, std::size_t, float>;

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

        using size_type = std::vector<LedMeasDistance>::size_type;
        void possiblyPushLedMeasurement(std::size_t ledIdx, std::size_t measIdx,
                                        float distThreshSquared) {
            auto meas = measRefs_[measIdx];
            auto led = ledRefs_[ledIdx];
            auto squaredDist = sqDist(led->getLocation(), meas->loc);
            if (squaredDist < distThreshSquared) {
                // If we're within the threshold, let's push this candidate on
                // the vector that will be turned into a heap.
                distanceHeap_.emplace_back(ledIdx, measIdx, distThreshSquared);
            }
        }
        LedIter getTopLed() const {
            return ledRefs_[ledIndex(distanceHeap_.front())];
        }
#if 0
        LedPtr getTopLedPtr() const {
            auto ledRef = getTopLed();
            if (ledRef == getEmptyLed()) {
                return nullptr;
            }
            return &(*ledRef);
        }
#endif
        MeasPtr getTopMeasurement() const {
            return measRefs_[measIndex(distanceHeap_.front())];
        }

        bool isTopValid() const {
            LedMeasDistance elt = distanceHeap_.front();
            return (ledRefs_[ledIndex(elt)] != ledsEnd_) &&
                   measRefs_[measIndex(elt)];
        }

        void markTopConsumed() {
            LedMeasDistance elt = distanceHeap_.front();
            ledRefs_[ledIndex(elt)] = ledsEnd_;
            measRefs_[measIndex(elt)] = nullptr;
        }

        float getDistanceThresholdSquared(LedMeasurement const &meas) const {
            auto thresh = blobMoveThreshFactor_ * meas.diameter;
            return thresh * thresh;
        }

        using HeapValueType = LedMeasDistance;
        using HeapType = std::vector<HeapValueType>;

        /// min heap comparator needs greater-than, want to compare on third
        /// tuple element.
        class Comparator {
          public:
            bool operator()(HeapValueType const &lhs,
                            HeapValueType const &rhs) const {
                return squaredDistance(lhs) > squaredDistance(rhs);
            }
        };

        void makeHeap() {
            /// cost of 3 * len, which is O(n m)
            std::make_heap(begin(distanceHeap_), end(distanceHeap_),
                           Comparator());
        }
        class HeapUsage {
          public:
            HeapUsage(HeapType &heap) : heap_(heap), n_(heap.size()) {}
            ~HeapUsage() {
                if (numPopped_ > 0) {
                    heap_.resize(n_ - numPopped_);
                }
            }
            HeapUsage(HeapUsage const &) = delete;
            HeapUsage &operator=(HeapUsage const &) = delete;
            using size_type = HeapType::size_type;
            void pop() {
                if (empty()) {
                    return;
                }

                std::pop_heap(begin(heap_), end(heap_), Comparator());
                numPopped_++;
            }

            bool empty() const { return n_ == 0 || n_ == numPopped_; }

            size_type size() const { return n_ - numPopped_; }

          private:
            HeapType &heap_;
            const HeapType::size_type n_;
            std::size_t numPopped_ = 0;
        };

        /// does not resize for you!
        void rawPopHeap() {
            BOOST_ASSERT_MSG(!distanceHeap_.empty(),
                             "Cannot pop from an empty heap");
            std::pop_heap(begin(distanceHeap_), end(distanceHeap_),
                          Comparator());
        }

        /// Resizes for you too.
        void popHeap() {
            BOOST_ASSERT_MSG(!distanceHeap_.empty(),
                             "Cannot pop from an empty heap");
            std::pop_heap(begin(distanceHeap_), end(distanceHeap_),
                          Comparator());
            distanceHeap_.resize(distanceHeap_.size() - 1);
        }

        void dropLastEntries(size_type numEntries) {
            if (numEntries == 0) {
                return;
            }
            auto n = distanceHeap_.size();
            BOOST_ASSERT_MSG(
                numEntries <= n,
                "Cannot drop more entries from heap than exist in it.");
            distanceHeap_.resize(n - numEntries);
        }

        // LedIter getEmptyLed() const { return end(leds_); }
        LedGroup &leds_;
        LedMeasurementVec &measurements_;
        const LedIter ledsEnd_;
        const std::size_t numBeacons_;
        const float blobMoveThreshFactor_;
        std::vector<LedIter> ledRefs_;
        std::vector<MeasPtr> measRefs_;
        HeapType distanceHeap_;
    };

} // namespace vbtracker
} // namespace osvr

#endif // INCLUDED_AssignMeasurementsToLeds_h_GUID_F7146BCA_13BF_4C91_1EE6_27E9FF039AD7
