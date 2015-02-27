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
#include <osvr/Util/OpenCVTypeDispatch.h>

#include <osvr/Util/Verbosity.h>

// Library/third-party includes
#include <opencv2/core/core.hpp>

// Standard includes
// - none

namespace osvr {
namespace common {
    namespace messages {
        class ImageRegion::MessageSerialization {
          public:
            MessageSerialization(OSVR_ImagingMetadata const &meta,
                                 OSVR_ImageBufferElement *imageData,
                                 OSVR_ChannelCount sensor)
                : m_meta(meta),
                  m_imgBuf(imageData,
                           [](OSVR_ImageBufferElement *) {
                           }), // That's a null-deleter right there for you.
                  m_sensor(sensor) {}

            MessageSerialization() : m_imgBuf(nullptr) {}

            template <typename T>
            void allocateBuffer(T &p, size_t bytes, std::true_type const &) {
                m_imgBuf.reset(reinterpret_cast<OSVR_ImageBufferElement *>(
                                   cv::fastMalloc(bytes)),
                               &cv::fastFree);
            }

            template <typename T>
            void allocateBuffer(T &, size_t, std::false_type const &) {
                // Does nothing if we're serializing.
            }

            template <typename T> void processMessage(T &p) {
                p(m_meta.height);
                p(m_meta.width);
                p(m_meta.channels);
                p(m_meta.depth);
                p(m_meta.type,
                  serialization::EnumAsIntegerTag<OSVR_ImagingValueType,
                                                  uint8_t>());

                auto bytes = m_meta.height * m_meta.width * m_meta.depth *
                             m_meta.channels;

                /// Allocate the matrix backing data, if we're deserializing
                /// only.
                allocateBuffer(p, bytes, p.isDeserialize());
                p(m_imgBuf.get(),
                  serialization::AlignedDataBufferTag(bytes, m_meta.depth));
            }

          private:
            OSVR_ImagingMetadata m_meta;
            shared_ptr<OSVR_ImageBufferElement> m_imgBuf;
            OSVR_ChannelCount m_sensor;
            cv::Mat m_mat;
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
        : m_numSensor(numChan) {}

    void ImagingComponent::sendImageData(OSVR_ImagingMetadata metadata,
                                         OSVR_ImageBufferElement *imageData,
                                         OSVR_ChannelCount sensor,
                                         OSVR_TimeValue const &timestamp) {
        Buffer<> buf;
        messages::ImageRegion::MessageSerialization msg(metadata, imageData,
                                                        sensor);
        serialize(buf, msg);
        m_getParent().packMessage(buf, imageRegion.getMessageType(), timestamp);
        m_getParent().sendPending();
    }

    int VRPN_CALLBACK
    ImagingComponent::handleImageRegion(void *userdata, vrpn_HANDLERPARAM p) {
        auto bufwrap = ExternalBufferReadingWrapper<unsigned char>(
            reinterpret_cast<unsigned char const *>(p.buffer), p.payload_len);
        auto bufReader = BufferReader<decltype(bufwrap)>(bufwrap);

        messages::ImageRegion::MessageSerialization msg;
        deserialize(bufReader, msg);
        return 0;
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