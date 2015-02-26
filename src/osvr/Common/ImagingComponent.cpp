/** @file
    @brief Implementation

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

// Internal Includes
#include <osvr/Common/ImagingComponent.h>
#include <osvr/Common/BaseDevice.h>
#include <osvr/Common/Serialization.h>
#include <osvr/Common/Buffer.h>

#include <osvr/Util/Verbosity.h>

// Library/third-party includes
// - none

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {
        class ImageRegion::MessageSerialization {
          public:
            MessageSerialization(std::string const &str = std::string())
                : m_str(str) {}

            template <typename T> void processMessage(T &p) {
                p(m_str, serialization::StringOnlyMessageTag());
            }

          private:
            std::string m_str;
        };
        const char *ImageRegion::identifier() {
            return "com.osvr.imaging.imageregion";
        }
    } // namespace messages

    shared_ptr<ImagingComponent>
    ImagingComponent::create(OSVR_ChannelCount numChan) {
        shared_ptr<ImagingComponent> ret(new ImagingComponent(numChan));
        return ret;
    }
    ImagingComponent::ImagingComponent(OSVR_ChannelCount numChan)
        : m_numChan(numChan) {}

    void ImagingComponent::sendImageData() {
        Buffer<> buf;
        messages::ImageRegion::MessageSerialization msg;
        serialize(buf, msg);
        m_getParent().packMessage(buf, imageRegion.getMessageType());
    }

    void
    ImagingComponent::registerImageRegionHandler(vrpn_MESSAGEHANDLER handler,
                                                 void *userdata) {
        m_registerHandler(handler, userdata, imageRegion.getMessageType());
    }
    void ImagingComponent::m_parentSet() {
        OSVR_DEV_VERBOSE("Finishing init of imaging component");
        m_getParent().registerMessageType(imageRegion);
    }
} // namespace common
} // namespace osvr