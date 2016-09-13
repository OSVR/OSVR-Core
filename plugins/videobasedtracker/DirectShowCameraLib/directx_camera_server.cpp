/** @file
    @brief Implementation

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

// Internal Includes
#include "directx_camera_server.h"
#include "ConnectTwoFilters.h"
#include "NullRenderFilter.h"
#include "PropertyBagHelper.h"
#include "dibsize.h"
#include "directx_samplegrabber_callback.h"
#include <osvr/Util/WideToUTF8.h>

// Library/third-party includes
// - none

// Standard includes
#include <chrono>
#include <cmath>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Uncomment to get a full device name and path listing in enumeration, instead
// of silently enumerating and early-exiting when we find one we like.
//#define VERBOSE_ENUM

#undef DXCAMSERVER_VERBOSE

//#define HACK_TO_REOPEN
//#define	DEBUG

/// @brief Checks something to see if it's false-ish, printing a message and
/// throwing an exception if it is.
template <typename T>
inline void checkForConstructionError(T const &ptr, const char objName[]) {
    if (!ptr) {
        fprintf(stderr, "directx_camera_server: Can't create %s\n", objName);
        throw ConstructionError(objName);
    }
}

/** The first time we are called, start the filter graph running in continuous
    mode and grab the first image that comes out.  Later times, grab each new
    image as it comes.  The "mode" parameter tells what mode we are in:
      Mode 0 = Run()
      Mode 1 = Pause()
    */
bool directx_camera_server::read_one_frame(unsigned minX, unsigned maxX,
                                           unsigned minY, unsigned maxY,
                                           unsigned exposure_millisecs) {
    HRESULT hr;

    if (!_status) {
        return false;
    };

#ifdef HACK_TO_REOPEN
    open_and_find_parameters();
    _started_graph = false;
#endif

    // If we have not yet started the media graph running, set up the callback
    // handler for the sample grabber filter so that we will hear about each
    // frame
    // as it comes in.  Then set the filter graph running.
    if (!_started_graph) {
        // Run the graph and wait until it captures a frame into its buffer
        switch (_mode) {
        case 0: // Case 0 = run
            hr = _pMediaControl->Run();
            break;
        case 1: // Case 1 = paused
            hr = _pMediaControl->Pause();
            break;
        default:
            fprintf(
                stderr,
                "directx_camera_server::read_one_frame(): Unknown mode (%d)\n",
                _mode);
            _status = false;
            return false;
        }
        if ((hr != S_OK) && (hr != S_FALSE)) {
            fprintf(stderr, "directx_camera_server::read_one_frame(): Can't "
                            "run filter graph, got %#010x\n",
                    static_cast<uint32_t>(hr));
            _status = false;
            return false;
        }

        _started_graph = true;
    }

    // XXX Should the app be allowed to set the timeout period?
    const int TIMEOUT_MSECS = 500;

    // Wait until there is a sample ready in the callback handler.  If there is,
    // copy it into our buffer and then tell it we are done processing the
    // sample.
    // If it takes too long, time out.
    BYTE *imageLocation;
    auto imageReady = sampleExchange_->waitForSample(
        std::chrono::milliseconds(TIMEOUT_MSECS));

    if (!imageReady) {
#ifdef DEBUG
        fprintf(stderr, "directx_camera_server::read_one_frame(): Timeout "
                        "when reading image\n");
#endif
        return false;
    }

    // If we are in mode 2, then we pause the graph after we captured one image.
    if (_mode == 2) {
        _pMediaControl->Pause();
        _mode = 1;
    }

    auto sampleWrapper = sampleExchange_->get();
    if (FAILED(sampleWrapper.get().GetPointer(&imageLocation))) {
        fprintf(stderr,
                "directx_camera_server::read_one_frame(): Can't get buffer\n");
        _status = false;
        return false;
    }

    // Store the timestamp
    ts_ = sampleWrapper.getTimestamp();

    // Step through each line of the video and copy it into the buffer.  We
    // do one line at a time here because there can be padding at the end of
    // each line on some video formats.
    for (DWORD iRow = 0; iRow < _num_rows; iRow++) {
        memcpy(_buffer.data() + _num_columns * 3 * iRow,
               imageLocation + _stride * iRow, _num_columns * 3);
    }

#ifdef HACK_TO_REOPEN
    close_device();
#endif
    return true;
}

