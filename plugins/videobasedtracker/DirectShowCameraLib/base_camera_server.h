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

#ifndef INCLUDED_base_camera_server_h_GUID_AE8BD56F_793F_4693_9B94_4E4CC953511C
#define INCLUDED_base_camera_server_h_GUID_AE8BD56F_793F_4693_9B94_4E4CC953511C

// Internal Includes
#include <vrpn_Shared.h>

// Library/third-party includes
// - none

// Standard includes
#include <stdio.h> // For NULL
#include <math.h>  // For floor()
#include <string>
#include <vector>

//----------------------------------------------------------------------------
// This class forms a basic wrapper for an image.  It treats an image as
// anything
// which can support requests on the number of pixels in an image and can
// perform queries by pixel coordinate in that image.  It is a set of functions
// that are needed by the com_osvr_VideoBasedHMDTracker.
// @todo If we template this class based on the type of pixel it has, we may be
// able to speed things up quite a bit for particular cases of interest.  Even
// better may be to implement functions that wrap the functions we need to
// have be fast (like writing to a GL_LUMINANCE OpenGL texture).

class image_wrapper {
  public:
    // Virtual destructor so that children can de-allocate space as needed.
    virtual ~image_wrapper(){};

    // Tell what the range is for the image.
    virtual void read_range(int &minx, int &maxx, int &miny,
                            int &maxy) const = 0;

    /// Return the number of colors that the image has
    virtual unsigned get_num_colors() const = 0;
    // XXX These should return the maximum number of possible rows/columns,
    // like the ones in the camera server do.
    virtual unsigned get_num_rows(void) const {
        int _minx, _maxx, _miny, _maxy;
        read_range(_minx, _maxx, _miny, _maxy);
        return _maxy - _miny + 1;
    }
    virtual unsigned get_num_columns(void) const {
        int _minx, _maxx, _miny, _maxy;
        read_range(_minx, _maxx, _miny, _maxy);
        return _maxx - _minx + 1;
    }

    /// Read a pixel from the image into a double; return true if the pixel
    // was in the image, false if it was not.
    virtual bool read_pixel(int x, int y, double &result,
                            unsigned rgb = 0) const = 0;

    // Overloaded by result type to enable optimization later but use by any.
    virtual bool read_pixel(int x, int y, vrpn_uint8 &result,
                            unsigned rgb = 0) const {
        double double_pix;
        bool err = read_pixel(x, y, double_pix, rgb);
        result = static_cast<vrpn_uint8>(double_pix);
        return err;
    }
    virtual bool read_pixel(int x, int y, vrpn_uint16 &result,
                            unsigned rgb = 0) const {
        double double_pix;
        bool err = read_pixel(x, y, double_pix, rgb);
        result = static_cast<vrpn_uint16>(double_pix);
        return err;
    }

    /// Read a pixel from the image into a double; Don't check boundaries.
    virtual double read_pixel_nocheck(int x, int y, unsigned rgb = 0) const = 0;

    // Do bilinear interpolation to read from the image, in order to
    // smoothly interpolate between pixel values.
    // All sorts of speed tweaks in here because it is in the inner loop for
    // the spot tracker and other codes.
    // Return a result of zero and false if the coordinate its outside the
    // image.
    // Return the correct interpolated result and true if the coordinate is
    // inside.
    inline bool read_pixel_bilerp(double x, double y, double &result,
                                  unsigned rgb = 0) const {
        result = 0; // In case of failure.
        // The order of the following statements is optimized for speed.
        // The double version is used below for xlowfrac comp, ixlow also used
        // later.
        // Slightly faster to explicitly compute both here to keep the answer
        // around.
        double xlow = floor(x);
        int ixlow = (int)xlow;
        // The double version is used below for ylowfrac comp, ixlow also used
        // later
        // Slightly faster to explicitly compute both here to keep the answer
        // around.
        double ylow = floor(y);
        int iylow = (int)ylow;
        int ixhigh = ixlow + 1;
        int iyhigh = iylow + 1;
        double xhighfrac = x - xlow;
        double yhighfrac = y - ylow;
        double xlowfrac = 1.0 - xhighfrac;
        double ylowfrac = 1.0 - yhighfrac;
        double ll, lh, hl, hh;

        // Combining the if statements into one using || makes it slightly
        // slower.
        // Interleaving the result calculation with the returns makes it slower.
        if (!read_pixel(ixlow, iylow, ll, rgb)) {
            return false;
        }
        if (!read_pixel(ixlow, iyhigh, lh, rgb)) {
            return false;
        }
        if (!read_pixel(ixhigh, iylow, hl, rgb)) {
            return false;
        }
        if (!read_pixel(ixhigh, iyhigh, hh, rgb)) {
            return false;
        }
        result = ll * xlowfrac * ylowfrac + lh * xlowfrac * yhighfrac +
                 hl * xhighfrac * ylowfrac + hh * xhighfrac * yhighfrac;
        return true;
    };

