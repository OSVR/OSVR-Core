/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/ClientInterface.h>
#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
#include <boost/range/algorithm.hpp>

OSVR_ClientInterfaceObject::OSVR_ClientInterfaceObject(
    ::osvr::common::ClientContext &ctx, std::string const &path)
    : m_ctx(ctx), m_path(path) {
    OSVR_DEV_VERBOSE("Interface initialized for " << m_path);
}

std::string const &OSVR_ClientInterfaceObject::getPath() const {
    return m_path;
}

void OSVR_ClientInterfaceObject::update() {}