inline std::string getDevicePath(PropertyBagHelper &prop) {
    return osvr::util::wideToUTF8String(prop.read(L"DevicePath"));
}

inline std::string getDeviceHumanDesc(PropertyBagHelper &prop) {
    auto desc = prop.read(L"Description");
    if (desc.empty()) {
        desc = prop.read(L"FriendlyName");
    }
    return desc.empty() ? std::string() : osvr::util::wideToUTF8String(desc);
}

bool directx_camera_server::start_com_and_graphbuilder() {
//-------------------------------------------------------------------
// Create COM and DirectX objects needed to access a video stream.

// Initialize COM.  This must have a matching uninitialize somewhere before
// the object is destroyed.
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "CoInitialize\n");
#endif

    _com = comutils::ComInit::init();
// Create the filter graph manager
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "CoCreateInstance FilterGraph\n");
#endif
    CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IGraphBuilder, AttachPtr(_pGraph));
    checkForConstructionError(_pGraph, "graph manager");

    _pGraph->QueryInterface(IID_IMediaControl, AttachPtr(_pMediaControl));

// Create the Capture Graph Builder.
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "CoCreateInstance CaptureGraphBuilder2\n");
#endif
    CoCreateInstance(CLSID_CaptureGraphBuilder2, nullptr, CLSCTX_INPROC,
                     IID_ICaptureGraphBuilder2, AttachPtr(_pBuilder));
    checkForConstructionError(_pBuilder, "graph builder");

// Associate the graph with the builder.
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "SetFilterGraph\n");
#endif
    _pBuilder->SetFiltergraph(_pGraph.get());
    return true;
}

/// @brief Checks something to see if it's false-ish, printing a message and
/// throwing an exception if it is.
template <typename T>
inline bool didConstructionFail(T const &ptr, const char objName[]) {
    if (!ptr) {
        fprintf(stderr, "directx_camera_server: Can't create %s\n", objName);
        return true;
    }
    return false;
}

// Enumerates the capture devices available, returning the first one where the
// passed functor (taking IMoniker& as a param) returns true.
template <typename F>
inline comutils::Ptr<IMoniker> find_first_capture_device_where(F &&f) {
    auto ret = comutils::Ptr<IMoniker>{};
// Create the system device enumerator.
#ifdef DEBUG
    printf("find_first_capture_device_where(): Before "
           "CoCreateInstance SystemDeviceEnum\n");
#endif
    auto pDevEnum = comutils::Ptr<ICreateDevEnum>{};
    CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC,
                     IID_ICreateDevEnum, AttachPtr(pDevEnum));
    if (didConstructionFail(pDevEnum, "device enumerator")) {
        return ret;
    }

// Create an enumerator for video capture devices.
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd407292(v=vs.85).aspx
#ifdef DEBUG
    printf("find_first_capture_device_where(): Before "
           "CreateClassEnumerator\n");
#endif
    auto pClassEnum = comutils::Ptr<IEnumMoniker>{};
    pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
                                    AttachPtr(pClassEnum), 0);
    if (didConstructionFail(pClassEnum, "video enumerator (no cameras?)")) {
        return ret;
    }

#ifdef DEBUG
    printf("find_first_capture_device_where(): Before Loop "
           "over enumerators\n");
#endif
// see
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd377566(v=vs.85).aspx
// for how to choose a camera
#ifdef VERBOSE_ENUM
    printf("\ndirectx_camera_server find_first_capture_device_where(): "
           "Beginning enumeration of video capture devices.\n\n");
