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
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_DummyTree_h_GUID_8ED7ACE5_0B4B_4B8A_37E8_E041714F0216
#define INCLUDED_DummyTree_h_GUID_8ED7ACE5_0B4B_4B8A_37E8_E041714F0216

// Internal Includes
#include <osvr/Common/PathTreeFull.h>
#include <osvr/Common/PathElementTypes.h>

// Library/third-party includes
#include <boost/lexical_cast.hpp>

// Standard includes
#include <string>

namespace dummy {
namespace common = osvr::common;
using osvr::common::PathTree;
using namespace osvr::common::elements;

inline std::string getPlugin() { return "com_osvr_bundled_Multiserver"; }

inline std::string getPluginPath() { return "/" + getPlugin(); }

inline std::string getDevice() { return getPlugin() + "/YEI_3Space_Sensor0"; }

inline std::string getDevicePath() { return "/" + getDevice(); }

inline std::string getHost() { return "localhost:3883"; }
inline std::string getInterface() { return "tracker"; }

inline std::string getInterfacePath() {
    return getDevicePath() + "/" + getInterface();
}

inline int getSensor() { return 1; }

inline std::string getFullSourcePath() {
    return getInterfacePath() + "/" +
           boost::lexical_cast<std::string>(getSensor());
}

inline std::string getAlias() { return "/me/hands/left"; }

inline void setupDummyDevice(PathTree &tree) {
    tree.getNodeByPath(getPluginPath(), PluginElement());
    tree.getNodeByPath(getDevicePath(), DeviceElement::createVRPNDeviceElement(
                                            getDevice(), getHost()));
    tree.getNodeByPath(getInterfacePath(), InterfaceElement());
}
inline void setupRawAlias(PathTree &tree) {
    tree.getNodeByPath(getAlias(), AliasElement(getFullSourcePath()));
}

inline void setupDummyTree(PathTree &tree) {
    setupDummyDevice(tree);
    setupRawAlias(tree);
}
} // namespace dummy

using dummy::setupDummyTree;
using dummy::getFullSourcePath;

#endif // INCLUDED_DummyTree_h_GUID_8ED7ACE5_0B4B_4B8A_37E8_E041714F0216
