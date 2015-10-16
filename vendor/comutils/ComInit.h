/** @file
    @brief Header providing RAII-style handling of COM initialization.

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

#ifndef INCLUDED_ComInit_h_GUID_87AC03A7_3018_4A82_1716_BE38E2E9D1FC
#define INCLUDED_ComInit_h_GUID_87AC03A7_3018_4A82_1716_BE38E2E9D1FC

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
#include <memory>

namespace comutils {

class ComInit;

/// @brief unique_ptr for holding the lifetime of a ComInit.
using ComInstance = std::unique_ptr<ComInit>;

/// @brief Simple RAII class for handling COM initialization.
class ComInit {
  public:
    enum ThreadingModels { ApartmentThreading, Multithreading };
    ComInit(ThreadingModels model = Multithreading);
    ~ComInit();
    /// @brief Factory function returning a unique pointer representing a
    /// default initialization of COM.
    static ComInstance init() {
        auto ret = ComInstance{new ComInit};
        return ret;
    }

    /// @brief Factory function returning a unique pointer representing a
    /// "multithreading" initialization of COM (passing `COINIT_MULTITHREADED`
    /// to `CoInitializeEx`)
    static ComInstance initMultithreading() {
        auto ret = ComInstance{new ComInit{Multithreading}};
        return ret;
    }

    /// @brief Factory function returning a unique pointer representing an
    /// "apartment threading" initialization of COM (passing
    /// `COINIT_APARTMENTTHREADED` to `CoInitializeEx`)
    static ComInstance initApartmentThreading() {
        auto ret = ComInstance{new ComInit{ApartmentThreading}};
        return ret;
    }
    ComInit(ComInit const &) = delete;
    ComInit &operator=(ComInit const &) = delete;
};

} // namespace comutils

#endif // INCLUDED_ComInit_h_GUID_87AC03A7_3018_4A82_1716_BE38E2E9D1FC