#endif
    auto pMoniker = comutils::Ptr<IMoniker>{};
    while (pClassEnum->Next(1, AttachPtr(pMoniker), nullptr) == S_OK) {

#ifdef VERBOSE_ENUM
        printf("- '%s' at path:\n  '%s'\n\n",
               getDeviceHumanDesc(*pMoniker).c_str(),
               getDevicePath(*pMoniker).c_str());
#endif // VERBOSE_ENUM

        if (!ret && f(*pMoniker)) {
            ret = pMoniker;
#ifdef VERBOSE_ENUM
            printf("^^ Accepted that device! (Would have exited "
                   "enumeration here if VERBOSE_ENUM were not defined)\n\n");
#else // !VERBOSE_ENUM
            return ret; // Early out if we find it and we're not in verbose enum
                        // mode.
#endif
        }
    }

#ifdef VERBOSE_ENUM
    printf("\ndirectx_camera_server find_first_capture_device_where(): End "
           "enumeration.\n\n");
#endif

#ifdef DXCAMSERVER_VERBOSE
    if (!ret) {
        fprintf(stderr,
                "directx_camera_server find_first_capture_device_where(): "
                "No device satisfied the predicate.\n");
    }
#endif
    return ret;
}

//---------------------------------------------------------------------
// Open the camera specified and determine its available features and
// parameters.

bool directx_camera_server::open_and_find_parameters(const int which,
                                                     unsigned width,
                                                     unsigned height) {
    auto graphbuilderRet = start_com_and_graphbuilder();
    if (!graphbuilderRet) {
        return false;
    }
    std::size_t i = 0;

    auto pMoniker = find_first_capture_device_where([&i, which](IMoniker &) {
        if (i == which) {
            return true;
        }
        ++i;
        return false;
    });

    if (!pMoniker) {
        fprintf(stderr, "directx_camera_server::open_and_find_parameters(): "
                        "Could not get the device requested - not enough "
                        "cameras?\n");
        return false;
    }
#ifdef DEBUG
    std::cout << "directx_camera_server::open_and_find_parameters(): Accepted!"
              << std::endl;
#endif
    return open_moniker_and_finish_setup(pMoniker, FilterOperation{}, width,
                                         height);
}

bool directx_camera_server::open_and_find_parameters(
    std::string const &pathPrefix, FilterOperation const &sourceConfig,
    unsigned width, unsigned height) {
    auto graphbuilderRet = start_com_and_graphbuilder();
    if (!graphbuilderRet) {
        return false;
    }

    auto pMoniker =
        find_first_capture_device_where([&pathPrefix](IMoniker &mon) {
            auto props = PropertyBagHelper{mon};
            if (!props) {
                return false;
            }
            auto path = getDevicePath(props);
            if (path.length() < pathPrefix.length()) {
                return false;
            }
            return (path.substr(0, pathPrefix.length()) == pathPrefix);
        });

    if (!pMoniker) {
#ifdef DXCAMSERVER_VERBOSE
        fprintf(stderr, "directx_camera_server::open_and_find_parameters(): "
                        "Could not get the device requested - not enough "
                        "cameras?\n");
#endif
        return false;
    }
    {
        /// Try to get and save the device path for later usage.
        auto props = PropertyBagHelper{*pMoniker};
        if (props) {
            devicePath_ = getDevicePath(props);
        }
    }

#ifdef DEBUG
    std::cout << "directx_camera_server::open_and_find_parameters(): Accepted!"
              << std::endl;
#endif
    return open_moniker_and_finish_setup(pMoniker, sourceConfig, width, height);
}

inline bool setBufferLatency(ICaptureGraphBuilder2 &builder,
                             IBaseFilter &filter, std::size_t numBuffers) {

    auto negotiation =
        GetVideoCapturePinInterface<IAMBufferNegotiation>(builder, filter);

    if (!negotiation) {
        // failure
        std::cerr << "Could not get IAMBufferNegotiation" << std::endl;
        return false;
    }

    ALLOCATOR_PROPERTIES props;
    props.cbBuffer = -1;
    props.cBuffers = numBuffers;
    props.cbAlign = -1;
    props.cbPrefix = -1;
    auto hr = negotiation->SuggestAllocatorProperties(&props);
    if (FAILED(hr)) {
        return false;
    }
    return true;
}

