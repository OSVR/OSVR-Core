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

#ifndef INCLUDED_Viewers_h_GUID_64108A5C_E8CF_409D_0757_C0A142678C5A
#define INCLUDED_Viewers_h_GUID_64108A5C_E8CF_409D_0757_C0A142678C5A

// Internal Includes
#include <osvr/Client/Export.h>
#include <osvr/Util/ClientOpaqueTypesC.h>
#include <osvr/Util/ChannelCountC.h>
#include <osvr/Client/Viewer.h>
#include <osvr/Client/InternalInterfaceOwner.h>
#include <osvr/Util/ContainerWrapper.h>

// Library/third-party includes

// Standard includes
#include <vector>
#include <stdexcept>
#include <utility>

namespace osvr {
namespace client {

    namespace detail {
        typedef util::ContainerWrapper<
            std::vector<Viewer>, util::container_policies::const_iterators,
            util::container_policies::subscript> ViewerContainerBase;
    } // namespace detail

    class Viewers : public detail::ViewerContainerBase {
      public:
        Viewers(Viewers const &) = delete;
        Viewers &operator=(Viewers const &) = delete;
        Viewers();
        inline OSVR_EyeCount size() const {
            return static_cast<OSVR_EyeCount>(container().size());
        }

      private:
        friend class DisplayConfigFactory;
    };

} // namespace client
} // namespace osvr
#endif // INCLUDED_Viewers_h_GUID_64108A5C_E8CF_409D_0757_C0A142678C5A
