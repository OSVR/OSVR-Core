/** @file
    @brief Header declaring a C++11 `finally` or "scope-guard" construct.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
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

#ifndef INCLUDED_Finally_h_GUID_B9F9A283_5381_41A5_3D60_CEF0F8BA8606
#define INCLUDED_Finally_h_GUID_B9F9A283_5381_41A5_3D60_CEF0F8BA8606

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <utility>

namespace osvr {
namespace util {
    /// The class that provides the scope-guard behavior. Often not referred to
    /// by name because auto is useful here.
    template <typename F> class FinalTask {
      public:
        /// Explicit constructor from something callable.
        explicit FinalTask(F &&f) : f_(std::move(f)) {}
        /// Move constructor - cancels the moved-from task.
        FinalTask(FinalTask &&other) : f_(std::move(other.f_)), do_(other.do_) {
            other.cancel();
        }
        /// non-copyable
        FinalTask(FinalTask const &) = delete;
        /// non-assignable
        FinalTask &operator=(FinalTask const &) = delete;
        /// Destructor - if we haven't been cancelled, do our callable thing.
        ~FinalTask() {
            if (do_) {
                f_();
            }
        }
        /// Cancel causes us to not do our final task on destruction.
        void cancel() { do_ = false; }

      private:
        /// Our callable task to do at destruction.
        F f_;
        /// Whether we should actually do it.
        bool do_ = true;
    };

    /// Creation free function for final tasks. Use like:
    /// `auto f = finally([&]{ dothis(); });` to have `dothis()` called when `f`
    /// goes out of scope, no matter how.
    template <typename F> inline auto finally(F &&f) -> FinalTask<F> {
        return FinalTask<F>(std::forward<F>(f));
    }
} // namespace util
} // namespace osvr

#endif // INCLUDED_Finally_h_GUID_B9F9A283_5381_41A5_3D60_CEF0F8BA8606
