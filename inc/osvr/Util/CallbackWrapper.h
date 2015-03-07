/** @file
    @brief Header providing a templated functor wrapping the holding and calling
   of function pointer and userdata callbacks.

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

#ifndef INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
#define INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED

// Internal Includes
// - none

// Library/third-party includes
#include <boost/type_traits/function_traits.hpp>
#include <boost/type_traits/remove_pointer.hpp>

// Standard includes
#include <type_traits>
#include <utility>

namespace osvr {
namespace util {
    /// @brief A class template turning a callback with some number of
    /// arguments,
    /// with a userdata pointer last, into a function object.
    template <typename FunctionPtrType> class CallbackWrapper {
      public:
        /// @brief Constructor from function pointer and user data pointer.
        CallbackWrapper(FunctionPtrType f, void *userData)
            : m_f(f), m_ud(userData) {}

        /// @brief Function type (remove pointer - computed)
        typedef
            typename boost::remove_pointer<FunctionPtrType>::type FunctionType;

        /// @brief Return type of the function (computed)
        typedef typename boost::function_traits<FunctionType>::result_type
            ReturnType;

        /// @brief Function call operator with non-void return
        template <typename... Args>
        ReturnType operator()(Args &&... args) const {
            return (*m_f)(std::forward<Args>(args)..., m_ud);
        }

      private:
        FunctionPtrType m_f;
        void *m_ud;
    };
} // namespace util
} // namespace osvr
#endif // INCLUDED_CallbackWrapper_h_GUID_6169ADE2_5BA1_4A81_47C9_9E492F6405ED
