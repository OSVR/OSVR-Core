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

#ifndef INCLUDED_Projection_h_GUID_C6381CFC_0E5B_44A4_D050_BE836F99BE9C
#define INCLUDED_Projection_h_GUID_C6381CFC_0E5B_44A4_D050_BE836F99BE9C

// Internal Includes
#include <osvr/Util/UniquePtr.h>
#include <osvr/Util/Matrix44C.h>

// Library/third-party includes
#include <boost/noncopyable.hpp>

// Standard includes
#include <vector>
#include <string>

namespace osvr {
namespace client {
    class ComputationalDisplayModel;
    typedef unique_ptr<ComputationalDisplayModel> DisplayModelPtr;
    typedef std::vector<DisplayModelPtr> DisplayModelList;
    class ComputationalDisplayModel : boost::noncopyable {
      public:
        static DisplayModelList
        generateComputationalDisplayModel(std::string const &displayDescriptor);
        ~ComputationalDisplayModel();
        class Impl;

      private:
        ComputationalDisplayModel(std::string const &displayDescriptor);
        unique_ptr<Impl> m_impl;
    };
} // namespace client
} // namespace osvr
#endif // INCLUDED_Projection_h_GUID_C6381CFC_0E5B_44A4_D050_BE836F99BE9C
