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
#include <osvr/Util/WideToUTF8.h>
#include "directx_camera_server.h"
#include "WinVariant.h"

// Library/third-party includes
// - none

// Standard includes
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <iostream>

// Uncomment to get a full device name and path listing in enumeration, instead
// of silently enumerating and early-exiting when we find one we like.
//#define VERBOSE_ENUM

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

/// @brief Computes bytes required for an UNCOMPRESSED RGB DIB
inline DWORD dibsize(BITMAPINFOHEADER const &bi) {
    // cf:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/dd318229(v=vs.85).aspx
    auto stride = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) >> 3);
    return stride * std::abs(bi.biHeight);
}

ComInit::ComInit() {
    auto hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        throw std::runtime_error("Could not initialize COM!");
    }
}

ComInit::~ComInit() { CoUninitialize(); }

// This class is used to handle callbacks from the SampleGrabber filter.  It
// grabs each sample and holds onto it until the camera server that is
// associated with the object comes and gets it.  The callback method in this
// class is called in another thread, so its methods need to be guarded with
// semaphores.

class directx_samplegrabber_callback : public ISampleGrabberCB {
  public:
    directx_samplegrabber_callback(void);
    ~directx_samplegrabber_callback(void);

    void shutdown(void) {
        _stayAlive = false;
        Sleep(100);
    }

    // Boolean flag telling whether there is a sample in the image
    // buffer ready for the application thread to consume.  Set to
    // true by the callback when there is an image there, and back
    // to false by the application thread when it reads the image.
    // XXX This should be done using a semaphore to avoid having
    // to poll in the application.
    bool imageReady; //< true when there is an image ready to be processed

    // Boolean flag telling whether the app is done processing the image
    // buffer so that the callback thread can return it to the filter graph.
    // Set to true by the application when it finishes, and back
    // to false by the callback thread when it gets a new image.
    // XXX This should be done using a semaphore to avoid having
    // to poll in the callback thread.
    bool imageDone; //< true when the app has finished processing an image

    // A pointer to the image sample that has been passed to the sample
    // grabber callback handler.
    IMediaSample *imageSample;

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

  protected:
    BITMAPINFOHEADER _bitmapInfo; //< Describes format of the bitmap
    bool _stayAlive;              //< Tells all threads to exit
};

//-----------------------------------------------------------------------
// Helper functions for editing the filter graph:

static WinPtr<IPin> GetPin(IBaseFilter &pFilter, PIN_DIRECTION const PinDir) {
    auto pEnum = WinPtr<IEnumPins>{};
    pFilter.EnumPins(AttachPtr(pEnum));
    auto pPin = WinPtr<IPin>{};
    while (pEnum->Next(1, AttachPtr(pPin), nullptr) == S_OK) {
        PIN_DIRECTION PinDirThis;
        pPin->QueryDirection(&PinDirThis);
        if (PinDir == PinDirThis) {
            return pPin;
        }
    }
    return WinPtr<IPin>{};
}

static HRESULT ConnectTwoFilters(IGraphBuilder &pGraph, IBaseFilter &pFirst,
                                 IBaseFilter &pSecond) {
    auto pOut = GetPin(pFirst, PINDIR_OUTPUT);
    auto pIn = GetPin(pSecond, PINDIR_INPUT);
    return pGraph.Connect(pOut.get(), pIn.get());
}

//-----------------------------------------------------------------------

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
        // Set the grabber do not do one-shot mode because that would cause
        // it to stop the filter graph after a single frame is captured.
        _pGrabber->SetOneShot(FALSE);

        // Set the grabber to not do buffering mode, because we've not
        // implemented
        // the handler for buffered callbacks.
        _pGrabber->SetBufferSamples(FALSE);

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
                            "run filter graph\n");
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
    if (!_pCallback->imageReady) {
        for (int i = 0; i < TIMEOUT_MSECS; i++) {
            vrpn_SleepMsecs(1);
            if (_pCallback->imageReady) {
                break;
            } // Break out of the wait if its ready
        }
        if (!_pCallback->imageReady) {
#ifdef DEBUG
            fprintf(stderr, "directx_camera_server::read_one_frame(): Timeout "
                            "when reading image\n");
#endif
            return false;
        }
    }

    // If we are in mode 2, then we pause the graph after we captured one image.
    if (_mode == 2) {
        _pMediaControl->Pause();
        _mode = 1;
    }

    if (_pCallback->imageReady) {
        _pCallback->imageReady = false;
        if (FAILED(_pCallback->imageSample->GetPointer(&imageLocation))) {
            fprintf(
                stderr,
                "directx_camera_server::read_one_frame(): Can't get buffer\n");
            _status = false;
            _pCallback->imageDone = true;
            return false;
        }
        // Step through each line of the video and copy it into the buffer.  We
        // do one line at a time here because there can be padding at the end of
        // each line on some video formats.
        for (DWORD iRow = 0; iRow < _num_rows; iRow++) {
            memcpy(_buffer + _num_columns * 3 * iRow,
                   imageLocation + _stride * iRow, _num_columns * 3);
        }
        _pCallback->imageDone = true;
    }