bool directx_camera_server::open_moniker_and_finish_setup(
    comutils::Ptr<IMoniker> pMoniker, FilterOperation const &sourceConfig,
    unsigned width, unsigned height) {

    if (!pMoniker) {
        fprintf(stderr,
                "directx_camera_server::open_moniker_and_finish_setup(): "
                "Null device moniker passed: no device found?\n");
        return false;
    }
    auto prop = PropertyBagHelper{*pMoniker};
    printf("directx_camera_server: Using capture device '%s' at path '%s'\n",
           getDeviceHumanDesc(prop).c_str(), getDevicePath(prop).c_str());

    // Bind the chosen moniker to a filter object.
    auto pSrc = comutils::Ptr<IBaseFilter>{};
    pMoniker->BindToObject(nullptr, nullptr, IID_IBaseFilter, AttachPtr(pSrc));

    // Try to set it up with minimal latency by reducing buffers.
    if (setBufferLatency(*_pBuilder, *pSrc, 1)) {
        // std::cout << "Call to set buffer latency succeeded..." << std::endl;
    } else {
        std::cout << "directx_camera_server: Call to set capture buffer "
                     "latency failed."
                  << std::endl;
    }

    //-------------------------------------------------------------------
    // Construct the sample grabber that will be used to snatch images from
    // the video stream as they go by.  Set its media type and callback.

    // Create and configure the Sample Grabber.
    _pSampleGrabberWrapper.reset(new SampleGrabberWrapper);

    // Get the exchange object for receiving data from the sample grabber.
    sampleExchange_ = _pSampleGrabberWrapper->getExchange();

    //-------------------------------------------------------------------
    // Ask for the video resolution that has been passed in.
    // This code is based on
    // intuiting that we need to use the SetFormat call on the IAMStreamConfig
    // interface; this interface is described in the help pages.
    // If the width and height are specified as 0, then they are not set
    // in the header, letting them use whatever is the default.
    /// @todo factor this out into its own header.
    if ((width != 0) && (height != 0)) {
        auto pStreamConfig = comutils::Ptr<IAMStreamConfig>{};
        _pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                 pSrc.get(), IID_IAMStreamConfig,
                                 AttachPtr(pStreamConfig));
        checkForConstructionError(pStreamConfig, "StreamConfig interface");

        AM_MEDIA_TYPE mt = {0};
        mt.majortype = MEDIATYPE_Video;  // Ask for video media producers
        mt.subtype = MEDIASUBTYPE_RGB24; // Ask for 8 bit RGB
        VIDEOINFOHEADER vih = {0};
        mt.pbFormat = reinterpret_cast<BYTE *>(&vih);
        auto pVideoHeader = &vih;
        pVideoHeader->bmiHeader.biBitCount = 24;
        pVideoHeader->bmiHeader.biWidth = width;
        pVideoHeader->bmiHeader.biHeight = height;
        pVideoHeader->bmiHeader.biPlanes = 1;
        pVideoHeader->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        pVideoHeader->bmiHeader.biSizeImage = dibsize(pVideoHeader->bmiHeader);

        // Set the format type and size.
        mt.formattype = FORMAT_VideoInfo;
        mt.cbFormat = sizeof(VIDEOINFOHEADER);

        // Set the sample size.
        mt.bFixedSizeSamples = TRUE;
        mt.lSampleSize = dibsize(pVideoHeader->bmiHeader);

        // Make the call to actually set the video type to what we want.
        if (pStreamConfig->SetFormat(&mt) != S_OK) {
            fprintf(stderr, "directx_camera_server::open_and_find_parameters():"
                            " Can't set resolution to %dx%d using uncompressed "
                            "24-bit video\n",
                    pVideoHeader->bmiHeader.biWidth,
                    pVideoHeader->bmiHeader.biHeight);
            return false;
        }
    }

//-------------------------------------------------------------------
// Create a null renderer that will be used to discard the video frames
// on the output pin of the sample grabber

