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
#include "SampleGrabberWrapper.h"
#include "directx_samplegrabber_callback.h"

// Library/third-party includes
// - none

// Standard includes
#include <stdexcept>
#include <dshow.h>
#include "qedit_wrapper.h"

extern "C" const CLSID CLSID_SampleGrabber;
struct SampleGrabberWrapper::Impl {
    comutils::Ptr<ISampleGrabber> grabber;
};

SampleGrabberWrapper::SampleGrabberWrapper()
    : impl_(new Impl), sampleExchange_(std::make_shared<MediaSampleExchange>()),
      callback_(new directx_samplegrabber_callback(sampleExchange_)) {

#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "CoCreateInstance SampleGrabber\n");
#endif
    // Create the sample grabber
    CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IBaseFilter, AttachPtr(sampleGrabber_));
    if (!sampleGrabber_) {
        throw std::runtime_error("Could not create Sample Grabber filter - "
                                 "possibly missing DirectShow/DirectX 8.1+?");
    }
    // Get its sample grabber interface
    sampleGrabber_->QueryInterface(IID_ISampleGrabber,
                                   AttachPtr(impl_->grabber));

    // Do the setup...

    // Ask for video media producers that produce 8-bit RGB
    AM_MEDIA_TYPE mt = {0};
    mt.majortype = MEDIATYPE_Video;  // Ask for video media producers
    mt.subtype = MEDIASUBTYPE_RGB24; // Ask for 8 bit RGB
    impl_->grabber->SetMediaType(&mt);

    impl_->grabber->SetCallback(callback_.get(),
                                0 /* to call the SampleCB method */);
    // Set the grabber do not do one-shot mode because that would cause
    // it to stop the stream after a single frame is captured.
    impl_->grabber->SetOneShot(FALSE);

    // Set the grabber to not do buffering mode, because we've not
    // implemented the handler for buffered callbacks.
    impl_->grabber->SetBufferSamples(FALSE);
}

SampleGrabberWrapper::~SampleGrabberWrapper() { shutdown(); }

void SampleGrabberWrapper::getConnectedMediaType(AM_MEDIA_TYPE &mt) {
    BOOST_ASSERT(impl_ && impl_->grabber);
    auto hr = impl_->grabber->GetConnectedMediaType(&mt);
    if (FAILED(hr)) {
        throw std::runtime_error("Could not get connected media type - is "
                                 "there nothing connected on the input pin of "
                                 "the filter?");
    }
}

void SampleGrabberWrapper::shutdown() {
    // Clear the callback
    BOOST_ASSERT(impl_ && impl_->grabber);
    impl_->grabber->SetCallback(nullptr, 0 /* to call the SampleCB method */);
    // Tell the callback object to finish up.
    if (callback_) {
        callback_->shutdown();
        // callback_.reset();
    }
}
