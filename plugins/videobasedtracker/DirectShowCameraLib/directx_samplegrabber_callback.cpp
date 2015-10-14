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
#include "directx_samplegrabber_callback.h"

// Library/third-party includes
#include <vrpn_Shared.h>

// Standard includes
// - none

//--------------------------------------------------------------------------------------------
// This section implements the callback handler that gets frames from the
// SampleGrabber filter.

directx_samplegrabber_callback::~directx_samplegrabber_callback() {
    // Make sure the other thread knows that it is okay to return the
    // buffer and wait until it has had time to do so.
    shutdown();
};

void directx_samplegrabber_callback::shutdown() {
    _stayAlive = false;
    Sleep(100);
}
HRESULT directx_samplegrabber_callback::QueryInterface(
    REFIID interfaceRequested, void **handleToInterfaceRequested) {
    if (handleToInterfaceRequested == nullptr) {
        return E_POINTER;
    }
    if (interfaceRequested == IID_IUnknown) {
        *handleToInterfaceRequested = static_cast<IUnknown *>(this);
    } else if (interfaceRequested == IID_ISampleGrabberCB) {
        *handleToInterfaceRequested = static_cast<ISampleGrabberCB *>(this);
    } else {
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

// This is the routine that processes each sample.  It gets the information
// about
// the sample (one frame) from the SampleGrabber, then marks itself as being
// ready
// to process the sample.  It then blocks until the sample has been processed by
// the associated camera server.
// The hand-off is handled by using two booleans acting as semaphores.
// The first semaphore (imageReady)
// controls access to the callback handler's buffer so that the application
// thread
// will only read it when it is full.  The second sempaphore (imageDone)
// controls when
// the handler routine can release a sample; it makes sure that the sample is
// not
// released before the application thread is done processing it.
// The directx camera must be sure to free an open sample (if any) after
// changing
// the state of the filter graph, so that this doesn't block indefinitely.  This
// means
// that the destructor for any object using this callback object has to destroy
// this object.  The destructor sets _stayAlive to false to make sure this
// thread terminates.

HRESULT directx_samplegrabber_callback::SampleCB(double time,
                                                 IMediaSample *sample) {
    // Point the image sample to the media sample we have and then set the flag
    // to tell the application it can process it.
    imageSample = sample;
    imageReady = true;

    // Wait until the image has been processed and then return the buffer to the
    // filter graph
    while (!imageDone && _stayAlive) {
        vrpn_SleepMsecs(1);
    }
    if (_stayAlive) {
        imageDone = false;
    }

    return S_OK;
}