#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "createNullRenderFilter\n");
#endif
    auto pNullRender = createNullRenderFilter();
    auto sampleGrabberFilter = _pSampleGrabberWrapper->getFilter();
    //-------------------------------------------------------------------
    // Build the filter graph.  First add the filters and then connect them.

    // pSrc is the capture filter for the video device we found above.
    auto hr = _pGraph->AddFilter(pSrc.get(), L"Video Capture");
    BOOST_ASSERT_MSG(SUCCEEDED(hr), "Adding Video Capture filter to graph");

    // Add the sample grabber filter
    hr = _pGraph->AddFilter(sampleGrabberFilter.get(), L"SampleGrabber");
    BOOST_ASSERT_MSG(SUCCEEDED(hr), "Adding SampleGrabber filter to graph");

    // Add the null renderer filter
    hr = _pGraph->AddFilter(pNullRender.get(), L"NullRenderer");
    BOOST_ASSERT_MSG(SUCCEEDED(hr), "Adding NullRenderer filter to graph");

    // Connect the output of the video reader to the sample grabber input
    // ConnectTwoFilters(*_pGraph, *pSrc, *sampleGrabberFilter);
    auto capturePin = GetVideoCapturePinInterface<IPin>(*_pBuilder, *pSrc);
    {
        auto pIn = GetPin(*sampleGrabberFilter, PINDIR_INPUT);
        _pGraph->Connect(capturePin.get(), pIn.get());
    }

    // Connect the output of the sample grabber to the null renderer input
    ConnectTwoFilters(*_pGraph, *sampleGrabberFilter, *pNullRender);

    // If we were given a config action for the source, do it now.
    if (sourceConfig) {
        sourceConfig(*pSrc);
    }
    //-------------------------------------------------------------------
    // XXX See if this is a video tuner card by querying for that interface.
    // Set it to read the video channel if it is one.
    auto pTuner = GetPinInterface<IAMTVTuner>(*_pBuilder, *pSrc);
    if (pTuner) {
#ifdef DEBUG
        printf("directx_camera_server::open_and_find_parameters(): Found a TV "
               "Tuner!\n");
#endif

        // XXX Put code here.
        // Set the first input pin to use the cable as input
        hr = pTuner->put_InputType(0, TunerInputCable);
        if (FAILED(hr)) {
            fprintf(stderr, "directx_camera_server::open_and_find_parameters():"
                            " Can't set input to cable\n");
        }

        // Set the channel on the video to be baseband (is this channel zero?)
        hr = pTuner->put_Channel(0, -1, -1);
        if (FAILED(hr)) {
            fprintf(stderr, "directx_camera_server::open_and_find_parameters():"
                            " Can't set channel\n");
        }
    }

    {
        //-------------------------------------------------------------------
        // Find _num_rows and _num_columns in the video stream.
        AM_MEDIA_TYPE mt = {0};
        _pSampleGrabberWrapper->getConnectedMediaType(mt);
        VIDEOINFOHEADER *pVih;
        if (mt.formattype == FORMAT_VideoInfo ||
            mt.formattype == FORMAT_VideoInfo2) {
            pVih = reinterpret_cast<VIDEOINFOHEADER *>(mt.pbFormat);
        } else {
            fprintf(stderr,
                    "directx_camera_server::open_and_find_parameters(): "
                    "Can't get video header type\n");
            fprintf(stderr, "  (Expected %x or %x, got %x)\n", FORMAT_VideoInfo,
                    FORMAT_VideoInfo2, mt.formattype);
            fprintf(stderr, "  (GetConnectedMediaType is not valid for DirectX "
                            "headers later than version 7)\n");
            fprintf(stderr, "  (We need to re-implement reading video in some "
                            "other interface)\n");
            return false;
        }

        // Number of rows and columns.  This is different if we are using a
        // target
        // rectangle (rcTarget) than if we are not.
        if (IsRectEmpty(&pVih->rcTarget)) {
            _num_columns = pVih->bmiHeader.biWidth;
            _num_rows = pVih->bmiHeader.biHeight;
        } else {
            _num_columns = pVih->rcTarget.right;
            _num_rows = pVih->bmiHeader.biHeight;
            printf("XXX directx_camera_server::open_and_find_parameters(): "
                   "Warning: may not work correctly with target rectangle\n");
        }
#ifdef DEBUG
        printf("Got %dx%d video\n", _num_columns, _num_rows);
#endif

        // Make sure that the image is not compressed and that we have 8 bits
        // per pixel.
        if (pVih->bmiHeader.biCompression != BI_RGB) {
            fprintf(stderr,
                    "directx_camera_server::open_and_find_parameters(): "
                    "Compression not RGB\n");
            switch (pVih->bmiHeader.biCompression) {
            case BI_RLE8:
                fprintf(stderr, "  (It is BI_RLE8)\n");
                break;
            case BI_RLE4:
                fprintf(stderr, "  (It is BI_RLE4)\n");
            case BI_BITFIELDS:
                fprintf(stderr, "  (It is BI_BITFIELDS)\n");
                break;
            default:
                fprintf(stderr, "  (Unknown compression type)\n");
            }
            return false;
        }
        int BytesPerPixel = pVih->bmiHeader.biBitCount / 8;
        if (BytesPerPixel != 3) {
            fprintf(stderr,
                    "directx_camera_server::open_and_find_parameters(): "
                    "Not 3 bytes per pixel (%d)\n",
                    pVih->bmiHeader.biBitCount);
            return false;
        }

        // A negative height indicates that the images are stored non-inverted
        // in Y
        // Not sure what to do with images that have negative height -- need to
        // read the book some more to find out.
        if (_num_rows < 0) {
            fprintf(stderr,
                    "directx_camera_server::open_and_find_parameters(): "
                    "Num Rows is negative (internal error)\n");
            return false;
        }

        // Find the stride to take when moving from one row of video to the
        // next.  This is rounded up to the nearest DWORD.
        _stride = (_num_columns * BytesPerPixel + 3) & ~3;
    }

    return true;
}

