/** @file
    @brief Header including the full PluginKit C++ interface.

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

#ifndef INCLUDED_PluginKit_h_GUID_1470B278_99B7_4F84_F99B_34FD9B817C89
#define INCLUDED_PluginKit_h_GUID_1470B278_99B7_4F84_F99B_34FD9B817C89

// Internal Includes
#include <ogvr/PluginKit/DeviceInterface.h>
#include <ogvr/PluginKit/PluginRegistration.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace ogvr {

/** @brief Contains C++ header-only wrappers for the PluginKit C API.
    @ingroup PluginKit

    Use of these wrappers is optional: they duplicate functionality found in the
    C API as they are implemented in headers on top of the C API. However, they
    also provide a much cleaner, low-boilerplate interface for the most common
    operations.

    The free functions and the members of ogvr::pluginkit::PluginContext perform
    the same functionality. Use of the class can just lead to cleaner code if
    your whole plugin is "C++-style".
*/
namespace pluginkit {
    /// @brief C++ wrapper class for the opaque plugin context.
    ///
    /// This class provides no additional functionality beyond the free
    /// functions available in this namespace, it just provides syntactic
    /// sugar.
    class PluginContext {
      public:
        /// @brief Constructor of the context wrapper object
        PluginContext(OGVR_PluginRegContext ctx) : m_ctx(ctx) {}

        /// @brief Register a hardware poll callback
        /// @sa ::ogvr::pluginkit::registerHardwarePollCallback()
        template <typename T> void registerHardwarePollCallback(T functor) {
            ::ogvr::pluginkit::registerHardwarePollCallback(m_ctx, functor);
        }

        /// @brief Register the given object (assumed to be deletable by
        /// `delete`) to be deleted on plugin unload. (Transfers lifetime
        /// control to the plugin context)
        /// @sa ::ogvr::pluginkit::registerObjectForDeletion
        template <typename T> T *registerObjectForDeletion(T *obj) {
            return ::ogvr::pluginkit::registerObjectForDeletion(m_ctx, obj);
        }

      private:
        OGVR_PluginRegContext m_ctx;
    };

} // namespace pluginkit
} // namespace ogvr

#endif // INCLUDED_PluginKit_h_GUID_1470B278_99B7_4F84_F99B_34FD9B817C89
