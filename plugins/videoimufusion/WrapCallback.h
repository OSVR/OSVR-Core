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
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_WrapCallback_h_GUID_4C52D585_7A4B_49BA_D069_B1F3B42E9896
#define INCLUDED_WrapCallback_h_GUID_4C52D585_7A4B_49BA_D069_B1F3B42E9896

// Internal Includes
#include <osvr/Util/TimeValue.h>

// Library/third-party includes
// - none

// Standard includes
#include <functional>
#include <memory>

namespace osvr {
namespace pluginkit {
    namespace detail {
        template <typename ReportType>
        using WrappedCallbackFunction =
            std::function<void(const OSVR_TimeValue *, const ReportType *)>;

        template <typename ReportType>
        using WrappedCallbackPtr =
            std::unique_ptr<WrappedCallbackFunction<ReportType>>;
        template <typename ReportType>
        void callbackCaller(void *userdata, const OSVR_TimeValue *timestamp,
                            const ReportType *report) {
            auto &f =
                *static_cast<WrappedCallbackFunction<ReportType> *>(userdata);
            f(timestamp, report);
        }
        template <typename ReportType> struct CallbackType_impl {
            typedef void (*type)(void *, const OSVR_TimeValue *,
                                 const ReportType *);
        };
        template <typename ReportType>
        using CallbackType = typename CallbackType_impl<ReportType>::type;

        template <typename ReportType>
        inline CallbackType<ReportType> getCaller() {
            return &callbackCaller<ReportType>;
        }

        template <typename ReportType, typename F>
        inline std::pair<CallbackType<ReportType>,
                         WrappedCallbackPtr<ReportType>>
        wrapCallback(F &&f) {
            auto functor = WrappedCallbackPtr<ReportType>{
                new WrappedCallbackFunction<ReportType>{std::forward<F>(f)}};
            return std::make_pair(getCaller<ReportType>(), std::move(functor));
        }
    } // namespace detail
    using detail::wrapCallback;
    using detail::WrappedCallbackPtr;
} // namespace pluginkit
} // namespace osvr

#endif // INCLUDED_WrapCallback_h_GUID_4C52D585_7A4B_49BA_D069_B1F3B42E9896
