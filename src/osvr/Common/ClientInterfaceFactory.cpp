/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/ClientInterfaceFactory.h>
#include <osvr/Common/ClientInterface.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace {
        /// Subclass solely for the purpose of make_shared capability.
        struct StandardClientInterface : public ClientInterface {
          public:
            StandardClientInterface(ClientContext &ctx, std::string const &path)
                : ClientInterface(ctx, path) {}
        };
    } // namespace
    ClientInterfaceFactory getStandardClientInterfaceFactory() {
        return [](ClientContext &ctx, const char path[]) {
            ClientInterfacePtr ret;
            if (!path) {
                return ret;
            }
            std::string p(path);
            if (p.empty()) {
                return ret;
            }
            ret = make_shared<StandardClientInterface>(ctx, p);
            return ret;
        };
    }
} // namespace common
} // namespace osvr
