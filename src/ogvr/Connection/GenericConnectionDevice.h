/** @file
    @brief Header

    @date 2014

    @author
    Ryan Pavlik
    <ryan@sensics.com>
    <http://sensics.com>
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
#include <ogvr/Connection/ConnectionDevice.h>

#include <ogvr/Util/ReturnCodesC.h>

// Library/third-party includes
#include <boost/assert.hpp>

// Standard includes
#include <functional>

namespace ogvr {
namespace connection {
    /// @brief ConnectionDevice implementation for advanced devices
    class GenericConnectionDevice : public ConnectionDevice {
      public:
        GenericConnectionDevice(std::string const &name,
                                std::function<OGVR_ReturnCode()> update)
            : ConnectionDevice(name), m_update(update) {}
        virtual ~GenericConnectionDevice() {}
        virtual void m_process() { m_update(); }
        virtual void m_sendData(util::time::TimeValue const &, MessageType *,
                                const char *, size_t) {
            BOOST_ASSERT_MSG(false, "Never called!");
        }

      private:
        std::function<OGVR_ReturnCode()> m_update;
    };
} // namespace connection
} // namespace ogvr

#endif // INCLUDED_GenericConnectionDevice_h_GUID_411E730D_4ACA_4385_77C9_05A84863E786