#ifdef HACK_TO_REOPEN
    close_device();
#endif

// Capture timing information and print out how many frames per second
// are being received.

#if 0
  { static struct timeval last_print_time;
    struct timeval now;
    static bool first_time = true;
    static int frame_count = 0;

    if (first_time) {
      gettimeofday(&last_print_time, nullptr);
      first_time = false;
    } else {
      static	unsigned  last_r = 10000;
      frame_count++;
      gettimeofday(&now, nullptr);
      double timesecs = 0.001 * vrpn_TimevalMsecs(vrpn_TimevalDiff(now, last_print_time));
      if (timesecs >= 5) {
	double frames_per_sec = frame_count / timesecs;
	frame_count = 0;
	printf("Received frames per second = %lg\n", frames_per_sec);
	last_print_time = now;
      }
    }
  }
#endif

    return true;
}

inline std::string getProperty(IMoniker &mon, const wchar_t *propName) {

    auto pPropBag = WinPtr<IPropertyBag>{};
    mon.BindToStorage(nullptr, nullptr, IID_IPropertyBag, AttachPtr(pPropBag));

    auto ret = std::string{};
    if (!pPropBag) {
#ifdef DEBUG
        std::cerr << "Could not get the property bag" << std::endl;
#endif
        return ret;
    }
    auto val = WinVariant{};
    pPropBag->Read(propName, AttachVariant(val), nullptr);
    if (!contains<std::wstring>(val)) {
#ifdef DEBUG
        std::cerr << "!contains<std::wstring>(val) for property "
                  << osvr::util::wideToUTF8String(propName) << std::endl;
#endif
        return ret;
    }
    return osvr::util::wideToUTF8String(get<std::wstring>(val));
}

inline std::string getDevicePath(IMoniker &mon) {
    return getProperty(mon, L"DevicePath");
}

inline std::string getDeviceHumanDesc(IMoniker &mon) {
    auto desc = getProperty(mon, L"Description");
    if (desc.empty()) {
        desc = getProperty(mon, L"FriendlyName");
    }
    return desc;
}

bool directx_camera_server::start_com_and_graphbuilder() {
    HRESULT hr;
//-------------------------------------------------------------------
// Create COM and DirectX objects needed to access a video stream.

// Initialize COM.  This must have a matching uninitialize somewhere before
// the object is destroyed.
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "CoInitialize\n");
#endif

    _com = ComInit::init();
// Create the filter graph manager
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "CoCreateInstance FilterGraph\n");
#endif
    CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IGraphBuilder, AttachPtr(_pGraph));
    checkForConstructionError(_pGraph, "graph manager");

    _pGraph->QueryInterface(IID_IMediaControl, AttachPtr(_pMediaControl));
    _pGraph->QueryInterface(IID_IMediaEvent, AttachPtr(_pEvent));

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
inline WinPtr<IMoniker> find_first_capture_device_where(F &&f) {
    auto ret = WinPtr<IMoniker>{};
// Create the system device enumerator.
#ifdef DEBUG
    printf("find_first_capture_device_where(): Before "
           "CoCreateInstance SystemDeviceEnum\n");
#endif
    auto pDevEnum = WinPtr<ICreateDevEnum>{};
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
    auto pClassEnum = WinPtr<IEnumMoniker>{};
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
    auto pMoniker = WinPtr<IMoniker>{};
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

    if (!ret) {
        fprintf(stderr,
                "directx_camera_server find_first_capture_device_where(): "
                "No device satisfied the predicate.\n");
    }
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
    return open_moniker_and_finish_setup(pMoniker, width, height);
}