/// Construct but do not open a camera
directx_camera_server::directx_camera_server() {
    // No image in memory yet.
    _minX = _maxX = _minY = _maxY = 0;
}

/// Open nth available camera with specified resolution.
directx_camera_server::directx_camera_server(int which, unsigned width,
                                             unsigned height) {
    //---------------------------------------------------------------------
    if (!open_and_find_parameters(which, width, height)) {
#ifdef DXCAMSERVER_VERBOSE
        fprintf(stderr, "directx_camera_server::directx_camera_server(): "
                        "Cannot open camera\n");
#endif
        _status = false;
        return;
    }

    allocate_buffer();

#ifdef HACK_TO_REOPEN
    close_device();
#endif

    _status = true;
}

directx_camera_server::directx_camera_server(std::string const &pathPrefix,
                                             unsigned width, unsigned height) {
    //---------------------------------------------------------------------
    if (!open_and_find_parameters(pathPrefix, FilterOperation{}, width,
                                  height)) {

#ifdef DXCAMSERVER_VERBOSE
        fprintf(stderr, "directx_camera_server::directx_camera_server(): "
                        "Cannot open camera\n");
#endif
        _status = false;
        return;
    }
    allocate_buffer();

#ifdef HACK_TO_REOPEN
    close_device();
#endif

    _status = true;
}

directx_camera_server::directx_camera_server(
    std::string const &pathPrefix, FilterOperation const &sourceConfig) {
    if (!open_and_find_parameters(pathPrefix, sourceConfig)) {

#ifdef DXCAMSERVER_VERBOSE
        fprintf(stderr, "directx_camera_server::directx_camera_server(): "
                        "Cannot open camera\n");
#endif
        _status = false;
        return;
    }
    allocate_buffer();

#ifdef HACK_TO_REOPEN
    close_device();
#endif

    _status = true;
}
//---------------------------------------------------------------------
// Close the camera and the system.  Free up memory.

void directx_camera_server::close_device() {
    // Clean up.
    _pGraph.reset();
    _pMediaControl.reset();
    _pBuilder.reset();
}

