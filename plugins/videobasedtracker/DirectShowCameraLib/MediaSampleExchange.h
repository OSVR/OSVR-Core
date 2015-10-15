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
// - none

// Standard includes
#include <memory>
#include <chrono>
#include <utility>

struct IMediaSample;
class MediaSampleExchange;
/// An RAII class for retrieving the sample and signalling consumption complete.
class Sample {
  public:
    ~Sample();
    Sample(Sample &&other)
        : sampleExchange_(other.sampleExchange_), sample_(other.sample_),
          owning_(false) {
        std::swap(owning_, other.owning_);
    }
    IMediaSample &get() { return sample_; }

  private:
    Sample(MediaSampleExchange &exchange, IMediaSample &sample)
        : sampleExchange_(exchange), sample_(sample), owning_(true) {}
    friend class MediaSampleExchange;
    MediaSampleExchange &sampleExchange_;
    IMediaSample &sample_;
    bool owning_;
};
class MediaSampleExchange {
  public:
    MediaSampleExchange();
    ~MediaSampleExchange();

    void signalSampleProduced(IMediaSample *sample);
    /// @returns true if sample was made available, false if it timed out.
    bool waitForSample(std::chrono::milliseconds timeout);

    IMediaSample &getSample() {
        IMediaSample *pin{sample_};
        return *pin;
    }

    Sample get() {
        IMediaSample *pin{sample_};
        return Sample{*this, *pin};
    }

    void signalSampleConsumed();
    /// @returns true if sample was consumed, false if it timed out.
    bool waitForSampleConsumed(std::chrono::milliseconds timeout);

  private:
    struct Impl;
    IMediaSample *volatile sample_ = nullptr;
    std::unique_ptr<Impl> impl_;
};

using MediaSampleExchangePtr = std::shared_ptr<MediaSampleExchange>;

inline Sample::~Sample() {
    if (owning_) {
        sampleExchange_.signalSampleConsumed();
    }
}

#endif // INCLUDED_MediaSampleExchange_h_GUID_6E82A284_0E96_4391_6408_D9602D2658D7
