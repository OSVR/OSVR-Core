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

#ifndef INCLUDED_SampleGrabberWrapper_h_GUID_3A2E02D3_5BD5_44B2_E95F_A31FE3B13ED1
#define INCLUDED_SampleGrabberWrapper_h_GUID_3A2E02D3_5BD5_44B2_E95F_A31FE3B13ED1

// Internal Includes
#include "MediaSampleExchange.h"
#include "comutils/ComPtr.h"

// Library/third-party includes
// - none

// Standard includes
#include <strmif.h>

// Forward declarations
class directx_samplegrabber_callback;

class SampleGrabberWrapper {
  public:
    SampleGrabberWrapper();
    ~SampleGrabberWrapper();

    /// Get the producer/consumer object used to consume samples.
    MediaSampleExchangePtr getExchange() const { return sampleExchange_; }

    /// Get the SampleGrabber filter by its IBaseFilter interface.
    comutils::Ptr<IBaseFilter> getFilter() { return sampleGrabber_; }

    /// Forwards call on to SampleGrabber.
    void getConnectedMediaType(AM_MEDIA_TYPE &mt);

    /// Gets the callback to finish up all its samples.
    void shutdown();

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    MediaSampleExchangePtr sampleExchange_;
    comutils::Ptr<IBaseFilter> sampleGrabber_;
    std::unique_ptr<directx_samplegrabber_callback> callback_;
};
#endif // INCLUDED_SampleGrabberWrapper_h_GUID_3A2E02D3_5BD5_44B2_E95F_A31FE3B13ED1
