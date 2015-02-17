/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_ImagingInterface_h_GUID_E9CA5277_A0F5_488E_9E65_8541D437187C
#define INCLUDED_ImagingInterface_h_GUID_E9CA5277_A0F5_488E_9E65_8541D437187C

// Internal Includes
#include <osvr/PluginKit/DeviceInterface.h>
#include <osvr/PluginKit/ImagingInterfaceC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Util/OpenCVTypeDispatch.h>

// Library/third-party includes
#include <opencv2/core/core_c.h>
#include <opencv2/core/core.hpp>

// Standard includes
#include <iosfwd>

namespace osvr {
namespace pluginkit {
    /** @defgroup PluginKitCppImaging Imaging interface (C++)
        @brief Sending image reports from a device in your plugin.
        @ingroup PluginKit

        @{
    */

    class ImagingMessage {
      public:
        ImagingMessage(cv::InputArray frame, OSVR_ChannelCount sensor = 0)
            : m_sensor(sensor) {
            m_frame.create(frame.size(), frame.type());
            m_frame = frame.getMat().clone();
        }

        void dump(std::ostream &os) {
            os << m_frame.size() << ", ";
            util::NumberTypeData typedata =
                util::opencvNumberTypeData(m_frame.type());
            os << m_frame.channels() << " channels, ";
            os << typedata.getSize() * 8 << " bit ";
            if (typedata.isFloatingPoint()) {
                os << "floating-point";
            } else {
                os << (typedata.isSigned() ? "signed integer"
                                           : "unsigned integer");
            }
        }

        cv::Mat const &getFrame() const { return m_frame; }

        OSVR_ChannelCount getSensor() const { return m_sensor; }

      private:
        cv::Mat m_frame;
        OSVR_ChannelCount m_sensor;
    };

    class ImagingInterface {
      public:
        ImagingInterface(OSVR_ImagingDeviceInterface iface = NULL)
            : m_iface(iface) {}
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
            metadata.depth = typedata.getSize();
            metadata.width = frame.size[0];
            metadata.height = frame.size[1];
            metadata.type = typedata.isFloatingPoint()
                                ? OSVR_IVT_FLOATING_POINT
                                : (typedata.isSigned() ? OSVR_IVT_SIGNED_INT
                                                       : OSVR_IVT_UNSIGNED_INT);

            OSVR_ReturnCode ret =
                osvrDeviceImagingReportFrame(dev, m_iface, metadata, frame.data,
                                             message.getSensor(), &timestamp);
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
