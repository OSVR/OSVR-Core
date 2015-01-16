/** @file
    @brief Header

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2014 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_GenericConnectionDevice_h_GUID_411E730D_4ACA_4385_77C9_05A84863E786
#define INCLUDED_GenericConnectionDevice_h_GUID_411E730D_4ACA_4385_77C9_05A84863E786

// Internal Includes
#include <osvr/Connection/ConnectionDevice.h>

#include <osvr/Util/ReturnCodesC.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <functional>

namespace osvr {
namespace connection {
    /// @brief ConnectionDevice implementation for advanced devices
    class GenericConnectionDevice : public ConnectionDevice {
      public:
        GenericConnectionDevice(std::string const &name,
                                std::function<OSVR_ReturnCode()> update)
            : ConnectionDevice(name), m_update(update) {}

        GenericConnectionDevice(ConnectionDevice::NameList const &names,
                                std::function<OSVR_ReturnCode()> update)
            : ConnectionDevice(names), m_update(update) {}

        virtual ~GenericConnectionDevice() {}
        virtual void m_process() { m_update(); }
        virtual void m_sendData(util::time::TimeValue const &, MessageType *,
                                const char *, size_t) {
            BOOST_ASSERT_MSG(false, "Never called!");
        }

      private:
        std::function<OSVR_ReturnCode()> m_update;
    };
} // namespace connection
} // namespace osvr

#endif // INCLUDED_GenericConnectionDevice_h_GUID_411E730D_4ACA_4385_77C9_05A84863E786
