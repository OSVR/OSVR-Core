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

#ifndef INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
#define INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9

// Internal Includes
#include <ogvr/PluginKit/PluginRegistrationC.h>
#include <ogvr/Util/GenericDeleter.h>

// Library/third-party includes
// - none

// Standard includes
#include <cstddef>
#include <string>
#include <stdexcept>

namespace ogvr {
namespace plugin {
    /// @brief Registers an object to be destroyed with delete when the plugin
    /// is unloaded.
    ///
    /// The template parameter does not need to be specified - it is deduced
    /// automatically.
    ///
    /// The plugin remains nominally the owner of the object, but your code
    /// should no longer concern itself with the destruction of this object.
    /// Thus, if you have it in a smart pointer of any kind, release it from
    /// that pointer when you pass it to your function.
    ///
    /// @param ctx The registration context passed to your entry point.
    /// @param obj A pointer to your object to register for deletion.
    ///
    /// Internally uses ogvrPluginRegisterDataWithDeleteCallback()
    template <typename T>
    inline T *registerObjectForDeletion(OGVR_PluginRegContext ctx, T *obj) {
        OGVR_ReturnCode ret = ogvrPluginRegisterDataWithDeleteCallback(
            ctx, &::ogvr::detail::generic_deleter<T>, static_cast<void *>(obj));
        if (ret != OGVR_RETURN_SUCCESS) {
            throw std::runtime_error("registerObjectForDeletion failed!");
        }
        return obj;
    }

    namespace detail {
        template <typename T>
        inline OGVR_ReturnCode
        doHardwarePollRegistration(OGVR_PluginRegContext ctx, T *functor) {
            registerObjectForDeletion(ctx, functor);
            return ogvrPluginRegisterHardwarePollCallback(
				ctx, &::ogvr::detail::generic_caller0<OGVR_ReturnCode, T>,
                static_cast<void *>(functor));
        }

		/// Template partial specializations for pointer vs value check.
        template <typename T>
        struct RegisterHardwarePollCallback_RegisterHeapFunctor {
            static OGVR_ReturnCode run(OGVR_PluginRegContext ctx,
                                       T functorByValue) {
                T *functorCopy = new T(functorByValue);
                return doHardwarePollRegistration(ctx, functorCopy);
            }
        };

        template <typename T>
        struct RegisterHardwarePollCallback_RegisterHeapFunctor<T *> {
            static OGVR_ReturnCode run(OGVR_PluginRegContext ctx,
                                       T *functorByPointer) {
                return doHardwarePollRegistration(ctx, functorByPointer);
            }
        };
    }
    /// @brief Registers a function object to be called when the core requests a
    /// hardware poll.
    ///
    /// Also provides for deletion of the function object.
    template <typename T>
    inline void registerHardwarePollCallback(OGVR_PluginRegContext ctx,
                                             T functor) {
        OGVR_ReturnCode ret =
            detail::RegisterHardwarePollCallback_RegisterHeapFunctor<T>::run(
                ctx, functor);
        if (ret != OGVR_RETURN_SUCCESS) {
            throw std::runtime_error("registerHardwarePollCallback failed!");
        }
    }
} // end of namespace plugin
} // end of namespace ogvr

#endif // INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