bool directx_camera_server::open_and_find_parameters(
    std::string const &pathPrefix, unsigned width, unsigned height) {
    auto graphbuilderRet = start_com_and_graphbuilder();
    if (!graphbuilderRet) {
        return false;
    }

    auto pMoniker =
        find_first_capture_device_where([&pathPrefix](IMoniker &mon) {
            auto path = getDevicePath(mon);
            if (path.length() < pathPrefix.length()) {
                return false;
            }
            return (path.substr(0, pathPrefix.length()) == pathPrefix);
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
    return open_moniker_and_finish_setup(pMoniker, width, height);
}

bool directx_camera_server::open_moniker_and_finish_setup(
    WinPtr<IMoniker> pMoniker, unsigned width, unsigned height) {

    if (!pMoniker) {
        fprintf(stderr,
                "directx_camera_server::open_moniker_and_finish_setup(): "
                "Null device moniker passed: no device found?\n");
        return false;
    }

    printf("directx_camera_server: Using capture device '%s' at path '%s'\n",
           getDeviceHumanDesc(*pMoniker).c_str(),
           getDevicePath(*pMoniker).c_str());

    // Bind the chosen moniker to a filter object.
    auto pSrc = WinPtr<IBaseFilter>{};
    pMoniker->BindToObject(0, 0, IID_IBaseFilter, AttachPtr(pSrc));

    //-------------------------------------------------------------------
    // Construct the sample grabber callback handler that will be used
    // to receive image data from the sample grabber.
    _pCallback.reset(new directx_samplegrabber_callback());

//-------------------------------------------------------------------
// Construct the sample grabber that will be used to snatch images from
// the video stream as they go by.  Set its media type and callback.

// Create the Sample Grabber.
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "CoCreateInstance SampleGrabber\n");
#endif
    CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IBaseFilter, AttachPtr(_pSampleGrabberFilter));
    checkForConstructionError(_pSampleGrabberFilter,
                              "SampleGrabber filter (not DirectX 8.1+?)");

#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "QueryInterface\n");
#endif
    _pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
                                          AttachPtr(_pGrabber));

// Set the media type to video
#ifdef DEBUG
    printf("directx_camera_server::open_and_find_parameters(): Before "
           "SetMediaType\n");
#endif
    AM_MEDIA_TYPE mt = {0};
    // Ask for video media producers that produce 8-bit RGB
    mt.majortype = MEDIATYPE_Video;  // Ask for video media producers
    mt.subtype = MEDIASUBTYPE_RGB24; // Ask for 8 bit RGB
    _pGrabber->SetMediaType(&mt);

    //-------------------------------------------------------------------
    // Ask for the video resolution that has been passed in.
    // This code is based on
    // intuiting that we need to use the SetFormat call on the IAMStreamConfig
    // interface; this interface is described in the help pages.
    // If the width and height are specified as 0, then they are not set
    // in the header, letting them use whatever is the default.
    if ((width != 0) && (height != 0)) {
        _pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
                                 pSrc.get(), IID_IAMStreamConfig,
                                 AttachPtr(_pStreamConfig));
        checkForConstructionError(_pStreamConfig, "StreamConfig interface");

        ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
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
        if (_pStreamConfig->SetFormat(&mt) != S_OK) {
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
           "CoCreateInstance nullptrRenderer\n");