    // Do bilinear interpolation to read from the image, in order to
    // smoothly interpolate between pixel values.
    // All sorts of speed tweaks in here because it is in the inner loop for
    // the spot tracker and other codes.
    // Does not check boundaries to make sure they are inside the image.
    inline double read_pixel_bilerp_nocheck(double x, double y,
                                            unsigned rgb = 0) const {
        // The order of the following statements is optimized for speed.
        // The double version is used below for xlowfrac comp, ixlow also used
        // later.
        // Slightly faster to explicitly compute both here to keep the answer
        // around.
        double xlow = floor(x);
        int ixlow = (int)xlow;
        // The double version is used below for ylowfrac comp, ixlow also used
        // later
        // Slightly faster to explicitly compute both here to keep the answer
        // around.
        double ylow = floor(y);
        int iylow = (int)ylow;
        int ixhigh = ixlow + 1;
        int iyhigh = iylow + 1;
        double xhighfrac = x - xlow;
        double yhighfrac = y - ylow;
        double xlowfrac = 1.0 - xhighfrac;
        double ylowfrac = 1.0 - yhighfrac;

        // Combining the if statements into one using || makes it slightly
        // slower.
        // Interleaving the result calculation with the returns makes it slower.
        return read_pixel_nocheck(ixlow, iylow, rgb) * xlowfrac * ylowfrac +
               read_pixel_nocheck(ixlow, iyhigh, rgb) * xlowfrac * yhighfrac +
               read_pixel_nocheck(ixhigh, iylow, rgb) * xhighfrac * ylowfrac +
               read_pixel_nocheck(ixhigh, iyhigh, rgb) * xhighfrac * yhighfrac;
    };

  protected:
};

//----------------------------------------------------------------------------
// This class forms a basic wrapper for a camera.  It treats an camera as
// anything which can do what is needed for an imager and also includes
// functions for reading images into memory from the camera.

class base_camera_server : public image_wrapper {
  public:
    virtual ~base_camera_server(void){};

    /// Is the camera working properly?
    bool working(void) const { return _status; };

    // These functions should be used to determine the stride in the
    // image when skipping lines.  They are in terms of the full-screen
    // number of pixels with the current binning level.
    unsigned get_num_rows(void) const { return _num_rows / _binning; };
    unsigned get_num_columns(void) const { return _num_columns / _binning; };

    /// Read an image to a memory buffer.  Max < min means "whole range".
    /// Setting binning > 1 packs more camera pixels into each image pixel, so
    /// the maximum number of pixels has to be divided by the binning constant
    /// (the effective number of pixels is lower and pixel coordinates are set
    /// in this reduced-count pixel space).  This routine returns false if a
    /// new image could not be read (for example, because of a timeout on
    /// the reading because we're at the end of a video stream).
    virtual bool read_image_to_memory(unsigned minX = 255, unsigned maxX = 0,
                                      unsigned minY = 255, unsigned maxY = 0,
                                      double exposure_time = 250.0) = 0;

    /// Get pixels out of the memory buffer, RGB indexes the colors
    virtual bool get_pixel_from_memory(unsigned X, unsigned Y, vrpn_uint8 &val,
                                       int RGB = 0) const = 0;
    virtual bool get_pixel_from_memory(unsigned X, unsigned Y, vrpn_uint16 &val,
                                       int RGB = 0) const = 0;

    // Makes the read routines in the base class faster by calling the above
    // methods.
    virtual bool read_pixel(int x, int y, vrpn_uint8 &result,
                            unsigned rgb = 0) const {
        return get_pixel_from_memory(x, y, result, rgb);
    }
    virtual bool read_pixel(int x, int y, vrpn_uint16 &result,
                            unsigned rgb = 0) const {
        return get_pixel_from_memory(x, y, result, rgb);
    }

    /// Read a pixel from the image into a double; return true if the pixel
    // was in the image, false if it was not.
    virtual bool read_pixel(int x, int y, double &result,
                            unsigned rgb = 0) const {
        vrpn_uint16 val = 0;
        result = 0.0; // Until we get a better one
        if (get_pixel_from_memory(x, y, val, rgb)) {
            result = val;
            return true;
        } else {
            return false;
        }
    };

    /// Read a pixel from the image into a double; Don't check boundaries.
    virtual double read_pixel_nocheck(int x, int y, unsigned rgb = 0) const {
        vrpn_uint16 val = 0;
        get_pixel_from_memory(x, y, val, rgb);
        return val;
    };

    /// Instantiation needed for image_wrapper
    virtual void read_range(int &minx, int &maxx, int &miny, int &maxy) const {
        minx = _minX;
        miny = _minY;
        maxx = _maxX;
        maxy = _maxY;
    }

  protected:
    bool _status;                     //< True is working, false is not
    unsigned _num_rows, _num_columns; //< Size of the memory buffer
    unsigned _minX = 0;
    unsigned _minY = 0;
    unsigned _maxX = 0;
    unsigned _maxY = 0; //< Region of the image in memory
    unsigned _binning;  //< How many camera pixels compressed into image pixel

    virtual bool open_and_find_parameters(void) { return false; };
    base_camera_server(unsigned binning = 1) {
        _binning = binning;
        if (_binning < 1) {
            _binning = 1;
        }
    };
};

#endif // INCLUDED_base_camera_server_h_GUID_AE8BD56F_793F_4693_9B94_4E4CC953511C
