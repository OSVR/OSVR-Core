/** @file
    @brief Header

    @date 2015

    @author
    Russ Taylor working through ReliaSolve for Sensics.
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

#ifdef _WIN32
#ifndef INCLUDED_directx_camera_server_h_GUID_9322F126_0DA4_4DB9_11F3_DDBF76A6D9D9
#define INCLUDED_directx_camera_server_h_GUID_9322F126_0DA4_4DB9_11F3_DDBF76A6D9D9

// Internal Includes
#include "base_camera_server.h"

// Library/third-party includes
#include <boost/intrusive_ptr.hpp>
#include <intrusive_ptr_COM.h>

// Standard includes
#include <windows.h>
#include <stdexcept>
#include <memory>

// Include files for DirectShow video input
#include <dshow.h>

// All we need from the "deprecated" qedit.h header file are these few lousy
// names:
// - ISampleGrabber
// - ISampleGrabberCB
// - CLSID_SampleGrabber
// - IID_ISampleGrabber
// - CLSID_NullRenderer
// - IID_ISampleGrabberCB

#include "qedit_wrapper.h"

extern "C" const CLSID CLSID_SampleGrabber;
extern "C" const CLSID CLSID_NullRenderer;

/// @brief Template alias for our desired COM smart pointer.
template <typename T> using WinPtr = boost::intrusive_ptr<T>;

struct ConstructionError : std::runtime_error {
    ConstructionError(const char objName[])
        : std::runtime_error(
              std::string("directx_camera_server: Can't create ") + objName) {}
};

class ComInit;
using ComInstance = std::unique_ptr<ComInit>;
/// @brief Simple RAII class for handling COM initialization.
class ComInit {
  public:
    ComInit();
    ~ComInit();
    static ComInstance init() {
        auto ret = ComInstance{new ComInit};
        return ret;
    }
    ComInit(ComInit const &) = delete;
    ComInit &operator=(ComInit const &) = delete;
};

// This code (and the code in the derived videofile server) is
// based on information the book "Programming Microsoft DirectShow
// for Digital Video and Television", Mark D. Pesce.  Microsoft Press.
// ISBN 0-7356-1821-6.  This book was required reading for me to
// understand how the filter graphs run, how to control replay rate,
// and how to reliably grab all of the samples from a stream.
// The chapter on using the Sample Grabber Filter
// (ch. 11) was particularly relevant.  Note that I did not break the seal
// on the enclosed disk, which would require me to agree to licensing terms
// that include me not providing anyone with copies of my modified versions
// of the sample code in source-code format.  Instead, I used the information
// gained in the reading of the book to write by hand my own version of this
// code.  What a sick world we live in when example code can't be freely
// shared.

// The Microsoft Platform SDK must be installed on your machine
// to make this work; this code makes use of the BaseClasses library and
// other parts of the library.

class directx_samplegrabber_callback; //< Forward declaration

class directx_camera_server : public base_camera_server {
  public:
    /// Open the nth available camera.  First camera is 0.
    directx_camera_server(int which, unsigned width = 0, unsigned height = 0);
    /// Open the first camera whose DevicePath begins with the supplied prefix.
    directx_camera_server(std::string const &pathPrefix, unsigned width = 0,
                          unsigned height = 0);
    virtual ~directx_camera_server(void);

    /// Return the number of colors that the device has
    virtual unsigned get_num_colors() const { return 3; };

    /// Read an image to a memory buffer.  Max < min means "whole range"
    virtual bool read_image_to_memory(unsigned minX = 255, unsigned maxX = 0,
                                      unsigned minY = 255, unsigned maxY = 0,
                                      double exposure_millisecs = 250.0);

    /// Get pixels out of the memory buffer, RGB indexes the colors
    virtual bool get_pixel_from_memory(unsigned X, unsigned Y, vrpn_uint8 &val,
                                       int RGB = 0) const;
    virtual bool get_pixel_from_memory(unsigned X, unsigned Y, vrpn_uint16 &val,
                                       int RGB = 0) const;

    /// Get a pointer to the actual memory buffer.
    /// @todo Construct a safer way to access this.
    const unsigned char *get_pixel_buffer_pointer(void) const {
        return _buffer;
    }

    /// Matches a method on the OpenCV camera, to let us easily integrate into
    /// existing code.
    bool isOpened(void) const { return _started_graph; }

  protected:
    bool start_com_and_graphbuilder();
    bool open_moniker_and_finish_setup(WinPtr<IMoniker> pMoniker,
                                       unsigned width, unsigned height);
    virtual void close_device(void);

    /// Construct but do not open camera (used by derived classes)
    directx_camera_server();

    ComInstance _com;

    // Objects needed for DirectShow video input.
    WinPtr<IGraphBuilder> _pGraph; // Constructs a DirectShow filter graph

    WinPtr<IMediaControl>
        _pMediaControl;          // Handles media streaming in the filter graph
    WinPtr<IMediaEvent> _pEvent; // Handles filter graph events

    WinPtr<ICaptureGraphBuilder2> _pBuilder; // Filter graph builder

    WinPtr<IBaseFilter>
        _pSampleGrabberFilter;        // Grabs samples from the media stream
    WinPtr<ISampleGrabber> _pGrabber; // Interface for the sample grabber filter
    WinPtr<IAMStreamConfig>
        _pStreamConfig; // Interface to set the video dimensions

    // Memory pointers used to get non-virtual memory
    unsigned char *_buffer = nullptr; //< Buffer for what comes from camera,
    size_t _buflen = 0;               //< Length of that buffer
    bool _started_graph = false;      //< Did we start the filter graph running?
    unsigned _mode = 0;               //< Mode 0 = running, Mode 1 = paused.

    long _stride; //< How many bytes to skip when going to next line (may be
    // negative for upside-down images)

    // Pointer to the associated sample grabber callback object.
    std::unique_ptr<directx_samplegrabber_callback> _pCallback;

    virtual bool open_and_find_parameters(const int which, unsigned width,
                                          unsigned height);
    bool open_and_find_parameters(std::string const &pathPrefix, unsigned width,
                                  unsigned height);
    virtual bool read_one_frame(unsigned minX, unsigned maxX, unsigned minY,
                                unsigned maxY, unsigned exposure_millisecs);
};

#endif // INCLUDED_directx_camera_server_h_GUID_9322F126_0DA4_4DB9_11F3_DDBF76A6D9D9
#endif // _WIN32