directx_camera_server::~directx_camera_server() {
    // Get the callback device to immediately return all samples
    // it has queued up, then shut down the filter graph.
    if (_pSampleGrabberWrapper) {
        _pSampleGrabberWrapper->shutdown();
    }

    close_device();

    // Delete the callback object, so that it can clean up and
    // make sure all of its threads exit.
    _pSampleGrabberWrapper.reset();
}

bool directx_camera_server::read_image_to_memory(unsigned minX, unsigned maxX,
                                                 unsigned minY, unsigned maxY,
                                                 double exposure_millisecs) {
    if (!_status) {
        fprintf(stderr,
                "directx_camera_server::read_image_to_memory(): broken\n");
        return false;
    };

    //---------------------------------------------------------------------
    // In case we fail, clear these
    _minX = _minY = _maxX = _maxY = 0;

    //---------------------------------------------------------------------
    // If the maxes are greater than the mins, set them to the size of
    // the image.
    if (maxX < minX) {
        minX = 0;
        maxX = _num_columns - 1;
    }
    if (maxY < minY) {
        minY = 0;
        maxY = _num_rows - 1;
    }

    //---------------------------------------------------------------------
    // Clip collection range to the size of the sensor on the camera.
    if (minX < 0) {
        minX = 0;
    };
    if (minY < 0) {
        minY = 0;
    };
    if (maxX >= _num_columns) {
        maxX = _num_columns - 1;
    };
    if (maxY >= _num_rows) {
        maxY = _num_rows - 1;
    };

    //---------------------------------------------------------------------
    // Store image size for later use
    _minX = minX;
    _minY = minY;
    _maxX = maxX;
    _maxY = maxY;

    //---------------------------------------------------------------------
    // Set up and read one frame, if we can.
    if (!read_one_frame(_minX, _maxX, _minY, _maxY, (int)exposure_millisecs)) {
        fprintf(stderr, "directx_camera_server::read_image_to_memory(): "
                        "read_one_frame() failed\n");
        return false;
    }

    return true;
}

bool directx_camera_server::get_pixel_from_memory(unsigned X, unsigned Y,
                                                  vrpn_uint8 &val,
                                                  int RGB) const {
    if (!_status) {
        return false;
    };

    // Switch red and blue, since they are backwards in the record (blue is
    // first).
    RGB = 2 - RGB;

    // XXX This will depend on what kind of pixels we have!
    if ((_maxX <= _minX) || (_maxY <= _minY)) {
        fprintf(stderr, "directx_camera_server::get_pixel_from_memory(): No "
                        "image in memory\n");
        return false;
    }
    if ((X < _minX) || (X > _maxX) || (Y < _minY) || (Y > _maxY)) {
        return false;
    }
    unsigned cols = _num_columns;
    val = _buffer[(Y * cols + X) * 3 + RGB];
    return true;
}

bool directx_camera_server::get_pixel_from_memory(unsigned X, unsigned Y,
                                                  vrpn_uint16 &val,
                                                  int RGB) const {
    if (!_status) {
        return false;
    };

    // Switch red and blue, since they are backwards in the record (blue is
    // first).
    RGB = 2 - RGB;

    // XXX This will depend on what kind of pixels we have!
    if ((_maxX <= _minX) || (_maxY <= _minY)) {
        fprintf(stderr, "directx_camera_server::get_pixel_from_memory(): No "
                        "image in memory\n");
        return false;
    }
    if ((X < _minX) || (X > _maxX) || (Y < _minY) || (Y > _maxY)) {
        return false;
    }
    unsigned cols = _num_columns;
    val = _buffer[(Y * cols + X) * 3 + RGB];
    return true;
}

void directx_camera_server::allocate_buffer() {
    //---------------------------------------------------------------------
    // Allocate a buffer that is large enough to read the maximum-sized
    // image with no binning.
    /// @todo replace with a vector that gets sized here!
    auto buflen = (_num_rows * _num_columns * 3); // Expect B,G,R; 8-bits each.
    _buffer.resize(buflen);

    // No image in memory yet.
    _minX = _maxX = _minY = _maxY = 0;
}
