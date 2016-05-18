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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_ImagingInterface_h_GUID_E9CA5277_A0F5_488E_9E65_8541D437187C
#define INCLUDED_ImagingInterface_h_GUID_E9CA5277_A0F5_488E_9E65_8541D437187C

// Internal Includes
#include <osvr/PluginKit/DeviceInterface.h>
#include <osvr/PluginKit/ImagingInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/OpenCVTypeDispatch.h>
#include <osvr/Util/AlignedMemory.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
#include <iosfwd>
#include <cstring> // for std::memcpy

namespace osvr {
namespace pluginkit {
    /** @defgroup PluginKitCppImaging Imaging interface (C++)
        @brief Sending image reports from a device in your plugin.
        @ingroup PluginKit

        @{
    */

    /// @brief A class wrapping a cv::Mat representing a frame, as well as the
    /// sensor ID it corresponds to. Pass to
    /// osvr::pluginkit::DeviceToken::send() along with your
    /// osvr::pluginkit::ImagingInterface.
    class ImagingMessage {
      public:
        /// @brief Constructor, optionally taking a sensor number. Clones the
        /// supplied image/data as the imaging interface will take ownership of
        /// the given buffer and free it when done.
        ImagingMessage(cv::Mat const &frame, OSVR_ChannelCount sensor = 0)
            : m_sensor(sensor) {
            std::size_t bytes = frame.total() * frame.elemSize();
            m_buf = reinterpret_cast<OSVR_ImageBufferElement *>(
                util::alignedAlloc(bytes));
            std::memcpy(m_buf, frame.data, bytes);
            m_frame = cv::Mat(frame.size(), frame.type(), m_buf);
        }
        ~ImagingMessage() {
            util::alignedFree(m_buf);
            m_buf = NULL;
        }

        /// @brief Retrieves a reference to the cv::Mat object.
        cv::Mat const &getFrame() const { return m_frame; }

        /// @brief Retrieves the (cloned) buffer pointer.
        OSVR_ImageBufferElement *getBuf() const { return m_buf; }

        /// @brief Gets the sensor number.
        OSVR_ChannelCount getSensor() const { return m_sensor; }

      private:
        // noncopyable
        ImagingMessage(ImagingMessage const &);
        // nonassignable
        ImagingMessage &operator=(ImagingMessage const &);
        cv::Mat m_frame;
        OSVR_ImageBufferElement *m_buf;
        OSVR_ChannelCount m_sensor;
    };

    /// @brief A class wrapping an imaging interface for a device.
    class ImagingInterface {
      public:
        /// @brief Default constructor or constructor from an existing
        /// OSVR_ImagingDeviceInterface (assumed to be registered with an
        /// OSVR_DeviceInitOptions already)
        ImagingInterface(OSVR_ImagingDeviceInterface iface = NULL)
            : m_iface(iface) {}

        /// @brief Constructor that registers an imaging interface with the
        /// device init options object.
        explicit ImagingInterface(OSVR_DeviceInitOptions opts,
                                  OSVR_ChannelCount numSensors = 1) {
            OSVR_ReturnCode ret =
                osvrDeviceImagingConfigure(opts, &m_iface, numSensors);
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::logic_error("Could not initialize an Imaging "
                                       "Interface with the device options "
                                       "given!");
            }
        }

        /// @brief Send method - usually called by
        /// osvr::pluginkit::DeviceToken::send()
        void send(DeviceToken &dev, ImagingMessage const &message,
                  OSVR_TimeValue const &timestamp) {
            if (!m_iface) {
                throw std::logic_error(
                    "Must initialize the imaging interface before using it!");
            }
            cv::Mat const &frame(message.getFrame());
            util::NumberTypeData typedata =
                util::opencvNumberTypeData(frame.type());
            OSVR_ImagingMetadata metadata;
            metadata.channels = frame.channels();
            metadata.depth = static_cast<OSVR_ImageDepth>(typedata.getSize());
            metadata.width = frame.cols;
            metadata.height = frame.rows;
            metadata.type = typedata.isFloatingPoint()
                                ? OSVR_IVT_FLOATING_POINT
                                : (typedata.isSigned() ? OSVR_IVT_SIGNED_INT
                                                       : OSVR_IVT_UNSIGNED_INT);

            OSVR_ReturnCode ret = osvrDeviceImagingReportFrame(
                dev, m_iface, metadata, message.getBuf(), message.getSensor(),
                &timestamp);
            if (OSVR_RETURN_SUCCESS != ret) {
                throw std::runtime_error("Could not send imaging message!");
            }
        }

      private:
        OSVR_ImagingDeviceInterface m_iface;
    };
    /// @}
} // namespace pluginkit
} // namespace osvr

#endif // INCLUDED_ImagingInterface_h_GUID_E9CA5277_A0F5_488E_9E65_8541D437187C
