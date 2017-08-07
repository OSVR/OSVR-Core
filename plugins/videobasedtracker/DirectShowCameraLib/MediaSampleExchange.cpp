/** @file
    @brief Implementation

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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "MediaSampleExchange.h"
#include "SignalEvent.h"

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
// - none

struct MediaSampleExchange::Impl {
    SignalEvent produced;
    SignalEvent consumed;
};
MediaSampleExchange::MediaSampleExchange()
    : impl_(new MediaSampleExchange::Impl) {}

MediaSampleExchange::~MediaSampleExchange() {
    // Required for unique_ptr pimpl - see http://herbsutter.com/gotw/_100/
}

void MediaSampleExchange::signalSampleProduced(
    IMediaSample *sample, osvr::util::time::TimeValue const &timestamp) {
    BOOST_ASSERT_MSG(
        sample != nullptr,
        "Should not be signalling that there is a null sample available!");
    BOOST_ASSERT_MSG(sample_ == nullptr,
                     "Sample should be consumed before the next one produced!");
    sample_ = sample;
    timestamp_ = timestamp;
    impl_->produced.set();
}

void MediaSampleExchange::signalSampleProduced(IMediaSample *sample) {
    signalSampleProduced(sample, osvr::util::time::getNow());
}

bool MediaSampleExchange::waitForSample(std::chrono::milliseconds timeout) {
    return impl_->produced.wait(static_cast<DWORD>(timeout.count()));
}

void MediaSampleExchange::signalSampleConsumed() {
    BOOST_ASSERT_MSG(sample_ != nullptr,
                     "Sample pointer should not be null when consumed!");
    sample_ = nullptr;
    impl_->consumed.set();
}

bool MediaSampleExchange::waitForSampleConsumed(
    std::chrono::milliseconds timeout) {
    BOOST_ASSERT_MSG(
        sample_ != nullptr,
        "Sample pointer should not be null when waiting for consumer!");
    return impl_->consumed.wait(static_cast<DWORD>(timeout.count()));
}
