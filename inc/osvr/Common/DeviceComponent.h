/** @file
    @brief Header

    @date 2015

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2015 Sensics, Inc.
//
// All rights reserved.
//
// (Final version intended to be licensed under
// the Apache License, Version 2.0)

#ifndef INCLUDED_DeviceComponent_h_GUID_FC172255_F4F1_41A2_ABA5_E5E33F8BD005
#define INCLUDED_DeviceComponent_h_GUID_FC172255_F4F1_41A2_ABA5_E5E33F8BD005

// Internal Includes
#include <osvr/Common/DeviceComponentPtr.h>
#include <osvr/Common/BaseDevicePtr.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    class DeviceComponent;
    typedef shared_ptr<DeviceComponent> DeviceComponentPtr;

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

        /// @brief Called once when we have a parent
        virtual void m_parentSet() = 0;

        /// @brief Implementation-specific (optional) stuff to do during
        /// mainloop
        virtual void m_update();

      private:
        Parent *m_parent;
    };
} // namespace common
} // namespace osvr
#endif // INCLUDED_DeviceComponent_h_GUID_FC172255_F4F1_41A2_ABA5_E5E33F8BD005
