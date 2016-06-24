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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_MediaSampleExchange_h_GUID_6E82A284_0E96_4391_6408_D9602D2658D7
#define INCLUDED_MediaSampleExchange_h_GUID_6E82A284_0E96_4391_6408_D9602D2658D7

// Internal Includes
// - none

// Library/third-party includes
#include <osvr/Util/TimeValue.h>

// Standard includes
#include <cassert>
#include <chrono>
#include <memory>
#include <utility>

struct IMediaSample;
class MediaSampleExchange;
/// An RAII class for retrieving the sample and signalling consumption complete.
/// (A move-construction-only object.)
class Sample {
  public:
    /// Destructor - signals consumer finished.
    ~Sample();
    /// Move constructor
    Sample(Sample &&other)
        : sampleExchange_(other.sampleExchange_), sample_(other.sample_),
          timestamp_(other.timestamp_), owning_(false) {
        std::swap(owning_, other.owning_);
    }
    Sample &operator=(Sample const &) = delete;

    /// Access the contained sample.
    /// Do not retain pointers to this or any sub-object after this Sample
    /// object goes out of scope!
    IMediaSample &get() {
        assert(owning_ && "Shouldn't try to get the contained sample from a "
                          "moved-from wrapper!");
        return sample_;
    }

    osvr::util::time::TimeValue const &getTimestamp() const {
        assert(owning_ && "Shouldn't try to get the contained timestamp from a "
                          "moved-from wrapper!");
        return timestamp_;
    }

  private:
    Sample(MediaSampleExchange &exchange, IMediaSample &sample,
           osvr::util::time::TimeValue const &timestamp)
        : sampleExchange_(exchange), sample_(sample), timestamp_(timestamp),
          owning_(true) {}
    friend class MediaSampleExchange;
    MediaSampleExchange &sampleExchange_;
    IMediaSample &sample_;
    osvr::util::time::TimeValue timestamp_;
    bool owning_;
};

/// A class to mediate between a consumer of directshow image samples and the
/// ISampleGrabberCallback implementation, with signalling on the producer and
/// consumer side.
///
/// Hides the newer Windows API used for the sync primitives from the ancient
/// DirectShow-affiliated headers.
class MediaSampleExchange {
  public:
    MediaSampleExchange();
    ~MediaSampleExchange();
    /// Signals that a sample (passed in) was produced. The producer should not
    /// return from its callback (that is, not release that sample) until the
    /// sample has been consumed or some other event has taken place (shutdown).
    void signalSampleProduced(IMediaSample *sample,
                              osvr::util::time::TimeValue const &timestamp);
    void signalSampleProduced(IMediaSample *sample);

    /// @returns true if sample was made available, false if it timed out.
    bool waitForSample(std::chrono::milliseconds timeout);

    /// Retrieves the sample in an RAII wrapper, so that signalling of
    /// consumption is automatic when the sample object goes out of scope.
    Sample get() {
        IMediaSample *pin{sample_};
        return Sample{*this, *pin, timestamp_};
    }

    /// @returns true if sample was consumed, false if it timed out.
    bool waitForSampleConsumed(std::chrono::milliseconds timeout);

  private:
    /// Indicates consumer finished - called by the Sample class
    void signalSampleConsumed();
    friend class Sample;
    struct Impl;
    IMediaSample *sample_ = nullptr;
    osvr::util::time::TimeValue timestamp_ = {};
    std::unique_ptr<Impl> impl_;
};

using MediaSampleExchangePtr = std::shared_ptr<MediaSampleExchange>;

inline Sample::~Sample() {
    if (owning_) {
        sampleExchange_.signalSampleConsumed();
    }
}

#endif // INCLUDED_MediaSampleExchange_h_GUID_6E82A284_0E96_4391_6408_D9602D2658D7