#endif
    auto pNullRender = WinPtr<IBaseFilter>{};
    CoCreateInstance(CLSID_NullRenderer, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IBaseFilter, AttachPtr(pNullRender));

    //-------------------------------------------------------------------
    // Build the filter graph.  First add the filters and then connect them.

    // pSrc is the capture filter for the video device we found above.
    _pGraph->AddFilter(pSrc.get(), L"Video Capture");

    // Add the sample grabber filter
    _pGraph->AddFilter(_pSampleGrabberFilter.get(), L"SampleGrabber");

    // Add the null renderer filter
    _pGraph->AddFilter(pNullRender.get(), L"NullRenderer");

    // Connect the output of the video reader to the sample grabber input
    ConnectTwoFilters(*_pGraph, *pSrc, *_pSampleGrabberFilter);

    // Connect the output of the sample grabber to the null renderer input
    ConnectTwoFilters(*_pGraph, *_pSampleGrabberFilter, *pNullRender);

    //-------------------------------------------------------------------
    // XXX See if this is a video tuner card by querying for that interface.
    // Set it to read the video channel if it is one.
    auto pTuner = WinPtr<IAMTVTuner>{};
    auto hr = _pBuilder->FindInterface(nullptr, nullptr, pSrc.get(),
                                       IID_IAMTVTuner, AttachPtr(pTuner));
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

    //-------------------------------------------------------------------
    // Find _num_rows and _num_columns in the video stream.
    _pGrabber->GetConnectedMediaType(&mt);
    VIDEOINFOHEADER *pVih;
    if (mt.formattype == FORMAT_VideoInfo ||
        mt.formattype == FORMAT_VideoInfo2) {
        pVih = reinterpret_cast<VIDEOINFOHEADER *>(mt.pbFormat);
    } else {
        fprintf(stderr, "directx_camera_server::open_and_find_parameters(): "
                        "Can't get video header type\n");
        fprintf(stderr, "  (Expected %x or %x, got %x)\n", FORMAT_VideoInfo,
                FORMAT_VideoInfo2, mt.formattype);
        fprintf(stderr, "  (GetConnectedMediaType is not valid for DirectX "
                        "headers later than version 7)\n");
        fprintf(stderr, "  (We need to re-implement reading video in some "
                        "other interface)\n");
        return false;
    }

    // Number of rows and columns.  This is different if we are using a target
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
        fprintf(stderr, "directx_camera_server::open_and_find_parameters(): "
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
        fprintf(stderr, "directx_camera_server::open_and_find_parameters(): "
                        "Not 3 bytes per pixel (%d)\n",
                pVih->bmiHeader.biBitCount);
        return false;
    }

    // A negative height indicates that the images are stored non-inverted in Y
    // Not sure what to do with images that have negative height -- need to
    // read the book some more to find out.
    if (_num_rows < 0) {
        fprintf(stderr, "directx_camera_server::open_and_find_parameters(): "
                        "Num Rows is negative (internal error)\n");
        return false;
    }

    // Find the stride to take when moving from one row of video to the
    // next.  This is rounded up to the nearest DWORD.
    _stride = (_num_columns * BytesPerPixel + 3) & ~3;

    // Set the callback, where '0' means 'use the SampleCB callback'
    _pGrabber->SetCallback(_pCallback.get(), 0);

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
        fprintf(stderr, "directx_camera_server::directx_camera_server(): "
                        "Cannot open camera\n");
        _status = false;
        return;
    }

    //---------------------------------------------------------------------
    // Allocate a buffer that is large enough to read the maximum-sized
    // image with no binning.
    _buflen =
        (unsigned)(_num_rows * _num_columns * 3); // Expect B,G,R; 8-bits each.
    if ((_buffer = new unsigned char[_buflen]) == nullptr) {
        fprintf(stderr, "directx_camera_server::directx_camera_server(): Out "
                        "of memory for buffer\n");
        _status = false;
        return;
    }
    // No image in memory yet.
    _minX = _maxX = _minY = _maxY = 0;

#ifdef HACK_TO_REOPEN
    close_device();
#endif

    _status = true;
}

directx_camera_server::directx_camera_server(std::string const &pathPrefix,
                                             unsigned width, unsigned height) {
    //---------------------------------------------------------------------
    if (!open_and_find_parameters(pathPrefix, width, height)) {
        fprintf(stderr, "directx_camera_server::directx_camera_server(): "
                        "Cannot open camera\n");
        _status = false;
        return;
    }

    //---------------------------------------------------------------------
    // Allocate a buffer that is large enough to read the maximum-sized
    // image with no binning.
    /// @todo replace with a vector that gets sized here!
    _buflen =
        (unsigned)(_num_rows * _num_columns * 3); // Expect B,G,R; 8-bits each.
    if ((_buffer = new unsigned char[_buflen]) == nullptr) {
        fprintf(stderr, "directx_camera_server::directx_camera_server(): Out "
                        "of memory for buffer\n");
        _status = false;
        return;
    }
    // No image in memory yet.
    _minX = _maxX = _minY = _maxY = 0;

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
    _pEvent.reset();
    _pBuilder.reset();
    _pSampleGrabberFilter.reset();
    _pGrabber.reset();
    _pStreamConfig.reset();
    _com.reset();
}

directx_camera_server::~directx_camera_server() {
    // Get the callback device to immediately return all samples
    // it has queued up, then shut down the filter graph.
    if (_pCallback) {
        _pCallback->shutdown();
    }
    close_device();

    if (_buffer != nullptr) {
        delete[] _buffer;
    }

    // Delete the callback object, so that it can clean up and
    // make sure all of its threads exit.
    _pCallback.reset();
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

//--------------------------------------------------------------------------------------------
// This section implements the callback handler that gets frames from the
// SampleGrabber filter.

directx_samplegrabber_callback::directx_samplegrabber_callback()
    : imageReady(false), imageDone(false), imageSample(nullptr),
      _stayAlive(true) {}

directx_samplegrabber_callback::~directx_samplegrabber_callback() {
    // Make sure the other thread knows that it is okay to return the
    // buffer and wait until it has had time to do so.
    _stayAlive = false;
    Sleep(100);
};

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
