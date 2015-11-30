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

#ifndef INCLUDED_directx_samplegrabber_callback_h_GUID_F0A39DA1_440B_4E8A_AF25_1383D20934AA
#define INCLUDED_directx_samplegrabber_callback_h_GUID_F0A39DA1_440B_4E8A_AF25_1383D20934AA

// Internal Includes
#include "MediaSampleExchange.h"

// Library/third-party includes
// - none

// Standard includes
#include <dshow.h>
#include "qedit_wrapper.h"

// This class is used to handle callbacks from the SampleGrabber filter.  It
// grabs each sample and holds onto it until the camera server that is
// associated with the object comes and gets it.  The callback method in this
// class is called in another thread, so its methods need to be controlled:
// the MediaSampleExchange object shared between this object and the camera
// server regulates that produce/consume cycle.
class directx_samplegrabber_callback : public ISampleGrabberCB {
  public:
    directx_samplegrabber_callback(MediaSampleExchangePtr const &exchange);
    ~directx_samplegrabber_callback();

    void shutdown();

    // These three methods must be defined because of the IUnknown parent class.
    // XXX The first two are a hack to pretend that we are doing reference
    // counting; this object must last longer than the sample grabber it is
    // connected to in order to avoid segmentations faults.
    STDMETHODIMP_(ULONG) AddRef(void) { return 1; }
    STDMETHODIMP_(ULONG) Release(void) { return 2; }
    STDMETHOD(QueryInterface)
    (REFIID interfaceRequested, void **handleToInterfaceRequested);

    // One of the following two methods must be defined do to the
    // ISampleGraberCB parent class; this is the way we hear from the grabber.
    // We implement the one that gives us unbuffered access.  Be sure to turn
    // off buffering in the SampleGrabber that is associated with this callback
    // handler.
    STDMETHODIMP BufferCB(double, BYTE *, long) { return E_NOTIMPL; }
    STDMETHOD(SampleCB)(double time, IMediaSample *sample);

  private:
    BITMAPINFOHEADER _bitmapInfo;    //< Describes format of the bitmap
    volatile bool _stayAlive = true; //< Tells all threads to exit
    MediaSampleExchangePtr sampleExchange_;
};

#endif // INCLUDED_directx_samplegrabber_callback_h_GUID_F0A39DA1_440B_4E8A_AF25_1383D20934AA
