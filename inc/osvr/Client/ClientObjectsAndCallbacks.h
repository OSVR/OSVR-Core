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

#ifndef INCLUDED_ClientObjectsAndCallbacks_h_GUID_DEA36722_2353_4980_2CF5_666C5D531004
#define INCLUDED_ClientObjectsAndCallbacks_h_GUID_DEA36722_2353_4980_2CF5_666C5D531004


// Internal Includes
#include <osvr/Common/PathTree.h>
#include <osvr/Common/ClientInterfacePtr.h>
#include <osvr/Client/InterfaceTree.h>

// Library/third-party includes
// - none

// Standard includes
#include <string>


namespace osvr {
    namespace client {

        class ClientObjectsAndCallbacks {
            public:
                void addInterface(common::ClientInterfacePtr const &iface);
                void releaseInterface(common::ClientInterfacePtr const &iface);
                common::PathTree const &getPathTree() const;
            private:
                /// @brief Given a path, remove any existing handler for that path, then
                /// attempt to fully resolve the path to its source and construct a
                /// handler for it.
                ///
                /// @return true if we were able to connect the path.
                bool m_connectCallbacksOnPath(std::string const &path);

                /// @brief Given a path, remove any existing handler for that path from
                /// both the handler container and the interface tree.
                void m_removeCallbacksOnPath(std::string const &path);


                /// @brief Calls m_connectCallbacksOnPath() for every path that has one
                /// or more interface objects but no remote handler.
                void m_connectNeededCallbacks();

                /// @brief Path tree
                common::PathTree m_pathTree;

                /// @brief Tree parallel to path tree for holding interface objects and
                /// remote handlers.
                InterfaceTree m_interfaces;
        };
    } // namespace client
} // namespace osvr

#endif // INCLUDED_ClientObjectsAndCallbacks_h_GUID_DEA36722_2353_4980_2CF5_666C5D531004

