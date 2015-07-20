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
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_DeviceComponent_h_GUID_FC172255_F4F1_41A2_ABA5_E5E33F8BD005
#define INCLUDED_DeviceComponent_h_GUID_FC172255_F4F1_41A2_ABA5_E5E33F8BD005

// Internal Includes
#include <osvr/Common/DeviceComponentPtr.h>
#include <osvr/Common/BaseDevicePtr.h>
#include <osvr/Common/MessageHandler.h>
#include <osvr/Common/BaseMessageTraits.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    class DeviceComponent {
      public:
        typedef BaseDevice Parent;

        /// @brief Called (only) by BaseDevice when being added: effectively
        /// records a "parent pointer" that does not convey ownership.
        void recordParent(Parent &dev);

        /// @brief Called during mainloop
        void update();

      protected:
        /// @brief Protected constructor, to require subclassing
        DeviceComponent();

        /// @brief Returns true if we have a parent registered.
        bool m_hasParent() const;

        /// @brief Gets the parent - only call if m_hasParent() is true
        Parent &m_getParent();

        /// @brief Registers a handler whose lifetime is tied to the lifetime of
        /// the component.
        ///
        /// Only call if m_hasParent() is true.
        ///
        /// Your handler should return 0 unless something is horribly wrong.
        void m_registerHandler(vrpn_MESSAGEHANDLER handler, void *userdata,
                               RawMessageType const &msgType);

        /// @brief Called once when we have a parent
        virtual void m_parentSet() = 0;

        /// @brief Implementation-specific (optional) stuff to do during
        /// mainloop
        virtual void m_update();

      private:
        Parent *m_parent;
        MessageHandlerList<BaseDeviceMessageHandleTraits> m_messageHandlers;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_DeviceComponent_h_GUID_FC172255_F4F1_41A2_ABA5_E5E33F8BD005
