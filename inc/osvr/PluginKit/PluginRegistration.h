/** @file
    @brief Header providing C++ interface wrappers around functionality in
    PluginRegistrationC.h

    @date 2014

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>

*/

// Copyright 2014 Sensics, Inc.
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

#ifndef INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
#define INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9

// Internal Includes
#include <osvr/PluginKit/PluginRegistrationC.h>
#include <osvr/Util/GenericDeleter.h>
#include <osvr/Util/GenericCaller.h>
#include <osvr/Util/BoostIsCopyConstructible.h>

// Library/third-party includes
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/static_assert.hpp>

// Standard includes
#include <cstddef>
#include <string>
#include <stdexcept>

namespace osvr {
namespace pluginkit {
    /** @defgroup PluginKitCppRegistration Plugin Registration (C++ wrappers)
        @brief How to start writing a plugin and advertise your capabilities to
       the core library.
        @ingroup PluginKit

        @{
    */
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
    /// Internally uses osvrPluginRegisterDataWithDeleteCallback()
    template <typename T>
    inline T *registerObjectForDeletion(OSVR_PluginRegContext ctx, T *obj) {
        OSVR_ReturnCode ret = osvrPluginRegisterDataWithDeleteCallback(
            ctx, &::osvr::util::generic_deleter<T>, static_cast<void *>(obj));
        if (ret != OSVR_RETURN_SUCCESS) {
            throw std::runtime_error("registerObjectForDeletion failed!");
        }
        return obj;
    }

#ifndef OSVR_DOXYGEN_EXTERNAL
    /// @brief Internal implementation-only namespace
    namespace detail {
        /// @brief Traits-based overload to register a hardware detect callback
        /// where we're given a pointer to a function object.
        template <typename T>
        inline OSVR_ReturnCode registerHardwareDetectCallbackImpl(
            OSVR_PluginRegContext ctx, T functor,
            typename boost::enable_if<boost::is_pointer<T> >::type * = NULL) {
            typedef typename boost::remove_pointer<T>::type FunctorType;
            registerObjectForDeletion(ctx, functor);
            return osvrPluginRegisterHardwareDetectCallback(
                ctx, &util::GenericCaller<OSVR_HardwareDetectCallback,
                                          FunctorType, util::this_last_t>::call,
                static_cast<void *>(functor));
        }

        /// @brief Traits based overload to copy a hardware detect callback
        /// passed by value then register the copy.
        template <typename T>
        inline OSVR_ReturnCode registerHardwareDetectCallbackImpl(
            OSVR_PluginRegContext ctx, T functor,
            typename boost::disable_if<boost::is_pointer<T> >::type * = NULL) {
#ifdef OSVR_HAVE_BOOST_IS_COPY_CONSTRUCTIBLE
            BOOST_STATIC_ASSERT_MSG(boost::is_copy_constructible<T>::value,
                                    "Hardware detect callback functors must be "
                                    "either passed as a pointer or be "
                                    "copy-constructible");
#endif
            T *functorCopy = new T(functor);
            return registerHardwareDetectCallbackImpl(ctx, functorCopy);
        }

        /// @brief Traits-based overload to register an instantiation callback
        /// where we're given a pointer to a function object.
        template <typename T>
        inline OSVR_ReturnCode registerDriverInstantiationCallbackImpl(
            OSVR_PluginRegContext ctx, const char driverName[], T functor,
            typename boost::enable_if<boost::is_pointer<T> >::type * = NULL) {
            typedef typename boost::remove_pointer<T>::type FunctorType;
            registerObjectForDeletion(ctx, functor);
            return osvrRegisterDriverInstantiationCallback(
                ctx, driverName,
                &util::GenericCaller<OSVR_DriverInstantiationCallback,
                                     FunctorType, util::this_last_t>::call,
                static_cast<void *>(functor));
        }
        /// @brief Traits based overload to copy an instantiation callback
        /// passed by value then register the copy.
        template <typename T>
        inline OSVR_ReturnCode registerDriverInstantiationCallbackImpl(
            OSVR_PluginRegContext ctx, const char driverName[], T functor,
            typename boost::disable_if<boost::is_pointer<T> >::type * = NULL) {
#ifdef OSVR_HAVE_BOOST_IS_COPY_CONSTRUCTIBLE
            BOOST_STATIC_ASSERT_MSG(
                boost::is_copy_constructible<T>::value,
                "Driver instantiation callback functors must be "
                "either passed as a pointer or be "
                "copy-constructible");
#endif
            T *functorCopy = new T(functor);
            return registerDriverInstantiationCallbackImpl(ctx, driverName,
                                                           functorCopy);
        }
    } // namespace detail
#endif

    /// @brief Registers a function object to be called when the core requests a
    /// hardware detection.
    ///
    /// Your callback should take one parameter of type
    /// ::OSVR_PluginRegContext and return a value of type ::OSVR_ReturnCode
    ///
    /// Also provides for deletion of the function object.
    ///
    /// @param ctx The registration context passed to your entry point.
    /// @param functor An function object (with operator() defined). Pass either
    /// a pointer, which will transfer ownership, or an object by value, which
    /// will result in a copy being made.
    ///
    /// @sa PluginContext::registerHardwareDetectCallback
    template <typename T>
    inline void registerHardwareDetectCallback(OSVR_PluginRegContext ctx,
                                               T functor) {
        OSVR_ReturnCode ret =
            detail::registerHardwareDetectCallbackImpl(ctx, functor);
        if (ret != OSVR_RETURN_SUCCESS) {
            throw std::runtime_error("registerHardwareDetectCallback failed!");
        }
    }

    /// @brief Registers a function object to be called when the server is told
    /// to instantiate a driver by name with parameters.
    ///
    /// Your callback should take a parameter of type
    /// ::OSVR_PluginRegContext and a parameter of type const char * (the
    /// JSON parameters as a string) and return a value of type
    /// ::OSVR_ReturnCode
    ///
    /// Also provides for deletion of the function object.
    ///
    /// @param ctx The registration context passed to your entry point.
    /// @param driverName The driver name you're advertising.
    /// @param functor An function object (with operator() defined). Pass either
    /// a pointer, which will transfer ownership, or an object by value, which
    /// will result in a copy being made.
    ///
    /// @sa PluginContext::registerDriverInstantiationCallback
    template <typename T>
    inline void registerDriverInstantiationCallback(OSVR_PluginRegContext ctx,
                                                    const char driverName[],
                                                    T functor) {
        OSVR_ReturnCode ret = detail::registerDriverInstantiationCallbackImpl(
            ctx, driverName, functor);
        if (ret != OSVR_RETURN_SUCCESS) {
            throw std::runtime_error(
                "registerDriverInstantiationCallback failed!");
        }
    }
    /// @}
} // namespace pluginkit
} // namespace osvr

#endif // INCLUDED_PluginRegistration_h_GUID_4F5D6422_2977_40A9_8BA0_F86FD6245CE9
